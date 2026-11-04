#include "driver/gpio.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef enum {
    off,
    on
} ledMode;

typedef struct {
    gpio_num_t gpioNum;
    uint16_t blinkDelayMs;
    ledMode modeState;
} ledConfig;

static ledConfig leds[] = {
    {.gpioNum = GPIO_NUM_21, .blinkDelayMs = 200, .modeState = off},
    {.gpioNum = GPIO_NUM_45, .blinkDelayMs = 500, .modeState = off},
    {.gpioNum = GPIO_NUM_44, .blinkDelayMs = 1000, .modeState = off},
};

#define LED_COUNT (sizeof(leds) / sizeof(leds[0]))

static void initLed(const ledConfig *led) {
    gpio_config_t ledGpioConfig = {
        .pin_bit_mask = (1ULL << led->gpioNum),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&ledGpioConfig));
    gpio_set_level(led->gpioNum, led->modeState);
}

static void toggleLedLevel(ledConfig *led) {
    led->modeState = !led->modeState;
    gpio_set_level(led->gpioNum, led->modeState);
}

static void blinkLedTask(void *taskParameter) {
    ledConfig *led = taskParameter;
    TickType_t lastWakeTime = xTaskGetTickCount();
    while (1) {
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(led->blinkDelayMs));
        toggleLedLevel(led);
    }
}

void app_main(void) {
    for (int ledIndex = 0; ledIndex < LED_COUNT; ledIndex++) {
        initLed(&leds[ledIndex]);
        xTaskCreate(blinkLedTask, "blinkLed", 2048, &leds[ledIndex], 1, NULL);
    }
}
