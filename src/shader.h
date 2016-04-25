#include <string>
#include <GLFW/glfw3.h>
#include <GL/glew.h>

namespace Shader {

    class Shader {
        GLuint shader, prog;

        GLuint compile(GLuint type, const std::string &src) {
            GLuint shader = glCreateShader(type);
            const char *source = src.c_str();
            GLint compiled;
            glShaderSource(shader, 1, &source, nullptr);
            glCompileShader(shader);
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
            if (!compiled) {
                return 0;
            }
            return shader;
        }
    public:
        Shader(GLuint type, const std::string &src) {
            shader = compile(type, src);
            prog = glCreateProgram();
            glAttachShader(prog, shader);
            glLinkProgram(prog);
        }

        inline ~Shader() {
            glDeleteProgram(prog);
            glDeleteShader(shader);
        }

        inline operator GLuint() const { return this->prog; }

        inline void operator() (void) const { glUseProgram(prog); }
    };

    class Vertex : public Shader {
    public:
        inline Vertex (const std::string &src) : Shader(GL_VERTEX_SHADER, src) {};
    };

    class Fragment : public Shader {
    public:
        inline Fragment (const std::string &src) : Shader(GL_FRAGMENT_SHADER, src) {};
    };
}
