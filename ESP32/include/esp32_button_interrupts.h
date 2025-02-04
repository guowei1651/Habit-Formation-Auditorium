#ifndef BUTTON_INTERRUPTS_H
#define BUTTON_INTERRUPTS_H

void buttonBegin();
void IRAM_ATTR handleButton1();
void IRAM_ATTR handleButton2();
void IRAM_ATTR handleButton3();


#endif