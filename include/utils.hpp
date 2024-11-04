#pragma once

// If given condition is true exit() the program, printing given messsage
#define ERROR_EXIT(cond, msg)                                                                           \
    if (cond) {                                                                                         \
        std::cerr << "Error in " << __FILE__ << " line " << __LINE__ << " --- " << msg << std::endl;    \
        exit(EXIT_FAILURE);                                                                             \
    }
