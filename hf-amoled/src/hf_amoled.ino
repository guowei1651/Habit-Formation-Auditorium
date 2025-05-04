#include "esp32_log.h"
#include "board/esp32_board.h"
#include "business/business_main.h"

const char *logTag = "main";

void setup() {
    Serial.begin(115200);
    delay(20000); // Wait for 20 seconds to allow the serial monitor to open

    esp32_log_level_set("*", ESP_LOG_INFO);
    ESP32_LOGI(logTag, "Starting setup...");

    // Initialize the ESP32 board, 启动 I2C 总线
    ESP32_LOGI(logTag, "Initializing ESP32 Board...");
    board.dataBus = new DataBus();
    board.dataBus->begin();

    // Get the unique ID of the ESP32
    ESP32_LOGI(logTag, "Getting unique ID...");
    board.uniqueID = getUniqueID();

    // Initialize the power management unit
    ESP32_LOGI(logTag, "Initializing power management unit...");
    board.powerManager = new PowerManager();
    board.powerManager->begin();

    // Initialize the display
    ESP32_LOGI(logTag, "Initializing display...");
    board.display = new Display(board.dataBus);
    board.display->begin();

    // Initialize the button control
    ESP32_LOGI(logTag, "Initializing button control...");
    board.button = new ButtonControl();
    board.button->begin();

    // Initialize the business logic
    ESP32_LOGI(logTag, "Initializing business logic...");
    business = new Business(&board);
    business->begin();

    // Initialize the clock
    ESP32_LOGI(logTag, "Initializing clock...");
    business->setClock(new Clock(board.display));
    business->getClock()->begin();
}

void loop() {
    ESP32_LOGI(logTag, "Running main loop...");
    business->loop();
    delay(5000);
}