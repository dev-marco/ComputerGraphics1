#ifndef SRC_WINDOW_H_
#define SRC_WINDOW_H_

#include <functional>
#include <map>
#include <GLFW/glfw3.h>
#include "object.h"

class Window {

    GLFWwindow *window;
    Object root;
    std::map<unsigned, std::tuple<std::function<bool()>, double, double>> timeouts;
    unsigned tick_counter = 0, timeout_counter = 0;

public:

    Window (int width, int height, const char *title, GLFWmonitor *monitor, GLFWwindow *share);

    void addObject (Object *obj) {
        root.addChild(obj);
    }

    void update () {

        double now = glfwGetTime();
        auto timeout = this->timeouts.begin(), timeout_end = this->timeouts.end();

        root.update(now, tick_counter);

        while (timeout != timeout_end) {

            auto next = std::next(timeout, 1);

            if (std::get<1>(timeout->second) <= now) {
                if (!(std::get<0>(timeout->second)())) {
                    this->timeouts.erase(timeout);
                } else {
                    std::get<1>(timeout->second) += std::get<2>(timeout->second);
                }
            }

            timeout = next;
        }

        tick_counter++;
    }

    unsigned setTimeout (const std::function<bool()> &func, double interval) {

        double now = glfwGetTime();

        unsigned id = timeout_counter;
        timeout_counter++;

        this->timeouts[id] = std::forward_as_tuple(func, now + interval, interval);

        return id;
    }

    void clearTimeout (unsigned id) {
        this->timeouts.erase(id);
    }

    unsigned animate (const std::function<bool(double)> &func, double total_time, unsigned steps) {

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
