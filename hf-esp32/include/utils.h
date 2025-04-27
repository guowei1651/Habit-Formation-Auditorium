#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <u8g2_fonts.h>

extern const uint8_t chinese_gb2312[254049] U8G2_FONT_SECTION("chinese_gb2312");

#include <pgmspace.h>

extern const unsigned char Bitmap_gengxing[] PROGMEM;
extern const unsigned char Bitmap_weizhi[] PROGMEM;
extern const unsigned char Bitmap_zhuangtai[] PROGMEM;
extern const unsigned char Bitmap_humidity[] PROGMEM;
extern const unsigned char Bitmap_fx[] PROGMEM;
extern const unsigned char Bitmap_batlow[] PROGMEM;
extern const unsigned char Bitmap_bat3[] PROGMEM;
extern const unsigned char Bitmap_bat2[] PROGMEM;
extern const unsigned char Bitmap_bat1[] PROGMEM;
extern const unsigned char Bitmap_qt[] PROGMEM;
extern const unsigned char Bitmap_qt_ws[] PROGMEM;
extern const unsigned char Bitmap_dy[] PROGMEM;
extern const unsigned char Bitmap_dy_ws[] PROGMEM;
extern const unsigned char Bitmap_yt[] PROGMEM;
extern const unsigned char Bitmap_zheny[] PROGMEM;
extern const unsigned char Bitmap_lzy[] PROGMEM;
extern const unsigned char Bitmap_lzybbb[] PROGMEM;
extern const unsigned char Bitmap_xy[] PROGMEM;
extern const unsigned char Bitmap_zhongy[] PROGMEM;
extern const unsigned char Bitmap_dayu[] PROGMEM;
extern const unsigned char Bitmap_by[] PROGMEM;
extern const unsigned char Bitmap_dby[] PROGMEM;
extern const unsigned char Bitmap_tdby[] PROGMEM;
extern const unsigned char Bitmap_dongy[] PROGMEM;
extern const unsigned char Bitmap_yjx[] PROGMEM;
extern const unsigned char Bitmap_zhenx[] PROGMEM;
extern const unsigned char Bitmap_xx[] PROGMEM;
extern const unsigned char Bitmap_dx[] PROGMEM;
extern const unsigned char Bitmap_bx[] PROGMEM;
extern const unsigned char Bitmap_fc[] PROGMEM;
extern const unsigned char Bitmap_ys[] PROGMEM;
extern const unsigned char Bitmap_scb[] PROGMEM;
extern const unsigned char Bitmap_w[] PROGMEM;
extern const unsigned char Bitmap_m[] PROGMEM;
extern const unsigned char Bitmap_f[] PROGMEM;
extern const unsigned char Bitmap_jf[] PROGMEM;
extern const unsigned char Bitmap_rdfb[] PROGMEM;
extern const unsigned char Bitmap_wz[] PROGMEM;
extern const unsigned char Bitmap_zhongx[] PROGMEM;
extern const unsigned char Bitmap_ljf[] PROGMEM;
extern const unsigned char Bitmap_yt_ws[] PROGMEM;
extern const unsigned char Bitmap_zy_ws[] PROGMEM;
extern const unsigned char Bitmap_zx_ws[] PROGMEM;
extern const unsigned char Bitmap_tempSHT30[] PROGMEM;
extern const unsigned char Bitmap_tempSHT30[] PROGMEM;
extern const unsigned char Bitmap_humiditySHT30[] PROGMEM;
extern const unsigned char Bitmap_jintian[] PROGMEM;
extern const unsigned char Bitmap_dlxtb[] PROGMEM;
extern const unsigned char Bitmap_yxm_yzq_50[] PROGMEM;

String callHttps(String url);
String callHttp(String url);

void espSleep(uint32_t seconds);

#endif