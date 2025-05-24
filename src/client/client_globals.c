#include <client_globals.h>
#include <uv.h>

// Define constants
const char SERVER_IP[] = "server-container";
const int SERVER_PORT = 12346;
const int FOOD_BOX = 1;
const int MEDICINE_BOX = 2;
const int WATER_BOX = 3;
const int WEAPONRY_BOX = 4;
const char ROOT_CLIENT[] = "root";
const char WAREHOUSE_CLIENT[] = "warehouse";
const char HUB_CLIENT[] = "hub";

// Define global variables
uv_poll_t socket_watcher;
uv_poll_t stdin_watcher;
uv_timer_t timer_supply_incr;
uv_timer_t timer_all_decr;
uv_timer_t timer_send_status;
int menu_level = 0;
int sock_fd;
int supply_defined;
int food_box , medicine_box , water_box , weaponry_box ; // Setea los valores de suministros
char client_type[20];

