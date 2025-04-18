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
	nasm -f elf64 $(SOURCES_DIR)/my_funcs.asm -o $(SOURCES_DIR)/my_funcs.o
	gcc $(C_SOURCES) $(SOURCES_DIR)/my_funcs.o -O0 -o $(BUILD_DIR)/$(EXECUTABLE) -I$(HEADERS_DIR)


run:
	$(BUILD_DIR)/$(EXECUTABLE)
