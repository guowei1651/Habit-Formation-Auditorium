#ifndef SHT30_H
#define SHT30_H

#include <Wire.h>

class SHT30 {
public:
    SHT30(TwoWire *wire = &Wire);
    bool begin(void);
    bool readTempHum(float &temp, float &hum);

private:
    TwoWire *_wire;
    uint8_t _address;
};

#endif