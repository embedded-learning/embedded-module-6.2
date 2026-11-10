#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_log.h"

typedef struct{
    UBaseType_t low;
    UBaseType_t realtime;
    UBaseType_t high;
    UBaseType_t normal;
} priorityValueTypes;

typedef struct{
    int16_t emergencyEventPeriod;
    int16_t dataReadPeriod;
    int16_t shortDelay;
    int16_t maintanceModeLongDelay;
} delayTypes;

static const priorityValueTypes priorityConfig = { .high=10, .low=1, .normal=5, .realtime=15};
static const delayTypes delayConfig = {.maintanceModeLongDelay=10000, .shortDelay=2000, .emergencyEventPeriod=3000, .dataReadPeriod=500};

void sensorReadTask(void * taskParam);
void workerTask(void * taskParam);
void emergencyTask(void * taskParam);
void maintenanceMode(void * taskParam);

static const char* TAG_SENSOR = "[Sensor]";
static const char* TAG_WORKER = "[WORKER]";
static const char* TAG_MAINTENANCE = "[Maintenance]";
static const char* TAG_EVENT = "[EVENT]";

static TaskHandle_t sensorTaskHandle;

void app_main(void) {

        xTaskCreate(sensorReadTask, "Sensor", 2048, NULL, priorityConfig.normal, &sensorTaskHandle);
        xTaskCreate(emergencyTask, "Event", 2048, NULL, priorityConfig.low, NULL);
        xTaskCreate(maintenanceMode, "Maintenance", 2048, NULL, priorityConfig.realtime, NULL);
}


void sensorReadTask(void * taskParam){
    TickType_t handleTime = xTaskGetTickCount();
    while (1) {
        ESP_LOGI(TAG_SENSOR, "Data read...");
        xTaskDelayUntil(&handleTime, pdMS_TO_TICKS(delayConfig.dataReadPeriod));
    }
}

void emergencyTask(void * taskParam){
    while (1) {
    vTaskDelay(pdMS_TO_TICKS(delayConfig.emergencyEventPeriod));
    ESP_LOGI(TAG_EVENT, "Something happend...");
    xTaskCreate(workerTask, "Event", 2048, NULL, priorityConfig.high, NULL);

    }
}

void workerTask(void * taskParam){
    ESP_LOGI(TAG_WORKER, "Processing event...");
    vTaskDelete(NULL);
}

void maintenanceMode(void * taskParam){
    TickType_t handleTime = xTaskGetTickCount();
    while (1) {
        xTaskDelayUntil(&handleTime, pdMS_TO_TICKS(delayConfig.maintanceModeLongDelay));
        vTaskSuspend(sensorTaskHandle);
        ESP_LOGI(TAG_MAINTENANCE, "System paused for 2 seconds.");
        vTaskDelay(pdMS_TO_TICKS(delayConfig.shortDelay));
        vTaskResume(sensorTaskHandle);
        ESP_LOGI(TAG_MAINTENANCE, "System works");
    }
}


