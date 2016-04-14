# Parametros alteraveis

CXX = c++
CXXFLAGS = -std=c++11 -g -Wall -O3 -lglfw3 -lopengl32 -lgdi32
SRC := main.cc brick.cc background.cc texture.cc
OBJ := $(SRC:%.cc=build/%.o)
DEP := $(SRC:%.cc=deps/%.d)
NAME = tp1

# Fim dos parametros

.DEFAULT: all

ALL := bin/$(NAME)

all default: $(ALL)

$(ALL): $(OBJ)
	$(CXX) -o $(ALL) $(OBJ) $(CXXFLAGS)

build: $(OBJ)
	@:

build/%.o: src/%.cc deps/%.d
	$(CXX) $(CXXFLAGS) -c -o $@ $<

autodeps deps: $(DEP)
	@:

deps/%.d: src/%.cc
	@$(CXX) $(CXXFLAGS) -MM -MT $(@:deps/%.d=build/%.o) -o $@ $<

check test: all
	bin/$(NAME)

.PHONY: clean

clean:
	$(RM) $(OBJ) $(DEP) $(ALL)

ifneq ($(shell (echo $(TYPES) | grep -oP "(all|default|build|check|test)")),)
-include $(DEP)
endif
