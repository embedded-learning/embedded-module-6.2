# Урок 6.2: чотири задачі FreeRTOS з різними пріоритетами

Система з чотирьох задач із чітким розподілом обов'язків і пріоритетів (Low / Normal / High / Realtime).

| Задача | Пріоритет | Що робить |
|---|---|---|
| Sensor Reader | Normal | Імітує опитування датчика: `[Sensor] Data read...` строго кожні 500 мс через `xTaskDelayUntil` (звичайний `vTaskDelay` заборонено). |
| Event Trigger | Low | Імітує зовнішню подію: спить 3 с через `vTaskDelay`, прокинувшись, динамічно створює Worker Task і знову засинає. |
| Worker Task | High | Створюється лише при події, виводить `[Worker] Processing event...` і завершується через `vTaskDelete(NULL)`; вихід через `return` або `}` заборонено. |
| Maintenance Mode | Realtime | Раз на 10 с ставить Sensor Reader на паузу (`vTaskSuspend`), виводить `[Maintenance] System paused for 2 seconds.`, чекає 2 с і відновлює датчик (`vTaskResume`). |

Що відпрацьовується: пріоритети й витіснення, `xTaskDelayUntil` проти `vTaskDelay`, динамічне створення й самовидалення задачі, керування іншою задачею через `TaskHandle_t`.

Нижче — довідка шаблону. Заготовка для нових проєктів на ESP-IDF. Працює з ESP32-C3 Super Mini, ESP32-S3 і звичайною ESP32.

## Новий проєкт

```bash
cd ~/work/esp32-template
make new-project NAME=traffic-light
```

З'явиться папка `~/work/traffic-light` — копія шаблону зі своїм іменем. Відкрий її в Cursor (**File → Open Folder**) і виконай `make build`.
Сам шаблон не чіпай: редагуй уже створені з нього проєкти.

## Команди

| Команда | Що робить |
|---|---|
| `make build` | зібрати |
| `make flash` | залити в плату |
| `make monitor` | дивитися `printf` з плати (вихід: **Ctrl+]** в англійській розкладці) |
| `make run` | залити й одразу дивитися |
| `make clean` | видалити все зібране |
| `make chip CHIP=esp32` | переключити проєкт на інший чип (`esp32`, `esp32c3` або `esp32s3`) |

У Cursor: **Cmd+Shift+B** — зібрати; **Cmd+Shift+P → Tasks: Run Task → flash and monitor** — залити й дивитися.

## Структура

```
├── main/main.c          ← твоя програма, починається з app_main()
├── sdkconfig.defaults   ← чип за замовчуванням
└── Makefile             ← команди make
```

## Як додати свій модуль

Створи папку `components/button/` з файлами `button.c`, `button.h` і `CMakeLists.txt`:

```cmake
idf_component_register(SRCS "button.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES esp_driver_gpio)
```

- `SRCS` — усі `.c` файли модуля.
- `PRIV_REQUIRES` — які частини ESP-IDF модуль використовує всередині (`esp_driver_gpio` — піни). Якщо збірка пише, що не знаходить заголовок, найчастіше сюди потрібно додати компонент.

Потім у `main.c`: `#include "button.h"`. Більше нічого реєструвати не потрібно: ESP-IDF сам знаходить усі папки в `components/`.

## Підказки в редакторі

Підказки clangd з'являться після першого `make build`: він бере налаштування з папки `build`.

## Де дивитися приклади й документацію

- Приклади ESP-IDF: папка `esp-idf-examples` у дереві проєкту (посилання на `~/esp/esp-idf/examples`), окремим вікном: `make examples`. Початкові: `get-started/blink`, периферія: `peripherals/gpio`.
- Довідник ESP-IDF для ESP32-S3: https://docs.espressif.com/projects/esp-idf/en/v6.0.3/esp32s3/ — API Reference → Peripherals (піни, UART, …), System → FreeRTOS (задачі, паузи), API Guides (статті: Watchdogs, FreeRTOS Overview).
- Заголовки: `~/esp/esp-idf/components/<компонент>/include/`, у Cursor — Cmd+клік по імені функції.
