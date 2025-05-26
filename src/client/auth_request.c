#include <auth_request.h>
#include <client_globals.h>
#include <client_utils.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int send_auth(
    const char* user, 
    const char* password
) {
    char msg[512];
    snprintf(msg, sizeof(msg),
        "{\"messageType\":\"AUTENTICATION_REQUEST\",\"body\":{\"user\":\"%s\",\"password\":\"%s\"}}",
        user, password);
    
    int bytes_sent = send(sock_fd, msg, strlen(msg), 0);
    if (bytes_sent < 0) {
        perror("Error sending authentication");
        return -1;
    }
    return bytes_sent;
}

int authentication(
    char* user, 
    char* password
) {
    const int tam = 256;
    char buffer_auth[tam];
    int auth_success = 0;

    if (send_auth(user, password) < 0) {
        fprintf(stderr, "Initial failure in sending credentials.\n");
        return 0;
    }
   
    while (!auth_success) {

        memset(buffer_auth, '\0', tam);
        int len_msg_auth = read(sock_fd, buffer_auth, tam);

        if (len_msg_auth <= 0) {
            perror("Error reading from socket");
            return 0;
        }

        printf("Response: %s\n", buffer_auth);

        const char* keys[] = {"messageType"};
        char* response_message_type = get_value_from_json(buffer_auth, keys, 1);

        if (response_message_type == NULL) {
            fprintf(stderr, "Error getting 'messageType' from JSON.\n");
            return 0;
        }

        if (strcmp(response_message_type, "AUTENTICATION_CONFIRM") == 0) {
            printf("Authentication success\n");
            const char* keys[] = {"body", "clientType"};
            char* value = get_value_from_json(buffer_auth, keys, 2);
            strncpy(client_type, value, sizeof(client_type) - 1);
            printf("The client_type is: %s\n", client_type);
            auth_success = 1;
        } else {
            printf("Authentication failed\n");

            char new_password[50];

            printf("Insert password: ");
            scanf("%49s", new_password);

            if (send_auth(user, new_password) < 0) {
                fprintf(stderr, "Failed to resend credentials.\n");
                free(response_message_type);
                return 0;
            }
        }

        free(response_message_type);
    }

    return auth_success;
}