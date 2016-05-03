#ifndef SRC_BREAKOUT_BALL_H_
#define SRC_BREAKOUT_BALL_H_

#include <valarray>
#include <random>
#include <chrono>
#include <cmath>
#include "brick.h"
#include "../engine/object.h"

namespace Breakout {

    class Ball : public Object {

        const Sphere2D *sphere_mesh;
        double max_speed, min_speed;

    public:

        Ball (double _max_speed, double _min_speed) : Object(
            { 0.0, 0.0, 4.0 },
            true,
            new Sphere2D({ 0.0, 0.0, 0.0 }, 0.025),
            new Sphere2D({ 0.0, 0.0, 0.0 }, 0.025),
            new BackgroundColor(Color::rgba(255, 255, 255, 0.5))
        ), max_speed(_max_speed), min_speed(_min_speed) {

            std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
            std::uniform_real_distribution<double> speed(-1.0, 1.0);

            this->sphere_mesh = static_cast<Sphere2D *>(this->getMesh());
            this->setSpeed(Object::resizeVector({ speed(gen), speed(gen), 0.0 }, this->min_speed));
        }

        inline void afterUpdate (double now, double delta_time, unsigned tick) {

            std::valarray<double> position = this->getPosition(), speed = this->getSpeed();
            double radius = this->sphere_mesh->getRadius();

            if ((position[0] + radius) >= 1.0) {
                speed[0] = -std::abs(speed[0]);
            } else if ((position[0] - radius) <= -1.0) {
                speed[0] = std::abs(speed[0]);
            }

            if ((position[1] + radius) >= 1.0) {
                speed[1] = -std::abs(speed[1]);
            } else if ((position[1] - radius) <= -1.0) {
                speed[1] = std::abs(speed[1]);
                // TODO Game Over
            }

            this->setSpeed(speed);
        }

        void onCollision (const Object *other, const std::valarray<double> &point) {

            bool is_brick = other->getType() == "breakout_brick", is_paddler = other->getType() == "breakout_paddler";

            if ((is_brick && static_cast<const Brick *>(other)->brickType() != "abstract_brick") || is_paddler) {

                std::valarray<double>
                    speed = this->getSpeed(),
                    diff = point - this->getPosition(),
                    abs_diff = std::abs(diff);

                if (abs_diff[0] > abs_diff[1]) {
                    if (diff[0] < 0) {
                        speed[0] = std::abs(speed[0]);
                    } else {
                        speed[0] = -std::abs(speed[0]);
                    }
                } else {
                    if (diff[1] < 0) {
                        speed[1] = std::abs(speed[1]);
                    } else {
                        speed[1] = -std::abs(speed[1]);
                    }
                }

                if (is_brick) {
                    // deacelerate
                    this->setSpeed(speed * 0.9);
                } else {
                    // add paddler speed
                    this->setSpeed(speed + (other->getSpeed() * 0.4));
                }
            }
        }

        void setSpeed (const std::valarray<double> &_speed, double update = 0.0) {
            std::valarray<double> speed(_speed);
            double size;
            constexpr double min_dir_speed = 0.001;

            if (speed[0] == 0.0) {
                speed[0] = min_dir_speed;
            }

            if (speed[1] == 0.0) {
                speed[1] = min_dir_speed;
            }

            if (std::abs(speed[1] / speed[0]) < 0.5) {
                if (speed[1] < 0) {
                    speed[1] = std::abs(speed[0]) * -0.5;
                } else {
                    speed[1] = std::abs(speed[0]) * 0.5;
                }
            }

            size = Object::vectorSize(speed);

            if (size > this->max_speed) {
                speed = Object::resizeVector(speed, size, this->max_speed);
            } else if (size < this->min_speed) {
                if (size == 0.0) {
                    speed = { 1.0, 1.0, 0.0 };
                    size = std::sqrt(2.0);
                }
                speed = Object::resizeVector(speed, size, this->min_speed);
            }

            this->setPosition(this->getPosition() + speed * update);

            Object::setSpeed(speed);
        }

        inline std::string getType (void) const { return "breakout_ball"; }

    };
}

#endif
