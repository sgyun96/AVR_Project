#include "RGB.h"

void ledInit(void)
{
    LED_DDR |= (1 << RED_PIN) | (1 << GREEN_PIN) | (1 << BLUE_PIN);
    ledOffAll();
}

void ledOn(RGB_Color color)
{
    switch (color)
    {
        case RED:
            LED_PORT |= (1 << RED_PIN);
            break;
        case GREEN:
            LED_PORT |= (1 << GREEN_PIN);
            break;
        case BLUE:
            LED_PORT |= (1 << BLUE_PIN);
            break;
    }
}

void ledOff(RGB_Color color)
{
    switch (color)
    {
        case RED:
            LED_PORT &= ~(1 << RED_PIN);
            break;
        case GREEN:
            LED_PORT &= ~(1 << GREEN_PIN);
            break;
        case BLUE:
            LED_PORT &= ~(1 << BLUE_PIN);
            break;
    }
}

void ledOffAll(void)
{
    LED_PORT &= ~((1 << RED_PIN) | (1 << GREEN_PIN) | (1 << BLUE_PIN));
}