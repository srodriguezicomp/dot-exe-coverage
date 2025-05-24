#pragma once
#include <stddef.h>

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
char* get_value_from_json(
    const char* json_str, 
    const char** keys, 
    size_t keys_count
);