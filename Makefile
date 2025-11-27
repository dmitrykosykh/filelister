# Компилятор и флаги для кросс-компиляции в Windows
CXX = x86_64-w64-mingw32-g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -static
TARGET = file_lister.exe
SOURCES = main.cpp

# Основная цель
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

# Цель для отладочной сборки
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Очистка
clean:
	rm -f $(TARGET)

# Проверка установки компилятора
check-env:
	@which $(CXX) > /dev/null && echo "Компилятор найден: $$(which $(CXX))" || echo "Ошибка: компилятор $(CXX) не найден!"

.PHONY: clean debug check-env