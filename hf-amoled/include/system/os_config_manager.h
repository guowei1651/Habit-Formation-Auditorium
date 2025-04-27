#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Preferences.h>
#include <Arduino.h>
#include <map>

class ConfigManager {
public:
    ConfigManager();
    void begin();
    bool setValue(const char* key, const String& value);
    String getValue(const char* key, const String& defaultValue = "");
    void clearAll();
    void commit();
    bool isDirty() const;

private:
    Preferences preferences;
    const char* NVS_NAMESPACE = "habit";
    std::map<String, String> configCache;
    bool dirty;
    
    void loadFromNVS(const char* key);
    void saveToNVS(const char* key, const String& value);
};

#endif