# Parametros alteraveis

CXX = c++
CXXFLAGS = -std=c++11 -g -Wall -O3 -Wno-missing-braces
CXXLIBS = -lglfw3
SRC := main.cc\
 engine/object.cc engine/mesh.cc engine/background.cc engine/event.cc engine/color.cc engine/window.cc engine/shader.cc\
 breakout/brick.cc breakout/game.cc breakout/stage.cc breakout/ball.cc
STAGES := stages/level_00.brk
OBJ := $(SRC:%.cc=build/%.o)
DEP := $(SRC:%.cc=deps/%.d)
NAME = tp1

ifeq ($(OS), Windows_NT)
CXXLIBS += -lopengl32 -lglew32 -lglu32 -lgdi32 -static-libstdc++ -static-libgcc
else
CXXLIBS += -lGL -lGLEW -lGLU -lXrandr -lXext -lX11 -ldl -lXxf86vm -lXinerama -lXcursor -lpthread
endif

# Fim dos parametros

ALL := bin/$(NAME)

all default: $(ALL)

$(ALL): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(ALL) $(OBJ) $(CXXLIBS)

build: $(OBJ)
	@:

build/%.o: src/%.cc deps/%.d
	$(CXX) $(CXXFLAGS) -c -o $@ $<

autodeps deps: $(DEP)
	@:

deps/%.d: src/%.cc
	@$(CXX) $(CXXFLAGS) -MM -MT $(@:deps/%.d=build/%.o) -o $@ $<

check test: all
	bin/$(NAME) $(STAGES) $(ARGS)

.PHONY: clean

clean:
	$(RM) $(OBJ) $(DEP) $(ALL)

.DEFAULT: all

ifeq ($(MAKECMDGOALS),)
TYPES := all
else
TYPES := $(MAKECMDGOALS)
endif

ifneq ($(shell (echo $(TYPES) | grep -oP "(all|default|build|check|test)")),)
-include $(DEP)
endif
