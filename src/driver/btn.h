#ifndef _BTN_H_
#define _BTN_H_

#include "def.h"

// 버튼 GPIO 설정
#define BTN_DDR      DDRD
#define BTN_PIN      PIND
#define BTN_PORT     PORTD

// 버튼 타이밍 설정
#define DEB_TIME     50      // 디바운스 시간 (ms)

// 버튼 상태 정의
typedef enum {
    BTN_IDLE,       // 대기 상태
    BTN_PRESS,      // 눌림 상태
    BTN_RELEASE     // 뗌 상태
} BtnState;

// 버튼 구조체
typedef struct BTN {
    volatile uint8_t *ddr;
    volatile uint8_t *pin;
    volatile uint8_t *port;
    uint8_t btnPin;
} BTN;

// 인터럽트 타이밍 변수
extern volatile uint32_t int0Time;
extern volatile uint32_t int1Time;
extern volatile uint32_t int2Time;
extern volatile uint32_t int3Time;
extern volatile uint32_t int4Time;
extern volatile uint32_t int5Time;

// 버튼 상태 변수
extern volatile uint8_t btn0Press;
extern volatile uint8_t btn1Press;
extern volatile uint8_t btn2Press;
extern volatile uint8_t btn3Press;
extern volatile uint8_t btn4Press;
extern volatile uint8_t btn5Press;

// 버튼 제어 함수
uint8_t isPressed(const BTN* btn);
void btnInit(BTN* btn, volatile uint8_t* ddr, volatile uint8_t* pin, 
            volatile uint8_t* port, uint8_t pinNumber);

// 외부 인터럽트 초기화 함수
void INT0_Init(void);
void INT1_Init(void);
void INT2_Init(void);
void INT3_Init(void);
void INT4_Init(void);
void INT5_Init(void);

#endif