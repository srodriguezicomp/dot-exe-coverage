#include <unity.h>
#include "supply_order_request.h"
#include "timer_functions.h"
#include "client_menu.h"
#include "client_globals.h"
#include "inventory_request.h"
#include "alert_request.h"
#include "client_supply.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Mock global variables
int fb;
int mb;
int wab;
int web;

/////////////////////////////////////////////////////////////
// stdout redirect
/////////////////////////////////////////////////////////////

static char buffer[1024];
static FILE* original_stdout;
static FILE* test_stdout;

void redirect_stdout(void) {
    original_stdout = stdout;
    test_stdout = fmemopen(buffer, sizeof(buffer), "w");
    stdout = test_stdout;
}

void restore_stdout(void) {
    fflush(test_stdout);
    fclose(test_stdout);
    stdout = original_stdout;
}

/////////////////////////////////////////////////////////////
// stdout redirect
/////////////////////////////////////////////////////////////

void setUp(void)
{
    food_box = 7;
    medicine_box = 6;
    water_box = 5;
    weaponry_box = 4;

    fb = food_box;
    mb = medicine_box;
    wab = water_box;
    web = weaponry_box;

    memset(buffer, 0, sizeof(buffer));
}

void tearDown(void)
{

}

void test_all_decr_decrements_correctly(void) {
    all_decr(NULL);

    TEST_ASSERT_EQUAL_INT(fb - 1, food_box);
    TEST_ASSERT_EQUAL_INT(mb - 1, medicine_box);
    TEST_ASSERT_EQUAL_INT(wab - 1, water_box);
    TEST_ASSERT_EQUAL_INT(web - 1, weaponry_box);
}

void test_all_decr_no_negative_values(void) {
    for (int i = 0; i < 10; i++) {
        all_decr(NULL);
    }

    TEST_ASSERT_EQUAL_INT(0, food_box);
    TEST_ASSERT_EQUAL_INT(0, medicine_box);
    TEST_ASSERT_EQUAL_INT(0, water_box);
    TEST_ASSERT_EQUAL_INT(0, weaponry_box);
}

void test_supply_incr(void) {

    supply_defined = FOOD_BOX;
    supply_incr(NULL);
    TEST_ASSERT_EQUAL_INT(fb + 1, food_box);

    supply_defined = MEDICINE_BOX;
    supply_incr(NULL);
    TEST_ASSERT_EQUAL_INT(mb + 1, medicine_box);
    
    supply_defined = WATER_BOX;
    supply_incr(NULL);
    TEST_ASSERT_EQUAL_INT(wab + 1, water_box);

    supply_defined = WEAPONRY_BOX;
    supply_incr(NULL);
    TEST_ASSERT_EQUAL_INT(web + 1, weaponry_box);
}

void test_get_supply_value_by_index(void)
{
    TEST_ASSERT_EQUAL_INT(7, get_supply_value_by_index(FOOD_BOX));
    TEST_ASSERT_EQUAL_INT(6, get_supply_value_by_index(MEDICINE_BOX));
    TEST_ASSERT_EQUAL_INT(5, get_supply_value_by_index(WATER_BOX));
    TEST_ASSERT_EQUAL_INT(4, get_supply_value_by_index(WEAPONRY_BOX));
    TEST_ASSERT_EQUAL_INT(-1, get_supply_value_by_index(5));
}

void test_set_supply_value_by_index(void)
{
    TEST_ASSERT_EQUAL_INT(0, set_supply_value_by_index(FOOD_BOX, 10));
    TEST_ASSERT_EQUAL_INT(10, food_box);
    TEST_ASSERT_EQUAL_INT(0, set_supply_value_by_index(MEDICINE_BOX, 20));
    TEST_ASSERT_EQUAL_INT(20, medicine_box);
    TEST_ASSERT_EQUAL_INT(0, set_supply_value_by_index(WATER_BOX, 30));
    TEST_ASSERT_EQUAL_INT(30, water_box);
    TEST_ASSERT_EQUAL_INT(0, set_supply_value_by_index(WEAPONRY_BOX, 40));
    TEST_ASSERT_EQUAL_INT(40, weaponry_box);
    TEST_ASSERT_EQUAL_INT(-1, set_supply_value_by_index(5, 50));
}

void test_add_supply_value_by_index(void)
{
    add_supply_value_by_index(FOOD_BOX, 3);
    TEST_ASSERT_EQUAL_INT(fb + 3, food_box);
    add_supply_value_by_index(MEDICINE_BOX, 4);
    TEST_ASSERT_EQUAL_INT(mb + 4, medicine_box);
    add_supply_value_by_index(WATER_BOX, 5);
    TEST_ASSERT_EQUAL_INT(wab + 5, water_box);
    add_supply_value_by_index(WEAPONRY_BOX, 6);
    TEST_ASSERT_EQUAL_INT(web + 6, weaponry_box);
}

void test_subtract_supply_value_by_index_less_than_stock(void)
{
    // Test subtracting < stock
    TEST_ASSERT_EQUAL_INT(5, subtract_supply_value_by_index(FOOD_BOX, 5));
    TEST_ASSERT_EQUAL_INT(4, subtract_supply_value_by_index(MEDICINE_BOX, 4));
    TEST_ASSERT_EQUAL_INT(3, subtract_supply_value_by_index(WATER_BOX, 3));
    TEST_ASSERT_EQUAL_INT(2, subtract_supply_value_by_index(WEAPONRY_BOX, 2));
    TEST_ASSERT_EQUAL_INT(0, subtract_supply_value_by_index(5, 10));
}

void test_subtract_supply_value_by_index_greater_than_stock(void)
{
    // Test subtracting > stock
    TEST_ASSERT_EQUAL_INT(fb, subtract_supply_value_by_index(FOOD_BOX, 10));
    TEST_ASSERT_EQUAL_INT(mb, subtract_supply_value_by_index(MEDICINE_BOX, 10));
    TEST_ASSERT_EQUAL_INT(wab, subtract_supply_value_by_index(WATER_BOX, 10));
    TEST_ASSERT_EQUAL_INT(web, subtract_supply_value_by_index(WEAPONRY_BOX, 10));
    TEST_ASSERT_EQUAL_INT(0, subtract_supply_value_by_index(5, 10));
}

void test_craft_supply_message(void)
{
    char* msg = craft_supply_message("test_message", 10, 20, 30, 40, 1234);
    TEST_ASSERT_NOT_NULL(msg);
    TEST_ASSERT_EQUAL_STRING("{\"messageType\":\"test_message\",\"body\":{\"inventory\":{\"waterBox\":10,\"weaponryBox\":20,\"foodBox\":30,\"medicineBox\":40},\"shipmentId\":1234}}", msg);
}

void test_handle_supply_send(void) {
    char *recv_buf = craft_supply_message("supply_send", 10, 20, 30, 40, 1234);
    handle_supply_send(recv_buf);

    TEST_ASSERT_EQUAL_INT(15, water_box);
    TEST_ASSERT_EQUAL_INT(24, weaponry_box);
    TEST_ASSERT_EQUAL_INT(37, food_box);
    TEST_ASSERT_EQUAL_INT(46, medicine_box);
}

void test_show_main_menu_hub(void) {
    strcpy(client_type, HUB_CLIENT);
    redirect_stdout();
    show_main_menu();
    restore_stdout();

    const char* expected_output =
        "\n--- HUB ---\n"
        "--- MAIN MENU ---\n"
        "1. View inventory\n"
        "2. Order supplies\n"
        "3. Exit\n"
        "Select an option:\n";

    TEST_ASSERT_EQUAL_STRING(expected_output, buffer);
}

void test_show_main_menu_warehouse(void) {
    strcpy(client_type, WAREHOUSE_CLIENT);
    redirect_stdout();
    show_main_menu();
    restore_stdout();

    const char* expected_output =
        "\n--- WAREHOUSE ---\n"
        "--- MAIN MENU ---\n"
        "1. View inventory\n"
        "2. Configure current supply\n"
        "3. Order supplies\n"
        "4. Send alert\n"
        "5. Exit\n"
        "Select an option:\n";

    TEST_ASSERT_EQUAL_STRING(expected_output, buffer);
}

void test_show_main_menu_root(void) {
    strcpy(client_type, ROOT_CLIENT);
    redirect_stdout();
    show_main_menu();
    restore_stdout();

    const char* expected_output =
        "\n--- ROOT ---\n"
        "--- MAIN MENU ---\n"
        "1. Create warehouse client\n"
        "2. Exit\n"
        "Select an option:\n";

    TEST_ASSERT_EQUAL_STRING(expected_output, buffer);
}

void test_show_inventary(void) {

    strcpy(client_type, ROOT_CLIENT);
    // Redirect stdout to capture the output
    redirect_stdout();
    show_inventary();
    restore_stdout();

    // Expected output
    const char* expected_output =
        "Inventario actual:\n"
        "foodBox: 7 | medicineBox: 6 | waterBox: 5 | weaponryBox: 4\n"
        "\n--- ROOT ---\n"
        "--- MAIN MENU ---\n"
        "1. Create warehouse client\n"
        "2. Exit\n"
        "Select an option:\n";

    // Assert the captured output matches the expected output
    TEST_ASSERT_EQUAL_STRING(expected_output, buffer);
}

void test_send_alert_weather(void) {
    strcpy(client_type, ROOT_CLIENT);
    char input_weather[] = "1|Severe storm warning";

    redirect_stdout();
    send_alert(input_weather);
    restore_stdout();

    // Expected output
    const char* expected_output_weather =
        "Alert sent\n"
        "\n--- ROOT ---\n"
        "--- MAIN MENU ---\n"
        "1. Create warehouse client\n"
        "2. Exit\n"
        "Select an option:\n";

    TEST_ASSERT_EQUAL_STRING(expected_output_weather, buffer);
}

void test_send_alert_infection(void) {
    strcpy(client_type, ROOT_CLIENT);
    char input_infection[] = "2|High infection rate warning";

    redirect_stdout();
    send_alert(input_infection);
    restore_stdout();

    // Expected output
    const char* expected_output_infection =
        "Alert sent\n"
        "\n--- ROOT ---\n"
        "--- MAIN MENU ---\n"
        "1. Create warehouse client\n"
        "2. Exit\n"
        "Select an option:\n";

    TEST_ASSERT_EQUAL_STRING(expected_output_infection, buffer);
}

void test_send_alert_enemy_threat(void) {
    strcpy(client_type, ROOT_CLIENT);
    char input_enemy_threat[] = "3|Enemy attack warning";

    redirect_stdout();
    send_alert(input_enemy_threat);
    restore_stdout();

    // Expected output
    const char* expected_output_enemy_threat =
        "Alert sent\n"
        "\n--- ROOT ---\n"
        "--- MAIN MENU ---\n"
        "1. Create warehouse client\n"
        "2. Exit\n"
        "Select an option:\n";

    TEST_ASSERT_EQUAL_STRING(expected_output_enemy_threat, buffer);
}

void test_send_alert_invalid(void) {
    strcpy(client_type, ROOT_CLIENT);
    char input_invalid[] = "4|Unknown alert";
    redirect_stdout();
    send_alert(input_invalid);
    restore_stdout();

    // Expected output
    const char* expected_output_invalid =
        "Invalid option. Please select 1, 2, or 3.\n"
        "Alert sent\n"
        "\n--- ROOT ---\n"
        "--- MAIN MENU ---\n"
        "1. Create warehouse client\n"
        "2. Exit\n"
        "Select an option:\n";

    TEST_ASSERT_EQUAL_STRING(expected_output_invalid, buffer);
}

void test_send_alert_empty(void) {
    char input_empty[] = "\n";
    redirect_stdout();
    send_alert(input_empty);
    restore_stdout();

    // Expected output
    const char* expected_output_empty =
        "Empty message. Nothing sent.\n"
        "\n--- ROOT ---\n"
        "--- MAIN MENU ---\n"
        "1. Create warehouse client\n"
        "2. Exit\n"
        "Select an option:\n";

    TEST_ASSERT_EQUAL_STRING(expected_output_empty, buffer);
}

void test_handle_alert_message(void) {
    char alert_message[] = "{\"messageType\":\"ALERT_MESSAGE\",\"body\":{\"type\":\"Weather\",\"message\":\"Severe storm warning\"}}";

    redirect_stdout();
    handle_alert_message(alert_message);
    restore_stdout();

    // Expected output
    const char* expected_output =
        "Broadcast alert received - Type: Weather - Message: Severe storm warning\n";

    // Assert the captured output matches the expected output
    TEST_ASSERT_EQUAL_STRING(expected_output, buffer);
}

void test_get_value_from_json_string(void) {
    const char* json_str = "{\"body\":{\"type\":\"Weather\",\"message\":\"Severe storm warning\"}}";
    const char* keys[] = {"body", "type"};
    char* value = get_value_from_json(json_str, keys, 2);
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING("Weather", value);
    free(value);
}

void test_get_value_from_json_number(void) {
    const char* json_str = "{\"body\":{\"type\":\"Weather\",\"message\":123}}";
    const char* keys[] = {"body", "message"};
    char* value = get_value_from_json(json_str, keys, 2);
    TEST_ASSERT_NOT_NULL(value);
    TEST_ASSERT_EQUAL_STRING("123", value);
    free(value);
}

void test_get_value_from_json_invalid(void) {
    const char* json_str = "{\"body\":{\"type\":\"Weather\",\"message\":true}}";
    const char* keys[] = {"body", "message"};
    char* value = get_value_from_json(json_str, keys, 2);
    TEST_ASSERT_NULL(value);
}

void test_get_value_from_json_not_found(void) {
    const char* json_str = "{\"body\":{\"type\":\"Weather\",\"message\":\"Severe storm warning\"}}";
    const char* keys[] = {"invalid_key", "message"};
    char* value = get_value_from_json(json_str, keys, 2);
    TEST_ASSERT_NULL(value);
}

void test_config_supply(void) {
    strcpy(client_type, HUB_CLIENT);
    
    redirect_stdout();
    config_supply(1);
    restore_stdout();

    const char* expected_output =
        "Current supply defined as: foodBox\n"
        "\n--- HUB ---\n"
        "--- MAIN MENU ---\n"
        "1. View inventory\n"
        "2. Order supplies\n"
        "3. Exit\n"
        "Select an option:\n";

    TEST_ASSERT_EQUAL_STRING(expected_output, buffer);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_all_decr_decrements_correctly);
    RUN_TEST(test_supply_incr);
    RUN_TEST(test_all_decr_no_negative_values);

    RUN_TEST(test_get_supply_value_by_index);
    RUN_TEST(test_set_supply_value_by_index);
    RUN_TEST(test_add_supply_value_by_index);
    RUN_TEST(test_subtract_supply_value_by_index_less_than_stock);
    RUN_TEST(test_subtract_supply_value_by_index_greater_than_stock);
    RUN_TEST(test_craft_supply_message);
    RUN_TEST(test_handle_supply_send);

    RUN_TEST(test_show_main_menu_hub);
    RUN_TEST(test_show_main_menu_warehouse);
    RUN_TEST(test_show_main_menu_root);

    RUN_TEST(test_show_inventary);

    RUN_TEST(test_send_alert_weather);
    RUN_TEST(test_send_alert_infection);
    RUN_TEST(test_send_alert_enemy_threat);
    RUN_TEST(test_send_alert_invalid);
    RUN_TEST(test_send_alert_empty);

    RUN_TEST(test_handle_alert_message);

    RUN_TEST(test_get_value_from_json_string);
    RUN_TEST(test_get_value_from_json_number);
    RUN_TEST(test_get_value_from_json_invalid);
    RUN_TEST(test_get_value_from_json_not_found);

    RUN_TEST(test_config_supply);

    return UNITY_END();
}