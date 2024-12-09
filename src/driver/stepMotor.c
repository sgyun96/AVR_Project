#include "stepMotor.h"
#include "time.h"
#include "ap.h"

// 전역 변수
volatile uint16_t motSteps = 0;    // 남은 스텝 수
volatile uint8_t motIdx = 0;       // 현재 스텝 인덱스
volatile uint8_t motDir = 1;       // 회전 방향 (1:시계, 0:반시계)
volatile uint32_t lastStep = 0;    // 마지막 스텝 시간

void motInit(void)
{
    // 모터 핀 출력 설정
    DDRB |= (1 << MOT_PIN1) | (1 << MOT_PIN2) | (1 << MOT_PIN3) | (1 << MOT_PIN4);
}

void motStep(uint8_t step)
{
    switch (step) {
        case 0:    
            PORTB = (1 << MOT_PIN1);
            break;
        case 1:    
            PORTB = (1 << MOT_PIN2);
            break;
        case 2:    
            PORTB = (1 << MOT_PIN3);
            break;
        case 3:    
            PORTB = (1 << MOT_PIN4);
            break;
        default:   
            PORTB = 0;
            break;
    }
}

void motStart(uint16_t steps, uint8_t clockwise)
{
    motSteps = steps;              // 목표 스텝 설정
    motDir = clockwise ? 1 : 0;    // 회전 방향 설정
    motIdx = 0;                    // 스텝 인덱스 초기화
    lastStep = millis();           // 시작 시간 저장
}

void motUpdate(void)
{
    if (motSteps == 0) return;    // 남은 스텝 없으면 종료

    if (millis() - lastStep >= 5)    // 5ms 간격으로 스텝 실행
    {
        motStep(motIdx);           // 현재 스텝 실행
        lastStep = millis();       // 시간 갱신

        // 다음 스텝 계산
        if (motDir) {
            motIdx = (motIdx + 1) % 4;    // 시계 방향
        } else {
            motIdx = (motIdx + 3) % 4;    // 반시계 방향
        }

        motSteps--;    // 남은 스텝 감소
    }
}

void motStop(void) {
    motSteps = 0;    // 모터 정지
}