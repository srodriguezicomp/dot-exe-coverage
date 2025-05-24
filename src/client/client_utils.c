#include <client_utils.h>
#include "cjson/cJSON.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* get_value_from_json(
    const char* json_str, 
    const char** keys, 
    size_t keys_count
) {
    // Parsear el JSON
    cJSON* root = cJSON_Parse(json_str);
    if (!root) {
        fprintf(stderr, "Error parsing JSON\n");
        return NULL;
    }

    cJSON* current = root;

    // Recorrer las claves en el arreglo
    for (size_t i = 0; i < keys_count; ++i) {
        current = cJSON_GetObjectItem(current, keys[i]);
        if (!current) {
            fprintf(stderr, "No se encontró el campo '%s'\n", keys[i]);
            cJSON_Delete(root);
            return NULL;
        }
    }

    // Verificar si el último elemento es una cadena
    if (cJSON_IsString(current)) {
        char* value = strdup(current->valuestring);
        cJSON_Delete(root);
        return value;
    }
    
    if (cJSON_IsNumber(current)) {
        char value[12];
        snprintf(value, 12, "%d", current->valueint);
        cJSON_Delete(root);
        return strdup(value);
    }

    fprintf(stderr, "Item is not a string or number\n");
    cJSON_Delete(root);
    return NULL;
}
