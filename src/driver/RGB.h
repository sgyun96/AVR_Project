#ifndef _RGB_H_
#define _RGB_H_

#include "def.h"

// LED 포트 설정
#define LED_DDR     DDRA
#define LED_PORT    PORTA

// LED 핀 설정
#define RED_PIN     1    // 빨간색 LED
#define GREEN_PIN   2    // 초록색 LED
#define BLUE_PIN    3    // 파란색 LED

// LED 색상
typedef enum {
    RED,      // 빨간색
    GREEN,    // 초록색
    BLUE,     // 파란색
} RGB_Color;

// LED 제어
void ledInit(void);              // LED 초기화
void ledOn(RGB_Color color);     // LED ON
void ledOff(RGB_Color color);    // LED OFF
void ledOffAll(void);           // 전체 LED OFF

#endif