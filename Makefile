CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g
SRCDIR = src
SOURCES = main.cpp
TARGET = cpu_emulator

# Find all header files (for dependency tracking)
HEADERS = $(shell find $(SRCDIR) -name "*.hpp")

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

# Example program runs
run-hello: $(TARGET)
	./$(TARGET) programs/hello.asm run

run-fibonacci: $(TARGET)
	./$(TARGET) programs/fibonacci.asm run

run-timer: $(TARGET)
	./$(TARGET) programs/timer.asm run

debug: CXXFLAGS += -DDEBUG -g3
debug: $(TARGET)

