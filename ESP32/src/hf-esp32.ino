#include <Arduino.h>
#include <esp_sleep.h>
#include <WiFi.h>
#include <WiFiAP.h>

#include "../include/esp32_board.h"
#include "../include/iot.h"
#include "../include/business_task_scheduler.h"

ESP32Board esp32;

struct Business
{
  IoTClient iotClient;
  TaskScheduler scheduler(esp32.display);
};
Business business;

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting setup...");

  // Get the unique ID of the ESP32
  esp32.uniqueID = getUniqueID();

  // Initialize IoT client with the unique ID
  Serial.println("Initializing battery...");
  esp32.battery = new Battery();
  esp32.battery->begin();

  // Initialize button interrupts
  Serial.println("Initializing button interrupts...");
  buttonBegin();

  // Initialize configuration manager
  Serial.println("Initializing NVS configuration manager...");
  esp32.configManager = new ConfigManager();
  esp32.configManager->begin();

  // Initialize sensor
  Serial.println("Initializing SHT30 sensor...");
  esp32.sht30 = new SHT30();
  esp32.sht30->begin();

  // Initialize display
  Serial.println("Initializing display...");
  esp32.display = new Display();
  esp32.display->begin();

  // Initialize network
  Serial.println("Initializing network...");
  esp32.network = new Network(esp32.configManager, esp32.display);
  esp32.network->begin();

  // Attempt to connect to stored WiFi configuration
  Serial.println("Attempting to connect to stored WiFi configuration...");

  // Start NTP service if connected to WiFi
  esp32.ntpClient.begin();
  if (esp32.network.isConnected())
  {
    Serial.println("Connected to WiFi. Starting NTP client...");
    esp32.ntpClient.syncTime();
  }
  else
  {
    Serial.println("WiFi not connected. Skipping NTP client initialization.");
  }

  // Initialize OTA manager
  Serial.println("Initializing OTA manager...");
  esp32.otaManager.begin(FIRMWARE_VERSION);

  // Initialize IoT client with the unique ID
  Serial.println("Initializing IoT client...");

  iotClient.begin(esp32.uniqueID);

  Serial.println("Setup complete.");
}

void loop()
{
  float temp, hum;
  if (esp32.sht30.readTempHum(temp, hum))
  {
    // Use temperature and humidity data
    Serial.printf("Temperature: %.2f°C, Humidity: %.2f%%\n", temp, hum);
  }

  // Display picture 1
  EPD_init();
  PIC_display1();
  EPD_refresh();
  EPD_sleep();
  delay(3000);

  esp32.ntpClient.updateIfNeeded();
  esp_sleep_enable_timer_wakeup(60 * 1000000);
  esp_deep_sleep_start();

  scheduler.begin();
  scheduler.addTask("30 8 * * *", "http://example.com/morning.bmp");
  scheduler.addTask("0 12 * * *", "http://example.com/noon.bmp");

  if (esp32.network.isConnected())
  {
    esp32.otaManager.checkUpdate(UPDATE_URL);
  }

  scheduler.checkTasks();

  if (esp32.battery->isLow())
  {
    Serial.println("Low battery!");
  }
  Serial.printf("Battery: %.2fV (%d%%)\n",
                esp32.battery->getVoltage(),
                esp32.battery->getPercentage());
}
