#include "cJSON.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* create_warehouse_register_message(const char* user, const char* password) {
    // Create the root JSON object
    cJSON* root = cJSON_CreateObject();
    cJSON* body = cJSON_CreateObject();

    // Add fields to the JSON object
    cJSON_AddItemToObject(root, "requestType", cJSON_CreateString("warehouse_register"));
    cJSON_AddItemToObject(body, "user", cJSON_CreateString(user));
    cJSON_AddItemToObject(body, "password", cJSON_CreateString(password));
    cJSON_AddItemToObject(root, "body", body);

    // Convert the JSON object to a string
    char* json_str = cJSON_Print(root);

    // Free the JSON object
    cJSON_Delete(root);

    return json_str; // Caller is responsible for freeing this memory
}

int main() {
    const char* user = "example_user";
    const char* password = "example_password";

    char* message = create_warehouse_register_message(user, password);
    printf("Generated JSON Message:\n%s\n", message);

    // Free the allocated memory
    free(message);

    return 0;
}