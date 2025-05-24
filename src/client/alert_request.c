#include <alert_request.h>
#include <client_globals.h>
#include <client_menu.h>
#include <client_utils.h>
#include <string.h>
#include <stdlib.h>
// Comentario

void send_alert_header(){
    printf("\n--- SEND ALERT ---\n");
    printf("1. Weather\n2. Infection\n3. Enemy Threat\n");
    printf("Specify: type|message\n");
}

void send_alert(
    char* input
){
    input[strcspn(input, "\n")] = 0; // Eliminar salto de línea
    if (strlen(input) > 0) {
        char msg[512];
      
        int option = atoi(strtok(input, "|"));
        char* alert_message = strtok(NULL, "|");      

        char* alert_type = NULL;

        switch (option) {
            case 1:
                alert_type = "Weather";
                break;
            case 2:
                alert_type = "Infection";
                break;
            case 3:
                alert_type = "Enemy Threat";
                break;
            default:
                printf("Invalid option. Please select 1, 2, or 3.\n");
                break;
        }

        snprintf(msg, sizeof(msg),
            "{\"messageType\":\"ALERT_MESSAGE\",\"body\":{\"type\":\"%s\",\"message\":\"%s\"}}",
            alert_type, alert_message);


        send(sock_fd, msg, strlen(msg), 0);
        printf("Alert sent\n");

        
        show_main_menu();
    } else {
        printf("Empty message. Nothing sent.\n");
        show_main_menu();
    }
    menu_level = 0;
}

void handle_alert_message(
    char* alert_message
){
    const char* keys_type[] = {"body", "type"};
    // char* type = get_value_from_json(alert_message, keys_type, 2);
    char* type = get_value_from_json(alert_message, (const char*[]){"body", "type"}, 2);
    if (type == NULL) {
        printf("Error: Unable to parse alert type.\n");
        free(type);
        return;
    }    
    const char* keys_msg[] = {"body", "message"};
    char* msg = get_value_from_json(alert_message, (const char*[]){"body", "message"}, 2);
    if (msg == NULL) {
        printf("Error: Unable to parse alert message.\n");
        free(type);
        free(msg);
        return;
    }

    printf("Broadcast alert received - Type: %s - Message: %s\n", type, msg);
    free(type);
    free(msg);
    return;
}
