#ifndef SRC_WINDOW_H_
#define SRC_WINDOW_H_

#include <GLFW/glfw3.h>
#include "object.h"

class Window {

    GLFWwindow *window;
    Object root;

public:

    Window (int width, int height, const char *title, GLFWmonitor *monitor, GLFWwindow *share);

    void addObject (Object *obj) {
        root.addChild(obj);
    }

    void update () {
        root.update();
    }

    void draw () {
        root.draw();
    }

    void makeCurrentContext () {
        glfwMakeContextCurrent(this->window);
    }

    bool shouldClose () {
        return glfwWindowShouldClose(this->window);
    }

    void swapBuffers () {
        glfwSwapBuffers(this->window);
    }

    GLFWwindow *get () {
        return this->window;
    }

    operator bool () {
        return this->window;
    }

};

#endif
