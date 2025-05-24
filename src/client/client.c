#include <client.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <time.h>
#include <uv.h>
#include "cjson/cJSON.h"
#include <netdb.h>
#include <client_globals.h>
#include <client_utils.h>
#include <supply_order_request.h>
#include <timer_functions.h>
#include <client_menu.h>
#include <alert_request.h>
#include <inventory_request.h>
#include <client_supply.h>
#include <warehouse_register_request.h>
#include <auth_request.h>

// Callback cuando hay datos en el socket
void on_socket_event(
    uv_poll_t* req, 
    int status, 
    int events
) {
    char recv_buf[1024];
    if (events & UV_READABLE) {
        int len = recv(sock_fd, recv_buf, sizeof(recv_buf) - 1, 0);
        if (len > 0) {
            recv_buf[len] = '\0';
            printf("Message received: %s\n", recv_buf);
            const char* keys[] = {"messageType"};
            char* response_message_type = get_value_from_json(recv_buf, keys, 1);

            if(strstr("ALERT", response_message_type)){
                handle_alert_message(recv_buf);
            }else if ("SUPPLY_ORDER_REQUEST", response_message_type) {
                handle_supply_order_request(recv_buf);
            }else if ("SUPPLY_SEND", response_message_type) {
                handle_supply_send(recv_buf);
            }
        }      
    }
}

// Configura sockets en modo no bloqueante.
void set_non_blocking(
    int fd
) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int run_client(
    int cant, 
    int supply, 
    char* user, 
    char* password
) {
    printf("user: %s\n", user);
    printf("pass: %s\n", password);

    supply_defined = supply;
    food_box = cant;
    medicine_box = cant;
    water_box = cant;
    weaponry_box = cant;

    // Socket TCP
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    // Resolver el nombre del contenedor a una dirección IP
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(SERVER_IP, NULL, &hints, &res) != 0) {
        perror("getaddrinfo");
        close(sock_fd);
        return 1;
    }

    struct sockaddr_in* addr = (struct sockaddr_in*)res->ai_addr;
    serv_addr.sin_addr = addr->sin_addr;
    freeaddrinfo(res);


    // Se conecta al servidor
    if (connect(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        close(sock_fd);
        return 1;
    }

    if (!authentication(user, password)) {
        fprintf(stderr, "Authentication failed. Closing client.\n");
        close(sock_fd);
        return 1;
    }

    printf("Despues de while 1 \n"); //TESTS

    set_non_blocking(sock_fd);

    uv_loop_t* loop = uv_default_loop();

    // Watchers de socket y stdin
    uv_poll_init(loop, &socket_watcher, sock_fd);
    uv_poll_start(&socket_watcher, UV_READABLE, on_socket_event);

    uv_poll_init(loop, &stdin_watcher, fileno(stdin));
    uv_poll_start(&stdin_watcher, UV_READABLE, on_stdin_event);

    // Timers
    uv_timer_init(loop, &timer_supply_incr);
    uv_timer_start(&timer_supply_incr, supply_incr, 10000, 10000);

    uv_timer_init(loop, &timer_all_decr);
    uv_timer_start(&timer_all_decr, all_decr, 30000, 30000);

    uv_timer_init(loop, &timer_send_status);
    uv_timer_start(&timer_send_status, send_status, 60000, 60000);

    show_main_menu();
    uv_run(loop, UV_RUN_DEFAULT);

    // IMPORTANTE: liberar recursos cuando se salga del loop
    uv_poll_stop(&socket_watcher);
    uv_poll_stop(&stdin_watcher);
    uv_timer_stop(&timer_supply_incr);
    uv_timer_stop(&timer_all_decr);
    uv_timer_stop(&timer_send_status);

    close(sock_fd);

    return 0;
}
