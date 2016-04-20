#ifndef SRC_WINDOW_H_
#define SRC_WINDOW_H_

#include <functional>
#include <map>
#include <unistd.h>
#include <GLFW/glfw3.h>
#include "event.h"
#include "object.h"

#define M_PI 3.141592653589793238462643383279502884L

class Window {

    GLFWwindow *window;
    Object root;
    std::map<unsigned, std::tuple<std::function<bool()>, double, double>> timeouts;
    unsigned tick_counter = 0, timeout_counter = 0;

public:

    static void perspective (double fovy, double aspect, double zNear, double zFar) {
        double xmin, xmax, ymin, ymax;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        ymax = zNear * tan(fovy * M_PI / 360.0);
        ymin = -ymax;
        xmin = ymin * aspect;
        xmax = ymax * aspect;
        glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
    }

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

    unsigned sync (unsigned fps = 60) {
        static double start_time = 0;
        double frame_time = 1.0 / static_cast<double>(fps), now = glfwGetTime();
        if ((start_time + frame_time) > now) {
            usleep((start_time + frame_time - now) * 1000000.0);
        } else {
            fps = static_cast<unsigned>(round(1.0 / (now - start_time)));
        }
        start_time = glfwGetTime();
        return fps;
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

        delta = 1.0 / static_cast<double>(steps);

        return this->setTimeout ([ delta, func ] () -> bool {
            static double progress = 0.0;
            progress = std::min(1.0, progress + delta);
            return func(progress) && progress < 1.0;
        }, total_time * delta);
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
