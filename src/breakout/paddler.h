#ifndef SRC_BREAKOUT_PADDLER_H_
#define SRC_BREAKOUT_PADDLER_H_

#include <iostream>
#include "../engine/mesh.h"
#include "../engine/window.h"
#include "../engine/event.h"
#include "../engine/object.h"

namespace Breakout {

    class Paddler : public Engine::Object {

    public:
        inline Paddler (Engine::Window &window, double max_speed) : Object(
            { 0.0, -0.95, 4.0 },
            true,
            new Engine::Rectangle2D({ -0.2, 0.0, 0.0 }, 0.4, 0.05),
            new Engine::Rectangle2D({ -0.2, 0.0, 0.0 }, 0.4, 0.05),
            new Engine::BackgroundColor(Engine::Color::rgba(255, 255, 255, 1.0))
        ) {
            window.eraseEvent<Engine::Event::MouseMove>("mousemove.paddler");
            window.event<Engine::Event::MouseMove>([this, max_speed] (GLFWwindow *window, double _x, double _y, double posx, double _posy) mutable {
                std::valarray<double> speed;
                double size;
                if (posx > 1.0) {
                    posx = 1.0;
                } else if (posx <= -1.0) {
                    posx = -1.0;
                }
                speed = { posx * 1.2, 0.0, 0.0 };
                size = Object::vectorSize(speed);
                if (size > max_speed) {
                    speed = Object::resizeVector(speed, size, max_speed);
                }
                this->setSpeed(speed);
            }, "mousemove.paddler");
        }

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
