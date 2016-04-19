#include <memory>
#include <iostream>
#include <GLFW/glfw3.h>
#include "window.h"
#include "object.h"
#include "brick.h"
#include "event.h"

int main (int argc, const char **argv) {

    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    Window window = Window(720, 720, "Trabalho Pratico 1", NULL, NULL);

    BackgroundColor *rectbg = new BackgroundColor(Color::rgba(100, 0, 255, 0.5));
    BackgroundColor *sphebg = new BackgroundColor(Color(0.0, 0.0, 0.0, 0.8));
    Brick *rect = new Brick({-0.1, -0.04, 0.0}, 0.2, 0.08, rectbg);
    Object *sphe = new Object({ 0.7,   0.7, -0.5}, true, new Sphere2D({0.0, 0.0}, 0.3), sphebg);
    Object *poly = new Object({-0.7,  -0.7, 0.0}, true, new Polygon2D({0.0, 0.0}, 0.3, 5, 0.0), new BackgroundColor(Color::rgba(255, 255, 0, 0.4)));
    window.addObject(rect);
    window.addObject(sphe);
    window.addObject(poly);

    if (window) {

        unsigned id;

        window.makeCurrentContext();

        id = window.setTimeout([ &window, &id ] () mutable {
            std::cout << "1.5 seconds" << std::endl;
            return true;
        }, 1.5);

        window.animate([&sphebg] (double progress) -> bool {
            sphebg->setColor(Color::rgba(progress * 100, progress * 0, progress * 255, 1.0));
            return true;
        }, 10);

        window.event<Event::MouseMove>([](GLFWwindow *window, double x, double y, double posx, double posy) {

        }, "mousemove.paddler");

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        while (!window.shouldClose()) {
            double ratio;
            int width, height;
            double start_time = glfwGetTime();

            window.getFramebufferSize(width, height);

            ratio = width / (double) height;
            glViewport(0, 0, width, height);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(-ratio, ratio, -1.0, 1.0, 1.0, -1.0);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            static int t_last = 0;
            int t = glfwGetTime() * 10;
            if (t != t_last) {
                t_last = t;
                if (Object::isValid(rect)) {
                    rect->destroy();
                } else {
                    rect = new Brick({-0.1, -0.04, 0.0}, 0.2, 0.08, new BackgroundColor(Color::rgba(100, 0, 255, 0.5)));
                    window.addObject(rect);
                }
            }

            window.draw();
            window.update();

            window.swapBuffers();
            glfwPollEvents();

            std::cout << window.sync(start_time, 60) << " FPS" << std::endl;
        }

    }

    glfwTerminate();

    return 0;

}
