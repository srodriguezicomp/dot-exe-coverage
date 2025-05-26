#pragma once
#include <uv.h>

// Declare constants
extern const char SERVER_IP[];
extern const int SERVER_PORT;
extern const int FOOD_BOX;
extern const int MEDICINE_BOX;
extern const int WATER_BOX;
extern const int WEAPONRY_BOX;
extern const char ROOT_CLIENT[];
extern const char WAREHOUSE_CLIENT[];
extern const char HUB_CLIENT[];

// Declare global variables
extern uv_poll_t socket_watcher;
extern uv_poll_t stdin_watcher;
extern uv_timer_t timer_supply_incr;
extern uv_timer_t timer_all_decr;
extern uv_timer_t timer_send_status;
extern int menu_level;
extern int sock_fd;
extern int supply_defined;
extern int food_box , medicine_box , water_box , weaponry_box ; // Setea los valores de suministros
extern char client_type[20];

