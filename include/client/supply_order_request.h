#pragma once
#include <stdlib.h>
#include <client_menu.h>
#include <client_globals.h>
#include <client_utils.h>
#include <string.h>

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
char* craft_supply_message(
    const char* message_type, 
    int water_ammount, 
    int weaponry_ammount, 
    int food_ammount, 
    int medicine_ammount, 
    int shipment_id
);

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
int get_supply_value_by_index(
    int index
);

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
int set_supply_value_by_index(
    int index, 
    int value
);

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
void add_supply_value_by_index(
    int index, 
    int value
);

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
int subtract_supply_value_by_index(
    int index, 
    int value
);

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
void request_supplies_header();

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
void request_supplies(
    char input[256]
);

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
void handle_supply_order_request(
    char* recv_buf
);

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
void handle_supply_send(
    char* recv_buf
);
