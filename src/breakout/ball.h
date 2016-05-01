#ifndef SRC_BREAKOUT_BALL_H_
#define SRC_BREAKOUT_BALL_H_

#include <valarray>
#include <random>
#include <cmath>
#include "../engine/object.h"

class Ball : public Object {

    const Sphere2D *sphere_mesh;
    double max_speed, min_speed;

public:

    inline static std::valarray<double> resizeVector (const std::valarray<double> &vector, double vector_size, double size) {
        return vector * (size / vector_size);
    }

    inline static std::valarray<double> resizeVector (const std::valarray<double> &vector, double size) {
        return Ball::resizeVector(vector, std::sqrt((vector * vector).sum()), size);
    }

    Ball (double _max_speed, double _min_speed) : Object(
        { 0.0, 0.0, 4.0 },
        true,
        new Sphere2D({ 0.0, 0.0, 0.0 }, 0.025),
        new Sphere2D({ 0.0, 0.0, 0.0 }, 0.025),
        new BackgroundColor(Color::rgba(255, 255, 255, 0.5))
    ), max_speed(_max_speed), min_speed(_min_speed) {

        std::default_random_engine gen(glfwGetTime() * 1000000);
        std::uniform_real_distribution<double> speed(1.0, 2.0);

        sphere_mesh = static_cast<const Sphere2D *>(this->getMesh());
        this->setSpeed(Ball::resizeVector({ speed(gen), speed(gen), 0.0 }, this->min_speed));
    }

    inline void afterUpdate (double now, unsigned tick) {

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

    void onCollision (const Object *other) {

        bool is_brick = other->getType() == "breakout_brick", is_paddler = other->getType() == "breakout_paddler";

        if (is_brick || is_paddler) {

            std::valarray<double> other_position = other->getPosition(), my_position = this->getPosition(), speed = this->getSpeed();
            const Rectangle2D *rect = static_cast<const Rectangle2D *>(other->getCollider());

            other_position += rect->getPosition();

            if ((my_position[0] >= other_position[0]) && (my_position[0] <= other_position[0] + rect->getWidth())) {
                if (my_position[1] >= other_position[1]) {
                    speed[1] = std::abs(speed[1]);
                } else {
                    speed[1] = -std::abs(speed[1]);
                }
            } else {
                if (my_position[0] >= other_position[0]) {
                    speed[0] = std::abs(speed[0]);
                } else {
                    speed[0] = -std::abs(speed[0]);
                }
            }

            if (is_brick) {
                // deacelerate
                this->setSpeed(speed * 0.99);
            } else {
                // add paddler speed
                this->setSpeed(speed + (other->getSpeed() * 0.5));
            }

        }

    }

    void setSpeed (const std::valarray<double> &_speed) {
        std::valarray<double> speed(_speed);
        double size = std::sqrt((speed * speed).sum());
        constexpr double min_dir_speed = 0.001;

        if (speed[0] >= -min_dir_speed && speed[0] <= min_dir_speed) {
            if (speed[0] < 0) {
                speed[0] = -min_dir_speed;
            } else {
                speed[0] = min_dir_speed;
            }
        }

        if (speed[1] >= -min_dir_speed && speed[1] <= min_dir_speed) {
            if (speed[1] < 0) {
                speed[1] = -min_dir_speed;
            } else {
                speed[1] = min_dir_speed;
            }
        }

        if (size > this->max_speed) {
            speed = Ball::resizeVector(_speed, size, this->max_speed);
        } else if (size < this->min_speed) {
            speed = Ball::resizeVector(_speed, size, this->min_speed);
        }

        Object::setSpeed(speed);
    }

    inline std::string getType (void) const { return "breakout_ball"; }

};

#endif
