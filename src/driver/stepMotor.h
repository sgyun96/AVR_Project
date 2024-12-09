#ifndef STEP_MOTOR_H
#define STEP_MOTOR_H

#include "def.h"

// 모터 상태
typedef enum {
    MOT_IDLE,    // 대기
    MOT_RUN,     // 동작
    MOT_STOP     // 정지
} MotState;
    
// 모터 핀 설정
#define MOT_PIN1   PORTB0    
#define MOT_PIN2   PORTB1    
#define MOT_PIN3   PORTB2   
#define MOT_PIN4   PORTB3   

// 모터 제어 구조체
typedef struct {
    uint16_t steps;    // 남은 스텝 수
    uint8_t step;      // 현재 스텝
    uint8_t dir;       // 회전 방향
    uint32_t last;     // 마지막 스텝 시간
} MotCtrl;

// 모터 제어
void motInit(void);     // 초기화
void motStep(uint8_t step);    // 스텝 실행
void motStart(uint16_t steps, uint8_t clockwise);    // 모터 시작
void motUpdate(void);   // 상태 갱신
void motStop(void);     // 모터 정지

// 전역 변수
extern volatile uint16_t motSteps;    // 남은 스텝 수
extern volatile uint8_t motDir;       // 회전 방향

#endif