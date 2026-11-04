# Обгортка над idf.py, щоб працювати звичними командами make.
# Плата шукається сама; якщо підключено кілька — вкажи явно: make flash PORT=/dev/cu.usbmodem11101
PORT ?= $(firstword $(wildcard /dev/cu.usbmodem* /dev/cu.usbserial* /dev/cu.wchusbserial* /dev/cu.SLAB_USBtoUART*))
IDF = export PATH="$$(brew --prefix python@3.12)/libexec/bin:$$PATH" && . $$HOME/esp/esp-idf/export.sh >/dev/null && idf.py

build:
	$(IDF) build

flash:
	$(IDF) -p $(PORT) flash

monitor:
	$(IDF) -p $(PORT) monitor

run:
	$(IDF) -p $(PORT) flash monitor

clean:
	$(IDF) fullclean

# Змінити чип: make chip CHIP=esp32   (або esp32c3, esp32s3)
chip:
	$(IDF) set-target $(CHIP)

# Новий проєкт із цієї папки: make new-project NAME=traffic-light  →  з'явиться ../traffic-light
new-project:
	@test -n "$(NAME)" || { echo "Вкажи ім'я: make new-project NAME=my-project"; exit 1; }
	@test ! -e ../$(NAME) || { echo "Папка ../$(NAME) уже існує"; exit 1; }
	rsync -a --exclude build --exclude sdkconfig --exclude sdkconfig.old \
		--exclude managed_components --exclude dependencies.lock ./ ../$(NAME)/
	sed -i '' 's/^project(.*)/project($(NAME))/' ../$(NAME)/CMakeLists.txt
	@echo "Готово: ../$(NAME). Відкрий цю папку в Cursor і виконай make build"

.PHONY: build flash monitor run clean chip new-project

# Открыть примеры ESP-IDF в Cursor: make examples
examples:
	open -a Cursor $$HOME/esp/esp-idf/examples

.PHONY: examples
