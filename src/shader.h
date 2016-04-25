#ifndef SRC_SHADER_H_
#define SRC_SHADER_H_

#include <string>
#include <vector>
#include <iostream>
#include <GL/glew.h>

namespace Shader {

    // TODO create program class

    class Shader {
        GLuint shader, prog;

        static GLuint compile (GLuint type, const std::vector<std::string> &src) {
            unsigned size = src.size();
            GLuint result;
            const char **source = new const char*[size];
            for (unsigned i = 0; i < size; i++) {
                source[i] = src[i].c_str();
            }
            result = Shader::compile(type, source, size);
            delete[] source;
            return result;
        }

        static GLuint compile (GLuint type, const char **src, unsigned size) {
            GLint compiled;
            GLuint shader = glCreateShader(type);
            glShaderSource(shader, size, src, nullptr);
            glCompileShader(shader);
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
            if (compiled) {
                return shader;
            }
            GLint length;
            std::string shader_error;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            shader_error.resize(length);
            glGetShaderInfoLog(shader, length, &length, &shader_error[0]);
            std::cerr << shader_error << std::endl;
            return GL_FALSE;
        }

        void createProgram (void) {
            this->prog = glCreateProgram();
            glAttachShader(this->prog, this->shader);
            glLinkProgram(this->prog);
        }

    public:
        Shader (GLuint type, const char **src, unsigned size) : shader(Shader::compile(type, src, size)) {
            this->createProgram();
        }

        Shader (GLuint type, const std::vector<std::string> &src) : shader(Shader::compile(type, src)) {
            this->createProgram();
        }

        inline ~Shader (void) {
            glDeleteProgram(this->prog);
            glDeleteShader(this->shader);
        }

        inline void operator() (void) const { glUseProgram(this->prog); }

        inline operator GLuint (void) const { return this->prog; }
        inline operator bool (void) const { return this->shader != 0; }
    };

    class Vertex : public Shader {
    public:
        inline Vertex (const std::vector<std::string> &src) : Shader(GL_VERTEX_SHADER, src) {};
        inline Vertex (const char **src, unsigned size) : Shader(GL_VERTEX_SHADER, src, size) {};
    };

    class Fragment : public Shader {
    public:
        inline Fragment (const std::vector<std::string> &src) : Shader(GL_FRAGMENT_SHADER, src) {};
        inline Fragment (const char **src, unsigned size) : Shader(GL_FRAGMENT_SHADER, src, size) {};
    };

    class Geometry : public Shader {
    public:
        inline Geometry (const std::vector<std::string> &src) : Shader(GL_GEOMETRY_SHADER_EXT, src) {};
        inline Geometry (const char **src, unsigned size) : Shader(GL_GEOMETRY_SHADER_EXT, src, size) {};
    };
}

#endif
