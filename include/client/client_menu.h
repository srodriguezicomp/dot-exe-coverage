#pragma once
#include <uv.h>

/**
 * @brief
 * 
 * Function description...
 * 
 * Example:
 * @code
 * functionName(); // Output:
 * @endcode
 * 
*/
void show_main_menu();

/**
 * @brief
 * 
 * Function description...
 * 
 * Example:
 * @code
 * functionName(); // Output:
 * @endcode
 * 
*/
void on_stdin_event(
    uv_poll_t* req, 
    int status, 
    int events
);
