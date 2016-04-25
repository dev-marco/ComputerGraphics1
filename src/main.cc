#include <memory>
#include <iostream>
#include <gl/glu.h>
#include <GLFW/glfw3.h>
#include "window.h"
#include "object.h"
#include "brick.h"
#include "event.h"
#include "shaders/cgwg_crt.h"

int main (int argc, const char **argv) {

    if (!glfwInit()) {
        std::cerr << "ERROR: Could not initialize GLFW" << std::endl;
        return -1;
    }

    std::cout << Shaders::cgwg_CRT << std::endl;
    return 0;

    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    Window window = Window(720, 720, "Trabalho Pratico 1", NULL, NULL);

    BackgroundColor *rectbg = new BackgroundColor(Color::rgba(100, 0, 255, 0.5));
    BackgroundColor *sphebg = new BackgroundColor(Color(0.0, 0.0, 0.0, 0.8));
    Brick *rect = new Brick({-0.1, -0.04, 0.0}, 0.2, 0.08, rectbg);
    Object *sphe = new Object({ 0.7,   0.7, 0.0}, true, new Sphere2D({0.0, 0.0, 0.0}, 0.3), sphebg);
    Object *poly = new Object({-0.7,  -0.7, 0.0}, true, new Polygon2D({0.0, 0.0, 0.0}, 0.3, 5, Polygon2D::PI / -2), new BackgroundColor(Color::rgba(255, 255, 0, 0.4)));
    window.addObject(rect);
    window.addObject(sphe);
    window.addObject(poly);

    if (window) {

        window.makeCurrentContext();

        double last = glfwGetTime();
        window.setTimeout([ &window, &last ] () mutable {
            double now = glfwGetTime();
            std::cout << (now - last) << " seconds" << std::endl;
            last = now;
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

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glShadeModel(GL_SMOOTH);

        while (!window.shouldClose()) {
            int width, height;

            window.getFramebufferSize(width, height);

            glViewport(0, 0, width, height);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(90.0, static_cast<double>(width) / height, 1, 100);
            // gluLookAt(0, 0, -1, 0, 0, 0, 0, 0, 0);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glTranslated(0.0, 0.0, -5.0);
            glRotated(window.getTick() % 360, 0.0, 1.0, 0);
            glRotated(window.getTick() % 360, 1.0, 0.0, 0);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

            window.sync(60);
        }
    } else {
        std::cerr << "ERROR: Could not initialize window" << std::endl;
    }

    glfwTerminate();

    return 0;

}
