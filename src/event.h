#ifndef SRC_EVENT_H_
#define SRC_EVENT_H_

#include <unordered_map>
#include <string>
#include <list>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <utility>
#include <functional>
#include <algorithm>
#include <iterator>
#include <GLFW/glfw3.h>

namespace Event {

    template <
        typename FType,
        typename ...FunctionArgs
    > class EventBase {

    public:

        typedef FType FunctionType;

        static void beforeEvents (GLFWwindow *window, FunctionArgs... args) {};
        static void triggerEvent (FunctionType ev, FunctionArgs... args) {
            ev(args...);
        };
        static void afterEvents (GLFWwindow *window, FunctionArgs... args) {};

    };

    class MouseMove : public EventBase<
        std::function<void(GLFWwindow *, double, double, double, double)>,
        double, double
    > {

    static double posx, posy;

    public:

        static std::unordered_map<GLFWwindow *, std::list<std::tuple<std::function<void(GLFWwindow *, double, double, double, double)>, std::string, unsigned>>> trigger_list;

        static void beforeEvents (GLFWwindow *window, double x, double y) {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            MouseMove::posx = x / (width / 2.0) - 1.0;
            MouseMove::posy = y / (height / 2.0) - 1.0;
        }

        static void triggerEvent (GLFWwindow *window, FunctionType func, double x, double y) {
            func(window, x, y, MouseMove::posx, MouseMove::posy);
        }
    };

    template <
        typename EventType
    > class Event {

        typedef typename EventType::FunctionType FunctionType;

        // pthread_mutex_t event_mutex = PTHREAD_MUTEX_INITIALIZER;

    public:

        template <typename ...FunctionArgs>
        static void trigger (GLFWwindow *window, FunctionArgs... args) {

            FunctionType ev;
            std::string id;
            unsigned counter;
            auto func_data = EventType::trigger_list[window].begin();

            // pthread_mutex_lock(&event_mutex);
            EventType::beforeEvents(window, args...);

            while (func_data != EventType::trigger_list[window].end()) {

                // FIXME may bug if triggerEvent remove next event!!! (maybe using queue would fix this)
                std::tie(ev, id, counter) = *func_data;
                auto func_erase = func_data++;

                EventType::triggerEvent(window, ev, args...);
                if (counter != 0) {
                    --counter;
                    if (counter == 0) {
                        EventType::trigger_list[window].erase(func_erase);
                    } else {
                        std::get<2>(*func_erase) = counter;
                    }
                }
            }

            EventType::afterEvents(window, args...);
            // pthread_mutex_unlock(&event_mutex);
        }

        static void add (GLFWwindow *window, const FunctionType &func, const std::string &id = "", unsigned limit = 0) {
            EventType::trigger_list[window].push_back(std::forward_as_tuple(func, id, limit));
        }

        static void add (GLFWwindow *window, const FunctionType &func, unsigned limit = 0) {
            Event<EventType>::add(window, func, "", limit);
        }

        static void erase (GLFWwindow *window, const FunctionType &func) {
            EventType::trigger_list[window].remove_if([ func ] (std::tuple<FunctionType, std::string, unsigned> val) {
                return func == std::get<0>(val);
            });
        }

        static void erase (GLFWwindow *window, const std::string &id) {
            if (id != "") {
                EventType::trigger_list[window].remove_if([ id ] (std::tuple<FunctionType, std::string, unsigned> val) {
                    return id == std::get<1>(val);
                });
            }
        }

    };
};

#endif
