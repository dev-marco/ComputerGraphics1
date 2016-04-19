#ifndef SRC_WINDOW_H_
#define SRC_WINDOW_H_

#include <functional>
#include <map>
#include <unistd.h>
#include <GLFW/glfw3.h>
#include "event.h"
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

    unsigned sync (double start_time, unsigned fps = 60) {
        double frame_time = 1.0 / (double) fps, now = glfwGetTime();
        if ((start_time + frame_time) > now) {
            usleep((start_time + frame_time - glfwGetTime()) * 1000000.0);
            return fps;
        }
        frame_time = now - start_time;
        return (1.0 / frame_time);
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

    unsigned animate (const std::function<bool(double)> &func, double total_time, unsigned steps = 0) {

        double delta;

        if (steps == 0) {
            steps = ceil(total_time / 0.01);
        }

        delta = 1.0 / (double) steps;

        return this->setTimeout ([ delta, func ] () -> bool {
            static double progress = 0.0;
            progress = std::min(1.0, progress + delta);
            return func(progress) && progress < 1.0;
        }, total_time / steps);
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

    void getFramebufferSize (int &width, int &height) {
        glfwGetFramebufferSize(this->window, &width, &height);
    }

    template <typename EventType>
    void event (typename EventType::FunctionType func, const std::string &id = "") {
        Event::Event<EventType>::add(this->window, func, id);
    }

    GLFWwindow *get () {
        return this->window;
    }

    operator bool () {
        return this->window;
    }

};

#endif
