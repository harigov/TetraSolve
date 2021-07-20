CC=g++
CXXOPTS=-c `wx-config --cxxflags` -I .
LINK=g++
LINKOPTS=`wx-config --libs ` -L foldbar/foldbar.so -L symex/symex.so -L tetgen/tetgen.so
SRC_DIR=src
BUILD_DIR=build
SRC_FILES=$(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES=$(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))


TetraGUI:$(OBJ_FILES)
	$(LINK) $(LINKOPTS) $(OBJ_FILES) -o $(BUILD_DIR)/TetraGUI

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CXXOPTS) -c -o $@ $<

clean:
	rm -rf $(OBJ_FILES) TetraGUI
