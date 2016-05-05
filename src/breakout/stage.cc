#include "stage.h"

namespace Breakout {

    Engine::Shader::Program Stage::shader_wave;
    std::function<double()> Stage::wave_function;

};
