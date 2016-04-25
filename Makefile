# Parametros alteraveis

CXX = c++
CXXFLAGS = -std=c++11 -g -Wall -O3 -Wno-missing-braces
CXXLIBS =
SRC := main.cc object.cc mesh.cc background.cc event.cc color.cc window.cc brick.cc
OBJ := $(SRC:%.cc=build/%.o)
DEP := $(SRC:%.cc=deps/%.d)
NAME = tp1

ifeq ($(OS), Windows_NT)
CXXLIBS := -lglfw3 -lglu32 -lopengl32 -lgdi32 -static-libstdc++ -static-libgcc
else
CXXLIBS := -lglfw3 -lGL -lXrandr -lXext -lX11 -ldl -lXxf86vm -lXinerama -lXcursor -lpthread
endif

# Fim dos parametros

ALL := bin/$(NAME)

all default: $(ALL)

$(ALL): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(ALL) $(OBJ) $(CXXLIBS)

build: $(OBJ)
	@:

build/%.o: src/%.cc deps/%.d
	$(CXX) $(CXXFLAGS) -c -o $@ $< #$(CXXLIBS)

autodeps deps: $(DEP)
	@:

deps/%.d: src/%.cc
	@$(CXX) $(CXXFLAGS) -MM -MT $(@:deps/%.d=build/%.o) -o $@ $<

check test: all
	bin/$(NAME)

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
