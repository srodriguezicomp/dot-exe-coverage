#include <timer_functions.h>
#include <uv.h>
#include <string.h>

// Callback cada 10 segundos
void supply_incr(
    uv_timer_t* handle
) {
    switch (supply_defined) {
        case 1:
            food_box++;
            // printf("Incrementando food_box: %d\n", food_box); //TESTS
            break;
        case 2:
            medicine_box++;
            // printf("Incrementando medicine_box: %d\n", medicine_box); //TESTS
            break;
        case 3:
            water_box++;
            // printf("Incrementando water_box: %d\n", water_box); //TESTS
            break;
        case 4:
            weaponry_box++;
            // printf("Incrementando weaponry_box: %d\n", weaponry_box); //TESTS
            break;
        default:
            // printf("Valor de supply_defined no válido: %d\n", supply_defined); //TESTS
            break;
    }
}

// Callback cada 30 segundos
void all_decr(
    uv_timer_t* handle
) {
    if (food_box > 0) food_box--;
    if (medicine_box > 0) medicine_box--;
    if (water_box > 0) water_box--;
    if (weaponry_box > 0) weaponry_box--;
    // printf("Cada 30s: disminuir todos | food_box: %d | medicine_box: %d | water_box: %d | weaponry_box: %d\n",food_box, medicine_box, water_box, weaponry_box); //TESTS
}

// Callback cada 60 segundos
void send_status(
    uv_timer_t* handle
) {
    char status_msg[512];
    snprintf(status_msg, sizeof(status_msg),
        "{\"messageType\":\"INVENTORY_UPDATE\",\"body\":{\"inventory\":{\"foodBox\":%d,\"medicineBox\":%d,\"waterBox\":%d,\"weaponryBox\":%d}}}",
        food_box, medicine_box, water_box, weaponry_box);
    send(sock_fd, status_msg, strlen(status_msg), 0);
    // printf("Cada 60s: enviar estado | food_box: %d | medicine_box: %d | water_box: %d | weaponry_box: %d\n",food_box, medicine_box, water_box, weaponry_box); //TESTS
}

