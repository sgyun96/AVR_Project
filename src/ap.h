#ifndef _AP_H_
#define _AP_H_

#include "def.h"
#include "btn.h"
#include "time.h"
#include "sensor.h"
#include "RGB.h"
#include "buzz.h"
#include "stepMotor.h"
#include "ultrasonic_int.h"



typedef enum {
    MANUAL_STATE_NONE = 0,
    MANUAL_STATE_SAVED = 1
} manual_state_t;

// 도어 상태
typedef enum {
    DOOR_CLOSE,     // 닫힘
    DOOR_OPEN,      // 열림 중
    DOOR_OPENED,    // 열림
    DOOR_CLOSING    // 닫힘 중
} DoorState;

// 알람 모드
typedef enum {
    ALARM_NONE,     // 알람 없음
    ALARM_OPEN,     // 열림 알람
    ALARM_CLOSE,    // 닫힘 알람
    ALARM_PARK      // 주차 알람
} AlarmMode;

// 시퀀스 상태
typedef enum {
    SEQ_WAIT,       // 대기
    SEQ_IN,         // 진입
    SEQ_STAY,       // 주차
    SEQ_OUT         // 출차
} SeqState;

// 알람 제어
typedef struct {
    uint32_t start;     // 시작 시간
    uint32_t lastLed;   // LED 토글 시간
    uint8_t active;     // 알람 동작 여부
    uint8_t led;        // LED 상태
    AlarmMode mode;     // 알람 모드
} Alarm;

// 시스템 설정
typedef struct {
    uint16_t alarmTime;    // 알람 시간
    uint16_t ledTime;      // LED 깜빡임 
    uint16_t buzzDuty;     // 일반 부저 PWM
    uint16_t parkDuty;     // 주차 부저 PWM
    uint16_t motSteps;     // 도어 스텝수
    uint8_t motSpeed;      // 모터 속도
    uint16_t btnTime;      // 버튼 길게 누름 
    uint16_t parkTime;     // 주차 알람 
    uint16_t waitTime;     // 대기 시간
    uint32_t timeoutTime;  // 타임아웃 시간 
} Config;

// 시스템 상태
typedef struct {
    DoorState door;     // 도어 상태
    Alarm alarm;        // 알람 상태
    MotState mot;       // 모터 상태
    BtnState btn;       // 버튼 상태
    SeqState seq;       // 시퀀스 상태
    uint32_t pressTime; // 버튼 누름 시간
    uint32_t timer;     // 타이머
    bool is_manual_operating;
    manual_state_t stored_ext_int_state;
} status_t;

void apInit(void);     // 시스템 초기화
void apMain(void);     // 메인 루프

#endif 