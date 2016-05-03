#ifndef SRC_BREAKOUT_BRICK_H_
#define SRC_BREAKOUT_BRICK_H_

#include <memory>
#include <array>
#include <valarray>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../engine/mesh.h"
#include "../engine/object.h"
#include "../engine/window.h"

namespace Breakout {

    class Brick : public Object {

        Window &window;
        double width, height;
        unsigned lives;

    public:

        static constexpr double DefaultWidth = 0.19, DefaultHeight = 0.05;

        inline Brick (
            Window &_window,
            const std::array<double, 3> &_position,
            Background *_background,
            double _width = Brick::DefaultWidth,
            double _height = Brick::DefaultHeight,
            const std::array<double, 3> &_speed = {0.0, 0.0, 0.0},
            const std::array<double, 3> &_acceleration = {0.0, 0.0, 0.0},
            unsigned _lives = 1
        ) : Object(_position, true, new Rectangle2D({0.0, 0.0, 0.0}, _width, _height), new Rectangle2D({0.0, 0.0, 0.0}, _width, _height), _background, _speed, _acceleration), window(_window), width(_width), height(_height), lives(_lives) {}

        std::string getType (void) const { return "breakout_brick"; }

        inline double getWidth (void) const { return this->width; }
        inline double getheight (void) const { return this->height; }

        inline bool isDestructible (void) const { return this->lives > 0; }

        inline void onCollision (const Object *other, const std::valarray<double> &point) {

            if (other->getType() == "breakout_ball" && this->lives > 0) {

                --this->lives;

                if (this->lives == 0) {
                    this->destroy();
                }
            }
        }

        inline Window &getWindow (void) const { return this->window; }

    };

    class BonusBrick : public Brick {

    public:

        inline BonusBrick (
            Window &_window,
            const std::array<double, 3> &_position,
            Background *_background,
            double _width = Brick::DefaultWidth,
            double _height = Brick::DefaultHeight,
            const std::array<double, 3> &_speed = {0.0, 0.0, 0.0},
            const std::array<double, 3> &_acceleration = {0.0, 0.0, 0.0},
            unsigned _lives = 1
        ) : Brick(_window, _position, _background, _width, _height, _speed, _acceleration, _lives) {}

        void beforeDestroy (void) {

            Brick::beforeDestroy();

            // TODO display bonus
        }

    };

    class TransitionBrick : public Brick {



    };

}

#endif
