#include "sensor.h"
#include "time.h"

// 동작 감지 상태
static uint32_t motTime = 0;    // 감지 시간
static uint8_t motDet = 0;      // 감지 플래그

void sensorInit(void) {
    DDRC &= ~(1 << PIR_PIN);    // PIR 센서 입력 설정
    PORTC |= (1 << PIR_PIN);    // 풀업 저항 활성화
    DDRC |= (1 << LED_PIN);     // LED 출력 설정
    PORTC &= ~(1 << LED_PIN);   // LED 초기 OFF
}

uint8_t isMotion(void) {
    return (PINC & (1 << PIR_PIN));    // PIR 센서 상태 확인
}

void motLedOn(void) {
    PORTC |= (1 << LED_PIN);    // LED ON
}

void motLedOff(void) {
    PORTC &= ~(1 << LED_PIN);   // LED OFF
}

void motHandle(void) {
    uint32_t now = millis();
    
    if (isMotion()) {           // 동작 감지
        motTime = now;
        motDet = 1;
        motLedOn();
    } 
    else if (motDet) {          // 감지 후 타이머
        if (now - motTime >= 5000) {    // 5초 후 LED OFF
            motDet = 0;
            motLedOff();
        }
    }
}