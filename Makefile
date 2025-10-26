# to compile and run in one command type:
# make run

#define which compiler to use
CXX					:= g++
OUTPUT				:= sfmlgame
OS					:= $(shell uname)
SRC_DIR				:= ./src
IMGUI_DIR			:= C:/Programming_Libraries/CPP_Libraries/imgui
IMGUI_SFML_DIR 		:= C:/Programming_Libraries/CPP_Libraries/imgui-sfml

# imgui sources used
IMGUI_SRC := \
	$(IMGUI_DIR)/imgui.cpp \
	$(IMGUI_DIR)/imgui_draw.cpp \
	$(IMGUI_DIR)/imgui_tables.cpp \
	$(IMGUI_DIR)/imgui_widgets.cpp \
	$(IMGUI_DIR)/imgui_demo.cpp \
	$(IMGUI_SFML_DIR)/imgui-SFML.cpp


# linux compiler / linker flags
ifeq ($(OS), Linux)
	CXX_FLAGS	:= -O3 -std=c++20 -Wno-unused-result -Wno-deprecated-declarations
	INCLUDES	:= -I$(SRC_DIR) -I$(SRC_DIR)/imgui
	LDFLAGS	:= -O3 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lGL
endif

# mac osx compiler / linker flags
ifeq ($(OS), Darwin)
	SFML_DIR	:= null
	CXX_FLAGS	:= -O3 -std=c++20 -Wno-unused-result -Wno-deprecated-declarations
	INCLUDES	:= -I$(SRC_DIR) -I$(SRC_DIR)/imgui
	LDFLAGS	:= -O3 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -L$(SFML_DIR)
endif

# windows compiler / linker flags
ifneq (,$(findstring NT,$(OS)))
	SFML_DIR			:= C:/Programming_Libraries/CPP_Libraries/SFML-3.0.2
	CXX_FLAGS			:= -O3 -std=c++20 -Wno-unused-result -Wno-deprecated-declarations
	INCLUDES 			:= -I$(SRC_DIR) -I$(SRC_DIR)/imgui -I$(SFML_DIR)/include -I$(IMGUI_DIR) -I$(IMGUI_SFML_DIR)
	LDFLAGS 			:= -O3 -L$(SFML_DIR)/lib -LC:/msys64/mingw64/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lopengl32

endif

# the source files for the ecs game engine
SRC_FILES	:= $(wildcard $(SRC_DIR)/*.cpp $(IMGUI_SRC))
OBJ_FILES	:= $(SRC_FILES:.cpp=.o)

# Include dependency files
DEP_FILES	:= $(OBJ_FILES:.o=.d)
-include $(DEP_FILES)

# all of these targets will be made if you just type make
all: $(OUTPUT)

# define the main executable requirements / command
$(OUTPUT): $(OBJ_FILES) Makefile
	$(CXX) $(OBJ_FILES) $(LDFLAGS) -o ./bin/$@

# specifies how the object files are compiled from cpp files
%.o: %.cpp
	$(CXX) -MMD -MP -c $(CXX_FLAGS) $(INCLUDES) $< -o $@

# typing 'make clean' will remove all intermediate build files
clean:
	rm -f $(OBJ_FILES) $(DEP_FILES) ./bin/$(OUTPUT)

# typing 'make run' will compile and run the program
run: $(OUTPUT)
	cd bin && PATH="$(SFML_DIR)/bin:$$PATH" ./$(OUTPUT) && cd ..
