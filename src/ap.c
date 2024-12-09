#include "ap.h"

// 버튼 이벤트 타입
typedef enum {
    BTN_EVENT_NONE,
    BTN_EVENT_MANUAL,     // 수동 버튼
    BTN_EVENT_EXT_PAIR,   // 외부 쌍버튼
    BTN_EVENT_INT_PAIR    // 내부 쌍버튼
} BtnEventType;

// 버튼 이벤트 구조체
typedef struct {
    BtnEventType type;
    uint32_t pressTime;
    uint8_t isLongPress;
} BtnEvent;

// 버튼 상태 관리
typedef struct {
    uint8_t isParked;         // 주차 상태
    uint8_t isManualMode;     // 수동 모드
    uint32_t lastAction;      // 마지막 동작 시간
} BtnStatus;


// 전역 변수
static BTN extBtn[3];    // 외부 버튼
static BTN intBtn[3];    // 내부 버튼
static BtnStatus btnStatus = {0, 0, 0};


// 시스템 설정값
static const Config cfg = {
    .alarmTime = 5000,    // 알람 시간 10초
    .ledTime = 300,        // LED 깜빡임 0.3초
    .buzzDuty = 180,       // 일반 부저 PWM
    .parkDuty = 240,       // 주차 부저 PWM
    .motSteps = 1000,      // 도어 스텝수
    .motSpeed = 1,         // 모터 속도
    .btnTime = 3000,       // 버튼 길게 누름 3초
    .parkTime = 1000,      // 주차 알람 1초
    .waitTime = 3000,       // 대기 시간 3초
    .timeoutTime = 30000    // 타임아웃 시간 30초
};

// 시스템 상태 초기화
status_t st = {
    .seq = 0,                                    // 현재 시퀀스 상태 (SEQ_WAIT)
    .door = DOOR_CLOSE,                          // 도어 초기 상태 (닫힘)
    .alarm = {0},                                // 알람 상태 초기화
    .mot = MOT_IDLE,                             // 모터 초기 상태 (대기)
    .btn = BTN_IDLE,                             // 버튼 초기 상태 (대기)
    .pressTime = 0,                              // 버튼 누름 시간 초기화
    .timer = 0,                                  // 시스템 타이머 초기화
    .is_manual_operating = false,                // 수동 조작 모드 비활성화
    .stored_ext_int_state = MANUAL_STATE_NONE    // 저장된 내외부 상태 없음
};

// 알람 시작 - 알람 상태를 초기화하고 LED/부저를 설정
static void startAlarm(AlarmMode mode) {
    Alarm* alarm = &st.alarm;
    alarm->start = millis();          // 알람 시작 시간 저장
    alarm->active = 1;                // 알람 활성화
    alarm->mode = mode;               // 알람 모드 설정 (열림/닫힘/주차)
    alarm->led = 0;                   // LED 초기 상태 OFF
    alarm->lastLed = millis();        // LED 토글 타이밍 초기화
    buzzInit();                       // 부저 초기화
    buzzStop();                       // 부저 정지
    ledOffAll();                      // 모든 LED 끄기
}

// 도어 제어
static void controlDoor(DoorState newState, uint8_t isOpen) {
    if (st.mot == MOT_IDLE) {
        motStart(cfg.motSteps, isOpen);
        st.door = newState;
        startAlarm(isOpen ? ALARM_OPEN : ALARM_CLOSE);
    }
}

// 통합 버튼 처리
static BtnEvent checkButtons(void) {
    uint32_t now = millis();
    static uint32_t lastManual = 0;
    static uint32_t extPairPressStart = 0;
    static uint32_t intPairPressStart = 0;
    static uint8_t extPairPressed = 0;
    static uint8_t intPairPressed = 0;
    static uint8_t intPairLongPressed = 0;
    BtnEvent event = {BTN_EVENT_NONE, 0, 0};
    
    // 수동 버튼 체크 (200ms 디바운스)
    if ((isPressed(&extBtn[2]) || isPressed(&intBtn[2])) && 
        (now - lastManual >= 200)) {
        event.type = BTN_EVENT_MANUAL;
        lastManual = now;
        btnStatus.isManualMode = 1;
    }
    
    // 외부 버튼 체크
    if (isPressed(&extBtn[0]) && isPressed(&extBtn[1])) {
        if (!extPairPressed) {
            extPairPressStart = now;
            extPairPressed = 1;
        }
        if (now - extPairPressStart >= cfg.btnTime) {
            event.type = BTN_EVENT_EXT_PAIR;
            event.isLongPress = 1;
            event.pressTime = now;
        }
    } else {
        extPairPressed = 0;
        extPairPressStart = 0;  // 타이머도 초기화
    }
    
    // 내부 버튼 체크
    if (isPressed(&intBtn[0]) && isPressed(&intBtn[1])) {
        if (!intPairPressed) {
            intPairPressStart = now;
            intPairPressed = 1;
        }
        if (!intPairLongPressed && (now - intPairPressStart >= cfg.btnTime)) {
            event.type = BTN_EVENT_INT_PAIR;
            event.isLongPress = 1;
            event.pressTime = now;
            intPairLongPressed = 1;
        }
    } else {
        if (intPairPressed && intPairLongPressed) {
            event.type = BTN_EVENT_INT_PAIR;
            event.isLongPress = 2;  // 릴리즈 표시
            event.pressTime = now;
        }
        intPairPressed = 0;
        intPairLongPressed = 0;
        intPairPressStart = 0;  // 타이머도 초기화
    }
    
    return event;
}

// 상태 업데이트
static void updateState(void) {
    BtnEvent event = checkButtons();
    uint32_t now = millis();
    
    if (st.mot != MOT_IDLE) return;

    // 수동 버튼 
    if (event.type == BTN_EVENT_MANUAL) {
        if (!st.is_manual_operating) {
            // 수동 조작 시작
            st.is_manual_operating = true;
            // 현재 상태 저장
            if (st.door == DOOR_CLOSE) {
                controlDoor(DOOR_OPEN, 1);
            }
            else if (st.door == DOOR_OPENED) {
                controlDoor(DOOR_CLOSING, 0);
            }
        }
    }
    
    // 수동 사이클 완료 체크
    if (st.is_manual_operating) {
        if ((st.door == DOOR_OPENED) || (st.door == DOOR_CLOSE)) {
            st.is_manual_operating = false;  // 사이클 완료
        }
    }
    
    // 내외부 버튼 처리
    if (!st.is_manual_operating) {
        // 정상적으로 내외부 시퀀스 처리
        if (st.stored_ext_int_state != MANUAL_STATE_NONE) {
            // 저장된 명령 실행
            st.seq = st.stored_ext_int_state;
            st.stored_ext_int_state = MANUAL_STATE_NONE;
        }
    } else {
        // 수동 조작 중이면 명령 저장
        if (event.type != BTN_EVENT_MANUAL) {
            st.stored_ext_int_state = st.seq;
        }
    }

    switch(st.seq) {
        case SEQ_WAIT:    // 대기 상태
            if (event.type == BTN_EVENT_MANUAL) {
                if (st.door == DOOR_CLOSE) {
                    controlDoor(DOOR_OPEN, 1);
                }
                else if (st.door == DOOR_OPENED) {
                    controlDoor(DOOR_CLOSING, 0);
                }
            }
            else if (!btnStatus.isManualMode && 
                     event.type == BTN_EVENT_EXT_PAIR && 
                     event.isLongPress) {
                controlDoor(DOOR_OPEN, 1);
                st.seq = SEQ_IN;
            }
            break;
            
        case SEQ_IN:    // 진입 상태
            if (st.door == DOOR_OPENED) {   
                if (event.type == BTN_EVENT_INT_PAIR) {
                    if (event.isLongPress) {  // 3초 이상 눌림 확인
                        st.seq = SEQ_STAY;
                        btnStatus.isParked = 1;
                        btnStatus.isManualMode = 0;
                        startAlarm(ALARM_PARK);
                        controlDoor(DOOR_CLOSING, 0);  // 주차 시 문 닫기 추가
                    }
                }
                else if (event.type == BTN_EVENT_MANUAL) {
                    controlDoor(DOOR_CLOSING, 0);
                    st.seq = SEQ_WAIT;
                    btnStatus.isParked = 0;
                }
            }
            break;
            
        case SEQ_STAY:    // 주차 상태
            if (st.mot != MOT_IDLE) return;
            
            if (event.type == BTN_EVENT_INT_PAIR) {
                if (event.isLongPress == 1) {  // 3초 눌림
                    if (st.door == DOOR_OPENED) {
                        controlDoor(DOOR_CLOSING, 0);
                        btnStatus.isParked = 1;
                        btnStatus.isManualMode = 0;  // 수동 모드 해제
                    }
                }
                else if (event.isLongPress == 2) {  // 릴리즈
                    if (btnStatus.isParked && st.door == DOOR_CLOSE) {
                        controlDoor(DOOR_OPEN, 1);
                        st.seq = SEQ_OUT;
                        btnStatus.isParked = 0;
                        btnStatus.isManualMode = 0;  // 수동 모드 해제
                    }
                }
            }
            else if (event.type == BTN_EVENT_MANUAL) {
                if (st.door == DOOR_CLOSE) {
                    controlDoor(DOOR_OPEN, 1);
                    if (btnStatus.isParked) {
                        st.seq = SEQ_OUT;
                        btnStatus.isParked = 0;
                    }
                }
                else if (st.door == DOOR_OPENED) {
                    controlDoor(DOOR_CLOSING, 0);
                }
            }
            break;
            
        case SEQ_OUT:    // 출차 상태
            if (st.mot != MOT_IDLE) return;

            // 외부 버튼으로만 문 닫기 가능
            if (isPressed(&extBtn[0]) && isPressed(&extBtn[1])) {
                if (st.door == DOOR_OPENED) {
                    controlDoor(DOOR_CLOSING, 0);
                    st.seq = SEQ_WAIT;
                    btnStatus.isParked = 0;
                    btnStatus.isManualMode = 0;
                }
            }
            // 내부 3초 눌림 -> 주차 상태로
            else if (event.type == BTN_EVENT_INT_PAIR && event.isLongPress == 1) {
                if (st.door == DOOR_OPENED) {
                    controlDoor(DOOR_CLOSING, 0);
                    st.seq = SEQ_STAY;
                    btnStatus.isParked = 1;
                    btnStatus.isManualMode = 0;
                }
            }
            break;
    }

    // 타임아웃 처리
    if (now - btnStatus.lastAction > cfg.timeoutTime) {  // 30초
        // 진입 상태(SEQ_IN)일 때만 타임아웃 처리
        if (st.seq == SEQ_IN) {
            if (st.door == DOOR_CLOSE) {
                if (st.seq != SEQ_WAIT) {
                    st.seq = SEQ_WAIT;
                    btnStatus.isManualMode = 0;
                    startAlarm(ALARM_PARK);
                }
            }
            else if (st.door == DOOR_OPENED) {
                controlDoor(DOOR_CLOSING, 0);
                st.seq = SEQ_WAIT;  // 시퀀스 초기화
                btnStatus.isManualMode = 0;
                startAlarm(ALARM_PARK);
            }
        }
    }
    
    btnStatus.lastAction = now;
}

// LED 상태 갱신
static void updateLed(void) {
    uint32_t now = millis();
    Alarm* alarm = &st.alarm;
    
    ledOffAll();
    
    if (alarm->active) {
        if (now - alarm->lastLed >= cfg.ledTime) {
            alarm->lastLed = now;
            
            if (!alarm->led) {
                switch (alarm->mode) {
                    case ALARM_OPEN:
                    case ALARM_CLOSE:
                        ledOn(RED);
                        buzzInit();
                        buzzRun(cfg.buzzDuty);
                        break;
                    case ALARM_PARK:
                        buzzInit();
                        buzzRun(cfg.parkDuty);
                        break;
                    default:
                        buzzStop();
                        break;
                }
            }
            alarm->led = !alarm->led;
        }
        
        uint32_t alarmDur = (alarm->mode == ALARM_PARK) ? 
                            cfg.parkTime : cfg.alarmTime;
        
        if (now - alarm->start >= alarmDur) {
            alarm->active = 0;
            buzzStop();
        }
    }
    
    if (!alarm->active || alarm->led == 0) {
        if (st.door == DOOR_CLOSE) {
            ledOn(BLUE);
        }
        else if (st.door == DOOR_OPEN || st.door == DOOR_CLOSING) {
            ledOn(RED);
        }
        else if (st.door == DOOR_OPENED) {
            ledOn(GREEN);
        }
    }
}

void apInit(void) {
    motInit();
    timeInit();
    sensorInit();
    ledInit();
    buzzInit();
    
    btnInit(&extBtn[0], &DDRD, &PIND, &PORTD, PORTD0);
    btnInit(&extBtn[1], &DDRD, &PIND, &PORTD, PORTD1);
    btnInit(&intBtn[0], &DDRD, &PIND, &PORTD, PORTD2);
    btnInit(&intBtn[1], &DDRD, &PIND, &PORTD, PORTD3);
    btnInit(&extBtn[2], &DDRE, &PINE, &PORTE, PORTE4);
    btnInit(&intBtn[2], &DDRE, &PINE, &PORTE, PORTE5);
    
    initDistLed();
    sonicInit();
    
    sei();
}

void apMain(void) {
    while (1) {
        updateState();
        updateDistLed();
        motHandle();
        
        if (motSteps == 1) {
            if (st.door == DOOR_OPEN) {
                st.door = DOOR_OPENED;
                startAlarm(ALARM_PARK);
            } 
            else if (st.door == DOOR_CLOSING) {
                st.door = DOOR_CLOSE;
                startAlarm(ALARM_PARK);
            }
        }
        
        updateLed();
        motUpdate();
    }
}