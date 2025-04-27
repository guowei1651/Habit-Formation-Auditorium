#include "esp32_log.h"
#include "board/esp32_board.h"
#include "business/business_main.h"

const char *logTag = "main";

void setup() {
    Serial.begin(115200);

    esp32_log_level_set("*", ESP_LOG_INFO);
    ESP32_LOGI(logTag, "Starting setup...");
    
    // Get the unique ID of the ESP32
    board.uniqueID = getUniqueID();

    // Initialize the power management unit
    ESP32_LOGI(logTag, "Initializing power management unit...");
    board.powerManager = new PowerManager();
    if (!board.powerManager->begin(Wire, SDA, SCL)) {
        ESP32_LOGE(logTag, "Failed to initialize power management unit");
        return;
    }

    // Initialize IoT client with the unique ID
    ESP32_LOGI(logTag, "Initializing data bus...");
    board.dataBus = new DataBus();
    board.dataBus->begin();

    // Initialize the display
    ESP32_LOGI(logTag, "Initializing display...");
    board.display = new Display(board.dataBus);
    board.display->begin();

    // Initialize the button control
    ESP32_LOGI(logTag, "Initializing button control...");
    board.button = new ButtonControl();
    board.button->begin();

    // Initialize the clock
    ESP32_LOGI(logTag, "Initializing clock...");
    business->setClock(new Clock(board.display));
    business->getClock()->begin();
}

void loop() {
    business->loop();
    delay(5);
}