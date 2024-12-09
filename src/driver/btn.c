#include "btn.h"
#include "time.h"

// 인터럽트 타이밍
volatile uint32_t int0Time = 0;
volatile uint32_t int1Time = 0;
volatile uint32_t int2Time = 0;
volatile uint32_t int3Time = 0;
volatile uint32_t int4Time = 0;
volatile uint32_t int5Time = 0;

// 버튼 상태
volatile uint8_t btn0Press = 0;
volatile uint8_t btn1Press = 0;
volatile uint8_t btn2Press = 0;
volatile uint8_t btn3Press = 0;
volatile uint8_t btn4Press = 0;
volatile uint8_t btn5Press = 0;

// 버튼 초기화
void btnInit(BTN* btn, volatile uint8_t* ddr, volatile uint8_t* pin,
            volatile uint8_t* port, uint8_t pinNumber)
{
    btn->ddr = ddr;
    btn->pin = pin;
    btn->port = port;
    btn->btnPin = pinNumber;
    
    *(btn->ddr) &= ~(1 << btn->btnPin);    // 입력으로 설정
    *(btn->port) |= (1 << btn->btnPin);    // 풀업 저항 활성화
}

// 외부 인터럽트 초기화
void INT0_Init(void) {
    EICRA |= (1 << ISC01);    // 하강 에지 트리거
    EIMSK |= (1 << INT0);     // INT0 활성화
}

void INT1_Init(void) {
    EICRA |= (1 << ISC11);    // 하강 에지 트리거
    EIMSK |= (1 << INT1);     // INT1 활성화
}

void INT2_Init(void) {
    EICRA |= (1 << ISC21);    // 하강 에지 트리거
    EIMSK |= (1 << INT2);     // INT2 활성화
}

void INT3_Init(void) {
    EICRA |= (1 << ISC31);    // 하강 에지 트리거
    EIMSK |= (1 << INT3);     // INT3 활성화
}

void INT4_Init(void) {
    EICRB |= (1 << ISC41);    // 하강 에지 트리거
    EIMSK |= (1 << INT4);     // INT4 활성화
}

void INT5_Init(void) {
    EICRB |= (1 << ISC51);    // 하강 에지 트리거
    EIMSK |= (1 << INT5);     // INT5 활성화
}

// 버튼 인터럽트 처리
ISR(INT0_vect) {
    uint32_t now = millis();
    if (now - int0Time >= DEB_TIME) {    // 디바운스 체크
        int0Time = now;
        btn0Press = 1;
    }
}

ISR(INT1_vect) {
    uint32_t now = millis();
    if (now - int1Time >= DEB_TIME) {
        int1Time = now;
        btn1Press = 1;
    }
}

ISR(INT2_vect) {
    uint32_t now = millis();
    if (now - int2Time >= DEB_TIME) {
        int2Time = now;
        btn2Press = 1;
    }
}

ISR(INT3_vect) {
    uint32_t now = millis();
    if (now - int3Time >= DEB_TIME) {
        int3Time = now;
        btn3Press = 1;
    }
}

ISR(INT4_vect) {
    uint32_t now = millis();
    if (now - int4Time >= DEB_TIME) {
        int4Time = now;
        btn4Press = 1;
    }
}

ISR(INT5_vect) {
    uint32_t now = millis();
    if (now - int5Time >= DEB_TIME) {
        int5Time = now;
        btn5Press = 1;
    }
}

// 버튼 상태 확인
uint8_t isPressed(const BTN* btn) {
    return !(*btn->pin & (1 << btn->btnPin));
}