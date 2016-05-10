#ifndef SRC_BREAKOUT_PADDLER_H_
#define SRC_BREAKOUT_PADDLER_H_

#include <iostream>
#include <memory>
#include "../engine/mesh.h"
#include "../engine/window.h"
#include "../engine/event.h"
#include "../engine/object.h"

namespace Breakout {

    class Paddler : public Engine::Object {

        Engine::Window &window;
        const std::valarray<double> start_position;
        const double max_speed;
        double width, height;
        std::unique_ptr<Engine::Rectangle2D> rect_mesh, rect_collider;
        std::unique_ptr<Engine::BackgroundColor> background_color;
        bool started = false;

    public:

        constexpr static double DefaultWidth (void) { return 0.4; }
        constexpr static double DefaultHeight (void) { return 0.05; }

        inline Paddler (Engine::Window &_window, double _max_speed, const std::valarray<double> &_position) : Object(
            _position,
            true,
            new Engine::Rectangle2D({ Paddler::DefaultWidth() * -0.5, 0.0, 0.0 }, Paddler::DefaultWidth(), Paddler::DefaultHeight()),
            new Engine::Rectangle2D({ Paddler::DefaultWidth() * -0.5, 0.0, 0.0 }, Paddler::DefaultWidth(), Paddler::DefaultHeight()),
            new Engine::BackgroundColor(Engine::Color::rgba(255, 255, 255, 1.0))
        ), window(_window), start_position(_position), max_speed(_max_speed), width(Paddler::DefaultWidth()), height(Paddler::DefaultHeight()) {

            this->rect_mesh.reset(static_cast<Engine::Rectangle2D *>(this->getMesh()));
            this->rect_collider.reset(static_cast<Engine::Rectangle2D *>(this->getCollider()));
            this->background_color.reset(static_cast<Engine::BackgroundColor *>(this->getBackground()));

            this->start();
        }

        inline ~Paddler (void) { this->stop(); }

        inline void start (void) {
            if (!this->started) {

                this->started = true;

                this->window.event<Engine::Event::MouseMove>([ this ] (GLFWwindow *window, double _x, double _y, double posx, double _posy) mutable {
                    std::valarray<double> speed;
                    double size;
                    if (posx > 1.0) {
                        posx = 1.0;
                    } else if (posx <= -1.0) {
                        posx = -1.0;
                    }
                    speed = { posx * 1.2, 0.0, 0.0 };
                    size = Engine::Mesh::norm(speed);
                    if (size > this->max_speed) {
                        speed = Engine::Mesh::resize(speed, size, this->max_speed);
                    }
                    this->setSpeed(speed);
                }, "mousemove.paddler");
            }
        }

        inline void stop (void) {
            this->started = false;
            this->window.eraseEvent<Engine::Event::MouseMove>("mousemove.paddler");
            this->setPosition(this->start_position);
        }

        inline void setWidth (double _width) {

            const std::valarray<double> position = { _width * -0.5, 0.0, 0.0 };

            this->rect_mesh->setWidth(_width);
            this->rect_mesh->setPosition(position);

            this->rect_collider->setWidth(_width);
            this->rect_collider->setPosition(position);
        }

        inline double getWidth (void) const { return this->rect_mesh->getWidth(); }

        void afterUpdate (double now, double delta_time, unsigned tick) {
            std::valarray<double> position = this->getPosition();
            double border = 1.0 - (static_cast<Engine::Rectangle2D *>(this->getMesh())->getWidth() / 2.0);

            if (position[0] <= -border) {
                position[0] = -border;
                this->setSpeed({ 0.0, 0.0, 0.0 });
            } else if (position[0] >= border) {
                position[0] = border;
                this->setSpeed({ 0.0, 0.0, 0.0 });
            }

            this->setPosition(position);
        }

        virtual inline std::string getType () const { return "breakout_paddler"; }

    };

};

#endif
