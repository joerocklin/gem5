#ifndef __WARPED_UTILS_HH__
#define __EARPED_UTILS_HH__

#ifdef DEBUG
# define print_method_string { std::cout << "-- " << __FILE__ << ":" << __LINE__ << " - " << __FUNCTION__; }
# define enter_method { print_method_string; std::cout << std::endl; }
# define method_with_id(id) { print_method_string; std::cout << " ID: " << id << std::endl; }
#else
# define print_method_string 
# define enter_method
# define method_with_id(id)
#endif

#endif