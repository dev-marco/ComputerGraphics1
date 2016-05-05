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

    class Brick : public Engine::Object {

        Engine::Window &window;
        double width, height;
        unsigned lives;
        bool draw_border;

    public:

        static constexpr double DefaultWidth = 0.19, DefaultHeight = 0.05;

        inline Brick (
            Engine::Window &_window,
            const std::array<double, 3> &_position,
            Engine::Background *_background,
            double _width = Brick::DefaultWidth,
            double _height = Brick::DefaultHeight,
            const std::array<double, 3> &_speed = {0.0, 0.0, 0.0},
            const std::array<double, 3> &_acceleration = {0.0, 0.0, 0.0},
            unsigned _lives = 1,
            bool _draw_border = true
        ) :
            Engine::Object(_position, true, new Engine::Rectangle2D({0.0, 0.0, 0.0}, _width, _height),
            new Engine::Rectangle2D({0.0, 0.0, 0.0}, _width, _height), _background, _speed, _acceleration),
            window(_window),
            width(_width),
            height(_height),
            lives(_lives),
            draw_border(_draw_border) {}

        std::string getType (void) const { return "breakout_brick"; }
        virtual std::string brickType (void) const { return "brick"; }

        inline double getWidth (void) const { return this->width; }
        inline double getheight (void) const { return this->height; }

        inline unsigned getLives (void) const { return this->lives; }
        inline bool isDestructible (void) const { return this->getLives() > 0; }

        inline Engine::Window &getWindow (void) const { return this->window; }

        virtual inline void onChangeLives () {}

        inline void onCollision (const Object *other, const std::valarray<double> &point) {

            if (other->getType() == "breakout_ball" && this->lives > 0) {

                --this->lives;

                if (this->lives == 0) {
                    this->destroy();
                } else {
                    this->onChangeLives();
                }
            }
        }

        inline void beforeDraw (bool only_border) const {
            if (draw_border) {
                Engine::BackgroundColor bg;
                if (this->isDestructible()) {
                    bg = Engine::BackgroundColor(Engine::Color::rgba(255, 255, 255, 0.1 * this->getLives()));
                } else {
                    bg = Engine::BackgroundColor(Engine::Color::rgba(255, 255, 255, 0.4));
                }
                this->getMesh()->draw(this->getPosition(), &bg, true);
            }
        }

    };

    class BonusBrick : public Brick {

        std::function<void(void)> bonus_function;

    public:

        inline BonusBrick (
            Engine::Window &_window,
            const std::array<double, 3> &_position,
            Engine::Background *_background,
            std::function<void(void)> _bonus_function,
            double _width = Brick::DefaultWidth,
            double _height = Brick::DefaultHeight,
            const std::array<double, 3> &_speed = {0.0, 0.0, 0.0},
            const std::array<double, 3> &_acceleration = {0.0, 0.0, 0.0},
            unsigned _lives = 1
        ) :
            Brick(_window, _position, _background, _width, _height, _speed, _acceleration, _lives, true),
            bonus_function(_bonus_function) {}

        void beforeDestroy (void) {

            Brick::beforeDestroy();

            this->bonus_function();

            // TODO display bonus
        }

        std::string brickType (void) const { return "bonus_brick"; }

    };

    class AbstractBrick : public Brick {

        Engine::BackgroundColor *color;

    public:

        inline AbstractBrick (
            Engine::Window &_window,
            const std::array<double, 3> &_position,
            Engine::BackgroundColor *_background,
            double _width = Brick::DefaultWidth,
            double _height = Brick::DefaultHeight,
            const std::array<double, 3> &_speed = {0.0, 0.0, 0.0},
            const std::array<double, 3> &_acceleration = {0.0, 0.0, 0.0},
            unsigned _lives = 1
        ) : Brick(_window, _position, _background, _width, _height, _speed, _acceleration, _lives, false), color(_background) {
            if (this->isDestructible()) {
                this->color->setA(0.25);
            } else {
                this->color->setA(0.5);
            }
        }

        std::string brickType (void) const { return "abstract_brick"; }

    };

}

#endif
