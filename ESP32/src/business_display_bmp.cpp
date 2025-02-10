#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "../include/esp32_log.h"
#include "../include/business_iot.h"
#include "../include/esp32_board.h"

// 因为内存，存储都不够所以直接在一个地方下载并展示。
const char *imageTag = "image";
const char *bmpUrl[] = {
    // 当下
    "http://172.25.1.22:8089/bpdd_4bit.bmp",

    // 久坐
    "http://172.25.1.22:8089/jzdd_1bit.bmp",

    // 持续学习
    "http://172.25.1.22:8089/klkgdd_4bit.bmp",

    // 美图
    "http://172.25.1.22:8089/mtdd_1bit.bmp"};

static uint8_t currentIndex = 0;

bool displayBMP(Display &display, WiFiClient *stream);

bool downloadAndDisplayImage(ESP32Board &board)
{
    if (!board.network->confirmIsConnected()) {
        ESP32_LOGI(imageTag, "确认网络连接失败");
        return false;
    }

    Display& display = *board.display;
    GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> *_display = display.getDEP();

    WiFiClient client;
    HTTPClient http;
    client.setTimeout(15); // 15秒的超时

    http.setReuse(2);

    ESP32_LOGI(imageTag, "开始下载图片: %s", bmpUrl[currentIndex]);
    http.begin(client, bmpUrl[currentIndex]);

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
        ESP32_LOGI(imageTag, "Image Downloaded!");

        WiFiClient *stream = http.getStreamPtr();
        if (!displayBMP(display, stream))
        {
            http.end();
            client.stop();
            return false;
        }
    }
    else
    {
        ESP32_LOGI(imageTag, "Failed to get image: %d\n", httpCode);
        return false;
    }

    http.end();
    client.stop();

    currentIndex = (currentIndex + 1) % 4;
    return true;
}

bool displayBMP(Display &display, WiFiClient *stream);
void process1BitBMP(Display &display, WiFiClient *stream, int width, int height, int rowSize);
void process4BitBMP(Display &display, WiFiClient *stream, int width, int height, int rowSize);
void process24BitBMP(Display &display, WiFiClient *stream, int width, int height, int rowSize);

bool displayBMP(Display &display, WiFiClient *stream)
{
    uint8_t bmpHeader[54];
    stream->read(bmpHeader, 54);

    int width = bmpHeader[18] | (bmpHeader[19] << 8);
    int height = bmpHeader[22] | (bmpHeader[23] << 8);
    int bitDepth = bmpHeader[28];
    int dataOffset = bmpHeader[10] | (bmpHeader[11] << 8);

    int rowSize = ((width * bitDepth + 31) / 32) * 4; // BMP 行字节数（4 字节对齐）

    ESP32_LOGI(imageTag, "BMP Width: %d, Height: %d, BitDepth: %d dataOffset: %d rowSize: %d", width, height, bitDepth, dataOffset, rowSize);
    if (dataOffset > 54)
    {
        uint16_t seekLength = dataOffset - 54;
        uint8_t seekData[seekLength];
        stream->read(seekData, seekLength); // **跳过 BMP 头部，直接读取像素数据**
    }

    GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> *_display = display.getDEP();

    _display->setFullWindow();
    _display->fillScreen(GxEPD_WHITE);
    _display->firstPage();

    switch (bitDepth)
    {
    case 1:
        process1BitBMP(display, stream, width, height, rowSize);
        break;
    case 4:
        process4BitBMP(display, stream, width, height, rowSize);
        break;
    case 24:
        process24BitBMP(display, stream, width, height, rowSize);
        break;
    default:
        ESP32_LOGI(imageTag, "Unsupported bit depth: %d", bitDepth);
        return false;
    }

    return true;
}

// **处理 1-bit BMP**
void process1BitBMP(Display &display, WiFiClient *stream, int width, int height, int rowSize)
{
    GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> *_display = display.getDEP();
    uint8_t rowBuffer[rowSize];

    do
    {
        for (int y = 0; y < height; y++)
        {
            stream->read(rowBuffer, rowSize);
            for (int x = 0; x < width; x++)
            {
                int byteIndex = x / 8;
                int bitIndex = 7 - (x % 8);
                uint8_t bit = (rowBuffer[byteIndex] >> bitIndex) & 0x1;
                int correctedY = height - y - 1; // 修正 Y 轴翻转
                _display->drawPixel(x, correctedY, bit ? GxEPD_WHITE : GxEPD_BLACK);
            }
        }
    } while (_display->nextPage());
}

// **处理 4-bit BMP**
void process4BitBMP(Display &display, WiFiClient *stream, int width, int height, int rowSize)
{
    GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> *_display = display.getDEP();
    uint8_t rowBuffer[rowSize];

    do
    {
        for (int y = 0; y < height; y++)
        {
            stream->read(rowBuffer, rowSize);
            for (int x = 0; x < width; x++)
            {
                int byteIndex = x / 2;
                bool isHighNibble = (x % 2 == 0);
                uint8_t pixelData = rowBuffer[byteIndex];
                uint8_t pixelValue = isHighNibble ? (pixelData >> 4) : (pixelData & 0xF);

                uint16_t color = GxEPD_WHITE;
                if (pixelValue == 0x0)
                {
                    color = GxEPD_BLACK;
                }
                else if (pixelValue >= 0xC)
                {
                    color = GxEPD_RED;
                }

                int correctedY = height - y - 1;
                _display->drawPixel(x, correctedY, color);
            }
        }
    } while (_display->nextPage());
}

// **处理 24-bit BMP**
void process24BitBMP(Display &display, WiFiClient *stream, int width, int height, int rowSize)
{
    GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> *_display = display.getDEP();
    uint8_t rowBuffer[rowSize];

    do
    {
        for (int y = 0; y < height; y++)
        {
            stream->read(rowBuffer, rowSize);
            for (int x = 0; x < width; x++)
            {
                int index = x * 3;
                uint8_t blue = rowBuffer[index];
                uint8_t green = rowBuffer[index + 1];
                uint8_t red = rowBuffer[index + 2];

                uint16_t color = GxEPD_WHITE;
                if (red > 200 && green < 100 && blue < 100)
                {
                    color = GxEPD_RED;
                }
                else if (red < 100 && green < 100 && blue < 100)
                {
                    color = GxEPD_BLACK;
                }

                int correctedY = height - y - 1;
                _display->drawPixel(x, correctedY, color);
            }
        }
    } while (_display->nextPage());
}
