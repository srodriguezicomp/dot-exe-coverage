#pragma once
#include <uv.h>

/**
 * @brief Corre el cliente.
 * 
 * Esta funcion corre el cliente.
 * 
 * Example:
 * @code
 * run_client(); // Output:
 * @endcode
 * 
*/
void on_socket_event(
    uv_poll_t* req, 
    int status, 
    int events
);

/**
 * @brief Corre el cliente.
 * 
 * Esta funcion corre el cliente.
 * 
 * Example:
 * @code
 * run_client(); // Output:
 * @endcode
 * 
*/
void set_non_blocking(
    int fd
);

/**
 * @brief Corre el cliente.
 * 
 * Esta funcion corre el cliente.
 * 
 * Example:
 * @code
 * run_client(); // Output:
 * @endcode
 * 
*/
int run_client(
    int cant, 
    int supply, 
    char* user, 
    char* password);