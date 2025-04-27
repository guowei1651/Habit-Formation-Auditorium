#include "../include/utils.h"
#include "../include/business_main.h"

// TODO: 维护设备状态
// TODO: 展示内容

Business::Business(ESP32Board esp32) : _esp32(esp32) {};

void Business::begin()
{
}

void Business::loop()
{
}

void display_tbcs(GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT> &display) // 图标测试
{
    display.fillRect(10, 10, 300, 300, GxEPD_WHITE); // 填充屏幕
    display.setPartialWindow(0, 0, 300, 300);        // 设置局部刷新窗口
    display.firstPage();
    do
    {
        display.drawInvertedBitmap(0, 0, Bitmap_qt, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(50, 0, Bitmap_dy, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(100, 0, Bitmap_yt, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(150, 0, Bitmap_zheny, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(200, 0, Bitmap_lzybbb, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(250, 0, Bitmap_xy, 45, 45, GxEPD_BLACK);

        display.drawInvertedBitmap(0, 40, Bitmap_zhongy, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(50, 40, Bitmap_dayu, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(100, 40, Bitmap_by, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(150, 40, Bitmap_dby, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(200, 40, Bitmap_tdby, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(250, 40, Bitmap_dongy, 45, 45, GxEPD_BLACK);

        display.drawInvertedBitmap(0, 80, Bitmap_yjx, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(50, 80, Bitmap_zhenx, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(100, 80, Bitmap_xx, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(150, 80, Bitmap_zhongx, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(200, 80, Bitmap_dx, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(250, 80, Bitmap_bx, 45, 45, GxEPD_BLACK);
    } while (display.nextPage());
    delay(10000);

    display.fillRect(10, 10, 300, 300, GxEPD_WHITE); // 填充屏幕
    display.setPartialWindow(0, 0, 300, 300);        // 设置局部刷新窗口
    display.firstPage();
    do
    {
        display.drawInvertedBitmap(0, 0, Bitmap_fc, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(50, 0, Bitmap_ys, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(100, 0, Bitmap_scb, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(150, 0, Bitmap_w, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(200, 0, Bitmap_m, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(250, 0, Bitmap_f, 45, 45, GxEPD_BLACK);

        display.drawInvertedBitmap(0, 40, Bitmap_jf, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(50, 40, Bitmap_rdfb, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(100, 40, Bitmap_ljf, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(150, 40, Bitmap_wz, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(200, 40, Bitmap_qt_ws, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(250, 40, Bitmap_yt_ws, 45, 45, GxEPD_BLACK);

        display.drawInvertedBitmap(0, 80, Bitmap_dy_ws, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(50, 80, Bitmap_zy_ws, 45, 45, GxEPD_BLACK);
        display.drawInvertedBitmap(100, 80, Bitmap_zx_ws, 45, 45, GxEPD_BLACK);
    } while (display.nextPage());
    delay(10000);

    display.display(1);
}