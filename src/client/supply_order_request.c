#include <supply_order_request.h>
#include "cjson/cJSON.h"

// #define UNIT_TEST

// #ifdef UNIT_TEST

// // Provide a dummy implementation of send that just returns the number of bytes
// ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
//     return len;
// }
// #endif

char* craft_supply_message(
    const char* message_type, 
    int water_ammount, 
    int weaponry_ammount, 
    int food_ammount, 
    int medicine_ammount, 
    int shipment_id
) {
    cJSON* root = cJSON_CreateObject();
    cJSON* body = cJSON_CreateObject();
    cJSON* inventory = cJSON_CreateObject();

    cJSON_AddItemToObject(root, "messageType", cJSON_CreateString(message_type));
    cJSON_AddItemToObject(inventory, "waterBox", cJSON_CreateNumber(water_ammount));
    cJSON_AddItemToObject(inventory, "weaponryBox", cJSON_CreateNumber(weaponry_ammount));
    cJSON_AddItemToObject(inventory, "foodBox", cJSON_CreateNumber(food_ammount));
    cJSON_AddItemToObject(inventory, "medicineBox", cJSON_CreateNumber(medicine_ammount));

    cJSON_AddItemToObject(body, "inventory", inventory);
    cJSON_AddItemToObject(body, "shipmentId", cJSON_CreateNumber(shipment_id));

    cJSON_AddItemToObject(root, "body", body);

    char* json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);

    return json_str;
}

int get_supply_value_by_index(
    int index
) {
    switch (index) {
        case 1: return food_box;
        case 2: return medicine_box;
        case 3: return water_box;
        case 4: return weaponry_box;
        default: return -1; // Valor inválido
    }
}

int set_supply_value_by_index(
    int index, 
    int value
) {
    switch (index) {
        case 1: food_box = value; break;
        case 2: medicine_box = value; break;
        case 3: water_box = value; break;
        case 4: weaponry_box = value; break;
        default: return -1; // Valor inválido
    }
    return 0; // Éxito
}

void add_supply_value_by_index(
    int index, 
    int value
) {
    switch (index) {
        case 1: food_box += value; break;
        case 2: medicine_box += value; break;
        case 3: water_box += value; break;
        case 4: weaponry_box += value; break;
        default: break; // Valor inválido
    }
}

int subtract_supply_value_by_index(
    int index, 
    int value
) {
    int* supply = NULL;
    switch(index) {
        case 1: supply = &food_box; break;
        case 2: supply = &medicine_box; break;
        case 3: supply = &water_box; break;
        case 4: supply = &weaponry_box; break;
        default: return 0; // Invalid index, nothing subtracted
    }
    int subtracted = (value > *supply) ? *supply : value;
    *supply -= subtracted;
    return subtracted;
}

void request_supplies_header(){
    printf("\n--- REQUEST SUPPLIES ---\n");
    printf("The supplies that can be requested are the following:\n");
    printf("- foodBox\n- medicineBox\n- waterBox\n- weaponryBox\n");
    printf("Specify the quantity to request of each of the forms: foodBoxQty|medicineBoxQty|waterBoxQty|weaponryBoxQty\n");
}

void request_supplies(
    char input[256]
){
    input[strcspn(input, "\n")] = 0; // Eliminar salto de línea
    if (strlen(input) > 0) {
        char msg[512];

        int food_box_qty, medicine_box_qty, water_box_qty, weaponry_box_qty;

        char *token = strtok(input, "|");
        if (token != NULL) food_box_qty = atoi(token);

        token = strtok(NULL, "|");
        if (token != NULL) medicine_box_qty = atoi(token);

        token = strtok(NULL, "|");
        if (token != NULL) water_box_qty = atoi(token);

        token = strtok(NULL, "|");
        if (token != NULL) weaponry_box_qty = atoi(token);
        
        if (food_box_qty && medicine_box_qty && water_box_qty && weaponry_box_qty) {
            snprintf(msg, sizeof(msg),
                "{\"messageType\":\"SUPPLY_ORDER\",\"body\":{\"inventory\":{\"foodBox\":%d,\"medicineBox\":%d,\"weaterBox\":%d,\"weaponryBox\":%d}}}",
                food_box_qty, medicine_box_qty, water_box_qty, weaponry_box_qty);

            send(sock_fd, msg, strlen(msg), 0);
            printf("Supply order request sent\n");
        } else {
            printf("Error: Input format is incorrect. Expected format: foodBoxQty|medicineBoxQty|waterBoxQty|weaponryBoxQty\n");
        }
        
        show_main_menu();
    } else {
        printf("Empty message. Nothing sent.\n");
        show_main_menu();
    }
    menu_level = 0;
}

void handle_supply_order_request(
    char* recv_buf
){
    char response[512];
    int value = 0, shipment_id = 0;

    // sscanf(recv_buf, "{\"messageType\":\"supply_order_request\",\"body\":{\"inventory\":{\"waterBox\":%d,\"ammoBox\":%d,\"foodBox\":%d,\"medicineBox\":%d}shipmentId\":%d}}", &water_box, &ammoBox, &food_box, &medicine_box, &shipment_id);

    printf("Message received: supply_order_request\n");
    value  = atoi(get_value_from_json(recv_buf, (const char*[]){"body", "inventory", "waterBox"}, 3)); // Si get_value_from_json devuelve NULL, atoi da segmentation fault
    int water_value = subtract_supply_value_by_index(WATER_BOX, value);
    printf("Water value sucess\n");
    value  = atoi(get_value_from_json(recv_buf, (const char*[]){"body", "inventory", "weaponryBox"}, 3));
    int weaponry_value = subtract_supply_value_by_index(WEAPONRY_BOX, value);
    printf("Weaponry value success\n");
    value  = atoi(get_value_from_json(recv_buf, (const char*[]){"body", "inventory", "foodBox"}, 3));
    int food_value = subtract_supply_value_by_index(FOOD_BOX, value);
    printf("Food value success\n");
    value  = atoi(get_value_from_json(recv_buf, (const char*[]){"body", "inventory", "medicineBox"}, 3));
    int medicine_value = subtract_supply_value_by_index(MEDICINE_BOX, value);
    printf("Medicine value success\n");
    shipment_id  = atoi(get_value_from_json(recv_buf, (const char*[]){"body", "shipmentId"}, 2));

    // snprintf(response, sizeof(response),
    //     "{\"requestType\":\"supply_order_request_done\",\"body\":{\"waterBox\":%d,\"ammoBox\":%d,\"foodBox\":%d,\"medicineBox\":%d,\"shipmentId\":%d}}",
    //     water_value, ammoValue, food_value, medicine_value, shipment_id);
    craft_supply_message("supply_order_confirmation", water_value, weaponry_value, food_value, medicine_value, shipment_id);
    send(sock_fd, response, strlen(response), 0);
}

void handle_supply_send(
    char* recv_buf
){
    char response[512];
    int value = 0, shipment_id = 0;

    printf("Message received: supply_send\n");
    value  = atoi(get_value_from_json(recv_buf, (const char*[]){"body", "inventory", "waterBox"}, 3));
    add_supply_value_by_index(WATER_BOX, value);
    printf("Water value success\n");
    value  = atoi(get_value_from_json(recv_buf, (const char*[]){"body", "inventory", "weaponryBox"}, 3));
    add_supply_value_by_index(WEAPONRY_BOX, value);
    printf("Weaponry value success\n");
    value  = atoi(get_value_from_json(recv_buf, (const char*[]){"body", "inventory", "foodBox"}, 3));
    add_supply_value_by_index(FOOD_BOX, value);
    printf("Food value success\n");
    value  = atoi(get_value_from_json(recv_buf, (const char*[]){"body", "inventory", "medicineBox"}, 3));
    add_supply_value_by_index(MEDICINE_BOX, value);
    printf("Medicine value success\n");
    shipment_id  = atoi(get_value_from_json(recv_buf, (const char*[]){"body", "shipmentId"}, 2));

    craft_supply_message("supply_order_confirmation", 0, 0, 0, 0, shipment_id);
    send(sock_fd, response, strlen(response), 0);
}
