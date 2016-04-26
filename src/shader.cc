#include "shader.h"

void Shader::push (const Shader::Program *shader) {
    Shader::shader_stack.push(shader);
    if (shader && Shader::current_shader != shader) {
        shader->use();
    }
}

void Shader::pop (void) {
    if (!Shader::shader_stack.empty()) {
        Shader::shader_stack.pop();
        if (Shader::shader_stack.empty()) {
            Shader::resetShaders();
        } else {
            const auto &shader = Shader::shader_stack.top();
            if (shader && Shader::current_shader != shader) {
                shader->use();
            }
        }
    }
}

void Shader::resetShaders (void) {
    current_shader = nullptr;
    glUseProgram(0);
}
