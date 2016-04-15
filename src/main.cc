#include <iostream>
#include <GLFW/glfw3.h>
#include "brick.h"

using namespace std;

int main (int argc, const char **argv) {

    GLFWwindow *window;

    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    window = glfwCreateWindow(720, 480, "Trabalho Pratico 1", NULL, NULL);

    if (window) {

        glfwSetCursorPosCallback(window, [](GLFWwindow *window, double x, double y) {
            double V, maxV = 10.0, threshold = 5.0;
        });

        glfwMakeContextCurrent(window);

        while (!glfwWindowShouldClose(window)) {
            float ratio;
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            cout << glfwGetTime() << endl;
            ratio = width / (float) height;
            glViewport(0, 0, width, height);
            glClear(GL_COLOR_BUFFER_BIT);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glBegin(GL_TRIANGLE_FAN);
            glColor3f(1.0, 1.0, 1.0);
            glVertex3f(0.025, 0.01f, 0.f);
            glVertex3f(-0.025, 0.01f, 0.f);
            glVertex3f(-0.025f, -0.01f, 0.f);
            glVertex3f(0.025f, -0.01f, 0.f);
            glEnd();
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    }

    glfwTerminate();

    return 0;

}
