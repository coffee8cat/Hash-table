SOURCES_DIR = src
C_SOURCES = $(wildcard $(SOURCES_DIR)/*.cpp)
ASM_SOURCES = $(wildcard $(SOURCES_DIR)/*.asm)

BUILD_DIR = build
OBJ_DIR = obj
OBJECTS = $(wildcard $(OBJ_DIR)/*.o)

HEADERS_DIR = headers

EXECUTABLE = hash-table

all:
	make compile
	make run

clean_data:
	rm data/*.png
	rm data/*.dot

compile:
	gcc $(C_SOURCES) -O2 -o $(BUILD_DIR)/$(EXECUTABLE) -I$(HEADERS_DIR) -mavx2


run:
	$(BUILD_DIR)/$(EXECUTABLE)
