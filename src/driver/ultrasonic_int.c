#include "ultrasonic_int.h"
#include "time.h"

// 전역 변수
volatile uint16_t startCnt;    // 에코 시작 시간
volatile uint16_t endCnt;      // 에코 종료 시간
volatile uint16_t usCnt;       // 에코 펄스 폭

// 에코 인터럽트 처리
ISR(INT6_vect)
{
    if(PINE & (1<< ECHO_PIN))    // 상승 에지
    {
        startCnt = TCNT1;    // 시작 시간 저장
        EICRB &= ~(1 << ISC60);    // 하강 에지로 변경
        EICRB |=  (1 << ISC61);     
    }
    else    // 하강 에지
    {
        endCnt = TCNT1;    // 종료 시간 저장
        usCnt = endCnt - startCnt;    // 펄스 폭 계산
        EICRB |= (1 << ISC61) | (1 << ISC60);    // 상승 에지로 복귀
    }
}

void sonicInit(void)
{
    // GPIO 설정
    TRIG_DDR |= (1 << TRIG_PIN);     // 트리거 출력
    ECHO_DDR &= ~(1 << ECHO_PIN);    // 에코 입력
    ECHO_PORT |= (1 << ECHO_PIN);    // 에코 풀업

    // 인터럽트 설정
    EICRB |= (1 << ISC61) | (1 << ISC60);    // 상승 에지
    EIMSK |= (1 << INT6);    // INT6 활성화

    // 타이머 설정
    TCCR1B = 0;
    TCCR1B |= (1 << CS11) | (1 << CS10);    // 분주비 64
    
    // 변수 초기화
    startCnt = 0;
    endCnt = 0;
    usCnt = 0;
}

void sonicTrig(void)
{
    // 트리거 신호 발생 (10us)
    PORTE &= ~(1<<TRIG_PIN);
    _delay_us(1);
    PORTE |= (1<<TRIG_PIN);
    _delay_us(10);
    PORTE &= ~(1<<TRIG_PIN);
}

uint16_t sonicDist(void)
{
    if (usCnt == 0) {
        return 0xFFFF;    // 측정 실패
    }
    // 거리 계산 (cm)
    uint16_t dist = (uint16_t)(usCnt * 0.000004 * 34000) / 2;
    return dist;
}

void initDistLed(void) {
    // 거리 LED 초기화
    DDRE |= (1 << PORTE1) | (1 << PORTE2);    // 출력 설정
    PORTE &= ~((1 << PORTE1) | (1 << PORTE2));    // LED OFF
}

void updateDistLed(void) {
    static uint32_t lastMeas = 0;    // 마지막 측정 시간
    static uint32_t ledOn = 0;       // LED ON 시간
    static uint8_t ledState = 0;     // LED 상태
    uint32_t now = millis();
    
    // 100ms 마다 거리 측정
    if (now - lastMeas >= 100) {
        sonicTrig();
        uint16_t dist = sonicDist();
        
        if (dist != 0xFFFF) {    // 측정 성공
            if (dist < 30) {      // 30cm 이내 감지
                PORTE |= (1 << PORTE1) | (1 << PORTE2);    // LED ON
                ledState = 1;
                ledOn = now;
            }
        } else {    // 측정 실패
            PORTE &= ~((1 << PORTE1) | (1 << PORTE2));    // LED OFF
        }
        
        lastMeas = now;
    }
    
    // LED 5초 타이머
    if (ledState && (now - ledOn >= 5000)) {
        PORTE &= ~((1 << PORTE1) | (1 << PORTE2));    // LED OFF
        ledState = 0;
    }
}