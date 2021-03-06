/*
 * Copyright (c) 2012 ARM Limited
 * All rights reserved
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Andreas Sandberg
 */

#ifndef __CPU_KVM_BASE_HH__
#define __CPU_KVM_BASE_HH__

#include <memory>

#include "base/statistics.hh"
#include "cpu/kvm/perfevent.hh"
#include "cpu/kvm/timer.hh"
#include "cpu/kvm/vm.hh"
#include "cpu/base.hh"
#include "cpu/simple_thread.hh"

/** Signal to use to trigger time-based exits from KVM */
#define KVM_TIMER_SIGNAL SIGRTMIN

// forward declarations
class ThreadContext;
struct BaseKvmCPUParams;

/**
 * Base class for KVM based CPU models
 *
 * All architecture specific KVM implementation should inherit from
 * this class. The most basic CPU models only need to override the
 * updateKvmState() and updateThreadContext() methods to implement
 * state synchronization between gem5 and KVM.
 *
 * The architecture specific implementation is also responsible for
 * delivering interrupts into the VM. This is typically done by
 * overriding tick() and checking the thread context before entering
 * into the VM. In order to deliver an interrupt, the implementation
 * then calls KvmVM::setIRQLine() or BaseKvmCPU::kvmInterrupt()
 * depending on the specifics of the underlying hardware/drivers.
 */
class BaseKvmCPU : public BaseCPU
{
  public:
    BaseKvmCPU(BaseKvmCPUParams *params);
    virtual ~BaseKvmCPU();

    void init();
    void startup();
    void regStats();

    void serializeThread(std::ostream &os, ThreadID tid);
    void unserializeThread(Checkpoint *cp, const std::string &section,
                           ThreadID tid);

    unsigned int drain(DrainManager *dm);
    void drainResume();

    void switchOut();
    void takeOverFrom(BaseCPU *cpu);

    void verifyMemoryMode() const;

    CpuPort &getDataPort() { return dataPort; }
    CpuPort &getInstPort() { return instPort; }

    void wakeup();
    void activateContext(ThreadID thread_num, Cycles delay);
    void suspendContext(ThreadID thread_num);
    void deallocateContext(ThreadID thread_num);
    void haltContext(ThreadID thread_num);

    ThreadContext *getContext(int tn);

    Counter totalInsts() const;
    Counter totalOps() const;

    /** Dump the internal state to the terminal. */
    virtual void dump();

    /**
     * A cached copy of a thread's state in the form of a SimpleThread
     * object.
     *
     * Normally the actual thread state is stored in the KVM vCPU. If KVM has
     * been running this copy is will be out of date. If we recently handled
     * some events within gem5 that required state to be updated this could be
     * the most up-to-date copy. When getContext() or updateThreadContext() is
     * called this copy gets updated.  The method syncThreadContext can
     * be used within a KVM CPU to update the thread context if the
     * KVM state is dirty (i.e., the vCPU has been run since the last
     * update).
     */
    SimpleThread *thread;

    /** ThreadContext object, provides an interface for external
     * objects to modify this thread's state.
     */
    ThreadContext *tc;

    KvmVM &vm;

  protected:
    enum Status {
        /** Context not scheduled in KVM */
        Idle,
        /** Running normally */
        Running,
    };

    /** CPU run state */
    Status _status;

    /**
     * Execute the CPU until the next event in the main event queue or
     * until the guest needs service from gem5.
     *
     * @note This method is virtual in order to allow implementations
     * to check for architecture specific events (e.g., interrupts)
     * before entering the VM.
     */
    virtual void tick();

    /**
     * Request KVM to run the guest for a given number of ticks. The
     * method returns the approximate number of ticks executed.
     *
     * @note The returned number of ticks can be both larger or
     * smaller than the requested number of ticks. A smaller number
     * can, for example, occur when the guest executes MMIO. A larger
     * number is typically due to performance counter inaccuracies.
     *
     * @param ticks Number of ticks to execute
     * @return Number of ticks executed (see note)
     */
    Tick kvmRun(Tick ticks);

    /**
     * Get a pointer to the kvm_run structure containing all the input
     * and output parameters from kvmRun().
     */
    struct kvm_run *getKvmRunState() { return _kvmRun; };

    /**
     * Retrieve a pointer to guest data stored at the end of the
     * kvm_run structure. This is mainly used for PIO operations
     * (KVM_EXIT_IO).
     *
     * @param offset Offset as specified by the kvm_run structure
     * @return Pointer to guest data
     */
    uint8_t *getGuestData(uint64_t offset) const {
        return (uint8_t *)_kvmRun + offset;
    };

    /**
     * @addtogroup KvmInterrupts
     * @{
     */
    /**
     * Send a non-maskable interrupt to the guest
     *
     * @note The presence of this call depends on Kvm::capUserNMI().
     */
    void kvmNonMaskableInterrupt();

    /**
     * Send a normal interrupt to the guest
     *
     * @note Make sure that ready_for_interrupt_injection in kvm_run
     * is set prior to calling this function. If not, an interrupt
     * window must be requested by setting request_interrupt_window in
     * kvm_run to 1 and restarting the guest.
     *
     * @param interrupt Structure describing the interrupt to send
     */
    void kvmInterrupt(const struct kvm_interrupt &interrupt);

    /** @} */

    /** @{ */
    /**
     * Get/Set the register state of the guest vCPU
     *
     * KVM has two different interfaces for accessing the state of the
     * guest CPU. One interface updates 'normal' registers and one
     * updates 'special' registers. The distinction between special
     * and normal registers isn't very clear and is architecture
     * dependent.
     */
    void getRegisters(struct kvm_regs &regs) const;
    void setRegisters(const struct kvm_regs &regs);
    void getSpecialRegisters(struct kvm_sregs &regs) const;
    void setSpecialRegisters(const struct kvm_sregs &regs);
    /** @} */

    /** @{ */
    /**
     * Get/Set the guest FPU/vector state
     */
    void getFPUState(struct kvm_fpu &state) const;
    void setFPUState(const struct kvm_fpu &state);
    /** @} */

    /** @{ */
    /**
     * Get/Set single register using the KVM_(SET|GET)_ONE_REG API.
     *
     * @note The presence of this call depends on Kvm::capOneReg().
     */
    void setOneReg(uint64_t id, const void *addr);
    void setOneReg(uint64_t id, uint64_t value) { setOneReg(id, &value); }
    void setOneReg(uint64_t id, uint32_t value) { setOneReg(id, &value); }
    void getOneReg(uint64_t id, void *addr) const;
    uint64_t getOneRegU64(uint64_t id) const {
        uint64_t value;
        getOneReg(id, &value);
        return value;
    }
    uint32_t getOneRegU32(uint64_t id) const {
        uint32_t value;
        getOneReg(id, &value);
        return value;
    }
    /** @} */

    /**
     * Get and format one register for printout.
     *
     * This function call getOneReg() to retrieve the contents of one
     * register and automatically formats it for printing.
     *
     * @note The presence of this call depends on Kvm::capOneReg().
     */
    std::string getAndFormatOneReg(uint64_t id) const;

    /** @{ */
    /**
     * Update the KVM state from the current thread context
     *
     * The base CPU calls this method before starting the guest CPU
     * when the contextDirty flag is set. The architecture dependent
     * CPU implementation is expected to update all guest state
     * (registers, special registers, and FPU state).
     */
    virtual void updateKvmState() = 0;

    /**
     * Update the current thread context with the KVM state
     *
     * The base CPU after the guest updates any of the KVM state. In
     * practice, this happens after kvmRun is called. The architecture
     * dependent code is expected to read the state of the guest CPU
     * and update gem5's thread state.
     */
    virtual void updateThreadContext() = 0;

    /**
     * Update a thread context if the KVM state is dirty with respect
     * to the cached thread context.
     */
    void syncThreadContext();

    /**
     * Update the KVM if the thread context is dirty.
     */
    void syncKvmState();
    /** @} */

    /** @{ */
    /**
     * Main kvmRun exit handler, calls the relevant handleKvmExit*
     * depending on exit type.
     *
     * @return Number of ticks spent servicing the exit request
     */
    virtual Tick handleKvmExit();

    /**
     * The guest performed a legacy IO request (out/inp on x86)
     *
     * @return Number of ticks spent servicing the IO request
     */
    virtual Tick handleKvmExitIO();

    /**
     * The guest requested a monitor service using a hypercall
     *
     * @return Number of ticks spent servicing the hypercall
     */
    virtual Tick handleKvmExitHypercall();

    /**
     * The guest exited because an interrupt window was requested
     *
     * The guest exited because an interrupt window was requested
     * (request_interrupt_window in the kvm_run structure was set to 1
     * before calling kvmRun) and it is now ready to receive
     *
     * @return Number of ticks spent servicing the IRQ
     */
    virtual Tick handleKvmExitIRQWindowOpen();

    /**
     * An unknown architecture dependent error occurred when starting
     * the vCPU
     *
     * The kvm_run data structure contains the hardware error
     * code. The defaults behavior of this method just prints the HW
     * error code and panics. Architecture dependent implementations
     * may want to override this method to provide better,
     * hardware-aware, error messages.
     *
     * @return Number of ticks delay the next CPU tick
     */
    virtual Tick handleKvmExitUnknown();

    /**
     * An unhandled virtualization exception occured
     *
     * Some KVM virtualization drivers return unhandled exceptions to
     * the user-space monitor. This interface is currently only used
     * by the Intel VMX KVM driver.
     *
     * @return Number of ticks delay the next CPU tick
     */
    virtual Tick handleKvmExitException();

    /**
     * KVM failed to start the virtualized CPU
     *
     * The kvm_run data structure contains the hardware-specific error
     * code.
     *
     * @return Number of ticks delay the next CPU tick
     */
    virtual Tick handleKvmExitFailEntry();
    /** @} */

    /**
     * Inject a memory mapped IO request into gem5
     *
     * @param paddr Physical address
     * @param data Pointer to the source/destination buffer
     * @param size Memory access size
     * @param write True if write, False if read
     * @return Number of ticks spent servicing the memory access
     */
    Tick doMMIOAccess(Addr paddr, void *data, int size, bool write);


    /**
     * @addtogroup KvmIoctl
     * @{
     */
    /**
     * vCPU ioctl interface.
     *
     * @param request KVM vCPU request
     * @param p1 Optional request parameter
     *
     * @return -1 on error (error number in errno), ioctl dependent
     * value otherwise.
     */
    int ioctl(int request, long p1) const;
    int ioctl(int request, void *p1) const {
        return ioctl(request, (long)p1);
    }
    int ioctl(int request) const {
        return ioctl(request, 0L);
    }
    /** @} */

    /** Port for data requests */
    CpuPort dataPort;

    /** Unused dummy port for the instruction interface */
    CpuPort instPort;

    /** Pre-allocated MMIO memory request */
    Request mmio_req;

    /**
     * Is the gem5 context dirty? Set to true to force an update of
     * the KVM vCPU state upon the next call to kvmRun().
     */
    bool threadContextDirty;

    /**
     * Is the KVM state dirty? Set to true to force an update of
     * the KVM vCPU state upon the next call to kvmRun().
     */
    bool kvmStateDirty;

    /** KVM internal ID of the vCPU */
    const long vcpuID;

  private:
    struct TickEvent : public Event
    {
        BaseKvmCPU &cpu;

        TickEvent(BaseKvmCPU &c)
            : Event(CPU_Tick_Pri), cpu(c) {}

        void process() { cpu.tick(); }

        const char *description() const {
            return "BaseKvmCPU tick";
        }
    };

    /**
     * Service MMIO requests in the mmioRing.
     *
     *
     * @return Number of ticks spent servicing the MMIO requests in
     * the MMIO ring buffer
     */
    Tick flushCoalescedMMIO();

    /**
     * Setup a signal handler to catch the timer signal used to
     * switch back to the monitor.
     */
    void setupSignalHandler();

    /** Setup hardware performance counters */
    void setupCounters();

    /** KVM vCPU file descriptor */
    int vcpuFD;
    /** Size of MMAPed kvm_run area */
    int vcpuMMapSize;
    /**
     * Pointer to the kvm_run structure used to communicate parameters
     * with KVM.
     *
     * @note This is the base pointer of the MMAPed KVM region. The
     * first page contains the kvm_run structure. Subsequent pages may
     * contain other data such as the MMIO ring buffer.
     */
    struct kvm_run *_kvmRun;
    /**
     * Coalesced MMIO ring buffer. NULL if coalesced MMIO is not
     * supported.
     */
    struct kvm_coalesced_mmio_ring *mmioRing;
    /** Cached page size of the host */
    const long pageSize;

    TickEvent tickEvent;

    /** @{ */
    /** Guest performance counters */
    PerfKvmCounter hwCycles;
    PerfKvmCounter hwInstructions;
    /** @} */

    /**
     * Does the runTimer control the performance counters?
     *
     * The run timer will automatically enable and disable performance
     * counters if a PerfEvent-based timer is used to control KVM
     * exits.
     */
    bool perfControlledByTimer;

    /**
     * Timer used to force execution into the monitor after a
     * specified number of simulation tick equivalents have executed
     * in the guest. This counter generates the signal specified by
     * KVM_TIMER_SIGNAL.
     */
    std::unique_ptr<BaseKvmTimer> runTimer;

    float hostFactor;

  public:
    /* @{ */
    Stats::Scalar numInsts;
    Stats::Scalar numVMExits;
    Stats::Scalar numMMIO;
    Stats::Scalar numCoalescedMMIO;
    Stats::Scalar numIO;
    Stats::Scalar numHalt;
    Stats::Scalar numInterrupts;
    Stats::Scalar numHypercalls;
    /* @} */
};

#endif
