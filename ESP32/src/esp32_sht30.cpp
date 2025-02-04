#include "../include/esp32_pins.h"
#include "../include/esp32_sht30.h"

#define SHT30_ADDR 0x44
#define SHT30_MEAS_HIGHREP_STRETCH 0x2C06

SHT30::SHT30(TwoWire *wire) {
    _wire = wire;
    _address = SHT30_ADDR;
}

bool SHT30::begin() {
    _wire->begin(SDA_PIN, SCL_PIN);
    return true;
}

bool SHT30::readTempHum(float &temp, float &hum) {
    uint8_t data[6];
    
    _wire->beginTransmission(_address);
    _wire->write(0x2C);
    _wire->write(0x06);
    if (_wire->endTransmission() != 0) {
        return false;
    }
    
    delay(50);
    
    _wire->requestFrom(_address, 6);
    if (_wire->available() != 6) {
        return false;
    }
    
    for (int i = 0; i < 6; i++) {
        data[i] = _wire->read();
    }
    
    uint16_t ST, SRH;
    ST = (data[0] << 8) | data[1];
    SRH = (data[3] << 8) | data[4];
    
    temp = -45 + (175.0 * (float)ST / 65535.0);
    hum = 100.0 * (float)SRH / 65535.0;
    
    return true;
}