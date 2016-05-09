#include <memory>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "engine/window.h"
#include "breakout/game.h"

#define WINDOW_FPS 60

int main (int argc, char **argv) {

    if (argc < 2) {
        std::cerr << "You should pass the name of the stage(s) via terminal in order." << std::endl;
        std::cerr << "Example: $ bin/tp1 ../stages/level_00.brk" << std::endl;
        return -1;
    }

    if (!glfwInit()) {
        std::cerr << "ERROR: Could not initialize GLFW" << std::endl;
        return -1;
    }

    Engine::Window window(720, 720, "Trabalho Pratico 1");

    if (window) {

        std::vector<std::string> stages;

        window.makeCurrentContext();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        window.swapBuffers();

        glewInit();

        for (int i = 1; i < argc; ++i) {
            stages.push_back(argv[i]);
        }

        Breakout::Game game(window, stages);

        game.start();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0);

        glShadeModel(GL_SMOOTH);

        while (!window.shouldClose()) {
            int width, height;

            window.getFramebufferSize(width, height);

            glViewport(0, 0, width, height);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(90.0, static_cast<double>(width) / height, 1, 100);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glTranslated(0.0, 0.0, -5.0);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            window.draw();
            window.update();

            window.swapBuffers();
            glfwPollEvents();

            game.update();

            unsigned fps = window.sync(WINDOW_FPS);
            if (fps != WINDOW_FPS) {
                std::cout << fps << " FPS" << std::endl;
            }
        }
    } else {
        std::cerr << "ERROR: Could not initialize window" << std::endl;
    }

    glfwTerminate();

    return 0;

}
