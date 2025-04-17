SOURCES_DIR = src
SOURCES = $(wildcard $(SOURCES_DIR)/*.cpp)

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
	g++ $(SOURCES) -O0 -o $(BUILD_DIR)/$(EXECUTABLE) -I$(HEADERS_DIR)


run:
	$(BUILD_DIR)/$(EXECUTABLE)
