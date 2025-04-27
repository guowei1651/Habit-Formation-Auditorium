#include <ArduinoJson.h>
#include "../include/esp32_log.h"
#include "../include/esp32_network.h"

const char *networkTag = "network";

Network::Network(ConfigManager &configManager, Display &display)
    : _configManager(configManager), _display(display), _server(80) {}

void Network::begin()
{
    esp32_log_level_set(networkTag, ESP_LOG_DEBUG);
    ESP32_LOGD(networkTag, "network 初始化开始");
    if (!connectStoredWiFi())
    {
        ESP32_LOGD(networkTag, "network 未获取到Wifi链接信息");
        startConfigPortal();
    }
    ESP32_LOGD(networkTag, "network 初始化结束");
}

bool Network::confirmIsConnected() {
    if (!isConnected()) {
        ESP32_LOGI(networkTag, "网络未连接，进行重连");
        connectStoredWiFi();
    }
    ESP32_LOGI(networkTag, "重连中的IP地址为: %s", WiFi.localIP().toString());
    return isConnected();
}

bool Network::connectStoredWiFi()
{
    String ssid;
    String password;
    String wifiConfig = _configManager.getValue("wifi");

    ESP32_LOGD(networkTag, "获取配置中的wifi: %s", wifiConfig.c_str());

    if (wifiConfig.length() > 0)
    {
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, wifiConfig);

        if (!error)
        {
            ssid = doc["ssid"].as<String>();
            password = doc["password"].as<String>();
        }
    }

    ESP32_LOGD(networkTag, "获取配置中的wifi解析出的内容 ssid: %s password: %s", ssid, password);

    if (ssid.length() <= 0)
    {
        return false;
    }

    ESP32_LOGD(networkTag, "开始链接wifi");
    if (password.length() > 0)
    {
        WiFi.begin(ssid.c_str(), password.c_str());
    }
    else
    {
        WiFi.begin(ssid.c_str());
    }

    ESP32_LOGD(networkTag, "等待链接wifi结束");
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < WIFI_TIMEOUT)
    {
        delay(500);
    }
    ESP32_LOGD(networkTag, "等待链接wifi结束，链接状态为:%d", WiFi.status());
    return isConnected();
}

void Network::startConfigPortal()
{
    WiFi.softAP(AP_SSID);

    _server.on("/", [this]()
               { handleRoot(); });
    _server.on("/save", [this]()
               { handleSave(); });
    _server.on("/scan", [this]()
               { handleScan(); });
    _server.begin();

    _display.prompt("请为设备链接网络。链接ssid：" + String(AP_SSID) + " 并访问：192.168.4.1");

    while (true)
    {
        _server.handleClient();
        if (isConnected())
        {
            break;
        }
    }
}

void Network::handleRoot()
{
    sendHtmlPage();
}

void Network::handleSave()
{
    String ssid = _server.arg("ssid");
    String password = _server.arg("password");

    ESP32_LOGD(networkTag, "配置好的wifi ssid:%s password:%s", ssid.c_str(), password.c_str());

    if (ssid.length() > 0)
    {
        DynamicJsonDocument doc(128);
        doc["ssid"] = ssid;
        doc["password"] = password;
        String json;
        serializeJson(doc, json);

        ESP32_LOGD(networkTag, "配置好的wifi 存储的json:%s", json.c_str());
        _configManager.setValue("wifi", json);
        _configManager.commit();
        _server.send(200, "text/html", "Saved. ESP will now restart...");
        delay(2000);
        ESP.restart();
    }
}

void Network::handleScan()
{
    int n = WiFi.scanNetworks();
    String json = "[";
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
            json += ",";
        json += "{";
        json += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
        json += "\"rssi\":" + String(WiFi.RSSI(i));
        json += "}";
    }
    json += "]";
    _server.send(200, "application/json", json);
}

void Network::sendHtmlPage()
{
    String html = R"(
        <!DOCTYPE html><html>
        <head>
            <meta name="viewport" content="width=device-width, initial-scale=1">
            <style>
                body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background-color: #f4f4f9; max-width: 600px; margin: auto; }
                h1 { color: #333; }
                form { background: #fff; padding: 20px; border-radius: 5px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
                input, button { margin: 5px 0; padding: 10px; width: 100%; box-sizing: border-box; }
                button { background-color: #007BFF; color: white; border: none; cursor: pointer; }
                button:hover { background-color: #0056b3; }
                #wifi-list { margin-top: 20px; }
                .wifi-item { padding: 10px; background: #fff; margin-bottom: 5px; border-radius: 5px; cursor: pointer; }
                .wifi-item:hover { background: #e9ecef; }
            </style>
        </head>
        <body>
            <h1>ESP32 WiFi Config</h1>
            <form action="/save" method="POST">
                <label for="ssid">SSID:</label><br>
                <input type="text" name="ssid" id="ssid"><br>
                <label for="password">Password:</label><br>
                <input type="password" name="password"><br>
                <button type="submit">Save</button>
            </form>
            <button id="scan-btn">Scan WiFi</button>
            <div id="wifi-list"></div>
            <script>
                document.getElementById('scan-btn').addEventListener('click', function() {
                    fetch('/scan')
                        .then(response => response.json())
                        .then(data => {
                            let list = document.getElementById('wifi-list');
                            list.innerHTML = '';
                            data.forEach(network => {
                                let div = document.createElement('div');
                                div.className = 'wifi-item';
                                div.innerHTML = 'SSID: ' + network.ssid + ', RSSI: ' + network.rssi;
                                div.onclick = function() {
                                    document.getElementById('ssid').value = network.ssid;
                                };
                                list.appendChild(div);
                            });
                        });
                });
            </script>
        </body>
        </html>
    )";
    _server.send(200, "text/html", html);
}

bool Network::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}