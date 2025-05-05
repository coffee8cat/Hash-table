SOURCES_DIR = src
C_SOURCES = $(wildcard $(SOURCES_DIR)/*.cpp)
ASM_SOURCES = $(wildcard $(SOURCES_DIR)/*.asm)

BUILD_DIR = build
OBJ_DIR = obj
OBJECTS = $(wildcard $(OBJ_DIR)/*.o)

HEADERS_DIR = headers

EXECUTABLE = hash-table
EXECUTABLE_PROD = hash-table-prod

all:
	make compile
	make run

clean_data:
	rm data/*.png
	rm data/*.dot

profile:
	perf record ./$(BUILD_DIR)/$(EXECUTABLE)

hotspot:
	hotspot perf.data

compile:
	nasm -f elf64 $(SOURCES_DIR)/asm_funcs.asm -o $(SOURCES_DIR)/asm_funcs.o
	gcc $(C_SOURCES) $(SOURCES_DIR)/asm_funcs.o -g -O3 -o $(BUILD_DIR)/$(EXECUTABLE) -I$(HEADERS_DIR) -mavx2


compile_prod:
	nasm -f elf64 $(SOURCES_DIR)/asm_funcs.asm -o $(SOURCES_DIR)/asm_funcs.o
	gcc $(C_SOURCES) $(SOURCES_DIR)/asm_funcs.o -g -O3 -DNDEBUG -o $(BUILD_DIR)/$(EXECUTABLE_PROD) -I$(HEADERS_DIR) -mavx2

run:
	$(BUILD_DIR)/$(EXECUTABLE)

run_prod:
	$(BUILD_DIR)/$(EXECUTABLE_PROD)
