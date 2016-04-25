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

    inline Window (int width, int height, const char *title, GLFWmonitor *monitor, GLFWwindow *share) :
        window(glfwCreateWindow(width, height, title, monitor, share)) {
        glfwSetCursorPosCallback(this->window, Event::Event<Event::MouseMove>::trigger);
    };

    void update () {

        double now = glfwGetTime();
        auto timeout = this->timeouts.begin(), timeout_end = this->timeouts.end();

        this->root.update(now, this->tick_counter);

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

        this->tick_counter++;
    }

    inline void addObject (Object *obj) {
        this->root.addChild(obj);
    }

    inline unsigned sync (unsigned fps = 60) {
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

    inline unsigned setTimeout (const std::function<bool()> &func, double interval) {

        double now = glfwGetTime();

        unsigned id = this->timeout_counter;
        this->timeout_counter++;

        this->timeouts[id] = std::forward_as_tuple(func, now + interval, interval);

        return id;
    }

    inline void clearTimeout (unsigned id) {
        this->timeouts.erase(id);
    }

    inline unsigned animate (const std::function<bool(double)> &func, double total_time, unsigned steps = 0) {

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

    inline void draw () const {
        this->root.draw();
    }

    inline void makeCurrentContext () const {
        glfwMakeContextCurrent(this->window);
    }

    inline bool shouldClose () const {
        return glfwWindowShouldClose(this->window);
    }

    inline void swapBuffers () const {
        glfwSwapBuffers(this->window);
    }

    inline void getFramebufferSize (int &width, int &height) const {
        glfwGetFramebufferSize(this->window, &width, &height);
    }

    inline GLFWwindow *get () const {
        return this->window;
    }

    inline operator bool () const {
        return this->window;
    }

    inline unsigned getTick () const {
        return this->tick_counter;
    }

    template <typename EventType>
    inline void event (typename EventType::FunctionType func, const std::string &id = "") {
        Event::Event<EventType>::add(this->window, func, id);
    }

};

#endif
