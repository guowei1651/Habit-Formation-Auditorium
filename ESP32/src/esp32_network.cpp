#include "../include/esp32_network.h"

WiFiManager::WiFiManager(ConfigManager& configManager, Display& display) 
    : _configManager(configManager), _display(display), _server(80) {}

void WiFiManager::begin() {
    if (!connectStoredWiFi()) {
        startConfigPortal();
    }
}

bool WiFiManager::connectStoredWiFi() {
    String ssid = _configManager.getWiFiSSID();
    String password = _configManager.getWiFiPassword();
    
    if (ssid.length() > 0) {
        if (password.length() > 0) {
            WiFi.begin(ssid.c_str(), password.c_str());
        } else {
            WiFi.begin(ssid.c_str());
        }
        
        unsigned long startTime = millis();
        
        while (WiFi.status() != WL_CONNECTED && millis() - startTime < WIFI_TIMEOUT) {
            delay(500);
        }
        
        return WiFi.status() == WL_CONNECTED;
    }
    return false;
}

void WiFiManager::startConfigPortal() {
    WiFi.softAP(AP_SSID);
    
    _server.on("/", [this]() { handleRoot(); });
    _server.on("/save", [this]() { handleSave(); });
    _server.on("/scan", [this]() { handleScan(); });
    _server.begin();
    
    _display.darw_text("请链接Wifi:"+AP_SSID+"进行联网配置");

    Serial.println("AP Mode Started");
    Serial.println("Please connect to " + String(AP_SSID));
    Serial.println("And visit 192.168.4.1");
    
    while(true) {
        _server.handleClient();
        if (WiFi.status() == WL_CONNECTED) {
            break;
        }
    }
}

void WiFiManager::handleRoot() {
    sendHtmlPage();
}

void WiFiManager::handleSave() {
    String ssid = _server.arg("ssid");
    String password = _server.arg("password");
    
    if (ssid.length() > 0 && password.length() > 0) {
        _configManager.saveWiFiConfig(ssid.c_str(), password.c_str());
        _server.send(200, "text/html", "Saved. ESP will now restart...");
        delay(2000);
        ESP.restart();
    }
}

void WiFiManager::handleScan() {
    int n = WiFi.scanNetworks();
    String json = "[";
    for (int i = 0; i < n; ++i) {
        if (i > 0) json += ",";
        json += "{";
        json += "\"ssid\":\"" + WiFi.SSID(i) + "\",";
        json += "\"rssi\":" + String(WiFi.RSSI(i));
        json += "}";
    }
    json += "]";
    _server.send(200, "application/json", json);
}

void WiFiManager::sendHtmlPage() {
    String html = R"(
        <!DOCTYPE html><html>
        <head>
            <meta name="viewport" content="width=device-width, initial-scale=1">
            <style>
                body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background-color: #f4f4f9; }
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
            <button onclick="scan()">Scan WiFi</button>
            <div id="wifi-list"></div>
            <script>
                function scan() {
                    fetch('/scan')
                        .then(response => response.json())
                        .then(data => {
                            let list = document.getElementById('wifi-list');
                            list.innerHTML = '';
                            data.forEach(network => {
                                let div = document.createElement('div');
                                div.className = 'wifi-item';
                                div.innerHTML = `SSID: ${network.ssid}, RSSI: ${network.rssi}`;
                                div.onclick = () => {
                                    document.getElementById('ssid').value = network.ssid;
                                };
                                list.appendChild(div);
                            });
                        });
                }
            </script>
        </body>
        </html>
    )";
    _server.send(200, "text/html", html);
}

bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}