CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra
LDFLAGS = -lboost_program_options -licuuc -licui18n #-lnlohmann_json

# Detect the operating system
UNAME_S := $(shell uname -s)

# Set include and library paths based on the operating system
ifeq ($(UNAME_S),Linux)
    CXXFLAGS += -I/usr/include
    LDFLAGS += -L/usr/lib
else ifeq ($(UNAME_S),Darwin)
    # Assuming Homebrew installations for macOS
    CXXFLAGS += -I/usr/local/include -I/usr/local/opt/icu4c/include
    LDFLAGS += -L/usr/local/lib -L/usr/local/opt/icu4c/lib
endif

TARGET = chinese_sentence_processor
SRCS = main.cpp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET)
