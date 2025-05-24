#include <warehouse_register_request.h>
#include <client_globals.h>
#include <client_menu.h>
#include <string.h>
#include <stdlib.h>

void create_warehouse_client_header(){
    printf("\n--- CREATE WAREHOUSE CLIENT ---\n");
    printf("Specify: username|password\n");
}

void create_warehouse_client(
    char input[256]
){
    input[strcspn(input, "\n")] = 0; // Eliminar salto de línea
    if (strlen(input) > 0) {
        char msg[512];

        char *user = strtok(input, "|");
        char *password = strtok(NULL, "|");
        
        if (user && password) {
            snprintf(msg, sizeof(msg),
                "{\"requestType\":\"warehouse_register\",\"body\":{\"user\":\"%s\",\"password\":\"%s\"}}",
                user, password);

            send(sock_fd, msg, strlen(msg), 0);
            printf("Warehouse client creation request sent\n");
        } else {
            printf("Error: Input format is incorrect. Expected format: user|password\n");
        }
        
        show_main_menu();
    } else {
        printf("Empty message. Nothing sent.\n");
        show_main_menu();
    }
    menu_level = 0;
}