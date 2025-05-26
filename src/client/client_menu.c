#include <client_menu.h>
#include <client_globals.h>
#include <client_supply.h>
#include <supply_order_request.h>
#include <inventory_request.h>
#include <alert_request.h>
#include <warehouse_register_request.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

void show_main_menu(){

    if (strcmp(client_type, "warehouse") == 0) {

        printf("\n--- WAREHOUSE ---\n");
        printf("--- MAIN MENU ---\n");
        printf("1. View inventory\n");
        printf("2. Configure current supply\n");
        printf("3. Order supplies\n");
        printf("4. Send alert\n");
        printf("5. Exit\n");
        printf("Select an option:\n");

    } else if (strcmp(client_type, "hub") == 0) {

        printf("\n--- HUB ---\n");
        printf("--- MAIN MENU ---\n");
        printf("1. View inventory\n");
        printf("2. Order supplies\n");
        printf("3. Exit\n");
        printf("Select an option:\n");

    } else if (strcmp(client_type, "root") == 0) {

        printf("\n--- ROOT ---\n");
        printf("--- MAIN MENU ---\n");
        printf("1. Create warehouse client\n");
        printf("2. Exit\n");
        printf("Select an option:\n");

    } else {
        printf("Unknown customer type\n");
    }
}

// Callback cuando hay datos en stdin
void on_stdin_event(
    uv_poll_t* req, 
    int status, 
    int events
) {
    if (events & UV_READABLE) {
        char input[256];
        if (fgets(input, sizeof(input), stdin)) {
            int option = atoi(input);

            if (strcmp(client_type, "warehouse") == 0) {

                switch (menu_level) {

                    case 0: // Main menu
                        if (option == 1) {
                            show_inventary();
                        } else if (option == 2) {
                            menu_level = 1;
                            config_supply_header();
                        } else if (option == 3) {
                            menu_level = 2;
                            request_supplies_header();
                        } else if (option == 4) {
                            menu_level = 3;
                            send_alert_header();
                        } else if (option == 5) {
                            printf("Leaving...\n");
                            uv_stop(uv_default_loop());
                        } else {
                            printf("Invalid option\n");
                        }
                        break;
    
                    case 1: // Submenu: Configure supply
                        config_supply(option);
                        break;
    
                    case 2: // Submenu: Order supplies
                        request_supplies(input);
                        break;

                    case 3: // Submenu: Send alert
                        send_alert(input);
                        break;

                    default:
                        menu_level = 0;
                        show_main_menu();
                        break;
                }
        
            } else if (strcmp(client_type, "hub") == 0) {
        
                switch (menu_level) {

                    case 0: // Main menu
                        if (option == 1) {
                            show_inventary();
                        } else if (option == 2) {
                            menu_level = 1;
                            request_supplies_header();
                        } else if (option == 3) {
                            printf("Leaving...\n");
                            uv_stop(uv_default_loop());
                        } else {
                            printf("Invalid option\n");
                        }
                        break;
    
                    case 1: // Submenu: Order supplies
                        request_supplies(input);
                        break;
    
                    default:
                        menu_level = 0;
                        show_main_menu();
                        break;
                }

            } else if (strcmp(client_type, "root") == 0) {
        
                switch (menu_level) {

                    case 0: // Main menu
                        if (option == 1) {
                            menu_level = 1;
                            create_warehouse_client_header();
                        } else if (option == 2) {
                            printf("Leaving...\n");
                            uv_stop(uv_default_loop());
                        } else {
                            printf("Invalid option\n");
                        }
                        break;
    
                    case 1: // Submenu: Create warehouse client
                        create_warehouse_client(input);
                    break;
    
                    default:
                        menu_level = 0;
                        show_main_menu();
                        break;
                }
                
            } else {
                printf("Unknown customer type\n");
            }
        }
    }
}
