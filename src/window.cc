#include "window.h"
#include "event.h"

Window::Window (int width, int height, const char *title, GLFWmonitor *monitor, GLFWwindow *share) :
    window(glfwCreateWindow(width, height, title, monitor, share)) {

    glfwSetCursorPosCallback(this->window, Event::Event<Event::MouseMove>::trigger);

};
