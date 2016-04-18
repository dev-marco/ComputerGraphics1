#include <memory>
#include <iostream>
#include <GLFW/glfw3.h>
#include "window.h"
#include "object.h"
#include "event.h"

int main (int argc, const char **argv) {

    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    Window window = Window(720, 720, "Trabalho Pratico 1", NULL, NULL);

    Object rect = Object({-0.1, -0.04, 0.0}, true, new Rectangle2D({0.0, 0.0}, 0.2, 0.08), new BackgroundColor(Color::rgba(100, 0, 255, 0.5)));
    Object sphe = Object({ 0.7,   0.7, 0.0}, true, new Sphere2D({0.0, 0.0}, 0.3), new BackgroundColor(Color::rgba(100, 255, 10, 0.8)));
    Object poly = Object({-0.7,  -0.7, 0.0}, true, new Polygon2D({0.0, 0.0}, 0.3, 5, 0.0), new BackgroundColor(Color::rgba(255, 255, 0, 0.4)));
    window.addObject(rect);
    window.addObject(sphe);
    window.addObject(poly);

    if (window) {

        window.makeCurrentContext();

        Event::Event<Event::MouseMove>::add(window.get(), [](GLFWwindow *window, double x, double y, double posx, double posy) {
            std::cout << x << " " << y << " - " << posx << " " << posy << std::endl;
        }, "mousemove.paddler");

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        while (!window.shouldClose()) {
            double ratio;
            int width, height;

            glfwGetFramebufferSize(window.get(), &width, &height);
            ratio = width / (double) height;
            glViewport(0, 0, width, height);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(-ratio, ratio, -1.0, 1.0, 1.0, -1.0);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            window.draw();
            window.update();

            window.swapBuffers();
            glfwPollEvents();
        }

    }

    glfwTerminate();

    return 0;

}
