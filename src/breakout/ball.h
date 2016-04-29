#ifndef SRC_BREAKOUT_BALL_H_
#define SRC_BREAKOUT_BALL_H_

#include <valarray>
#include "../object.h"

class Ball : public Object {

    const Sphere2D *sphere_mesh;
    double max_speed;

public:

    Ball (double _max_speed) : Object(
        { 0.0, 0.0, 4.0 },
        true,
        new Sphere2D({ 0.0, 0.0, 0.0 }, 0.025),
        new Sphere2D({ 0.0, 0.0, 0.0 }, 0.025),
        new BackgroundColor(Color::rgba(255, 255, 255, 0.5))
    ), max_speed(_max_speed) {
        sphere_mesh = static_cast<const Sphere2D *>(this->getMesh());
        this->setSpeed({ 0.008, 0.004, 0.0 });
    }

    void update (double now, unsigned tick) {

        std::valarray<double> position = this->getPosition(), speed = this->getSpeed();

        if (position[0] + this->sphere_mesh->getRadius() >= 1.0) {
            // right side collision
            speed[0] = -speed[0];
            this->setSpeed(speed);
        } else if (position[0] - this->sphere_mesh->getRadius() <= -1.0) {
            speed[0] = -speed[0];
            this->setSpeed(speed);
            // left side collision
        } else if (position[1] + this->sphere_mesh->getRadius() >= 1.0) {
            speed[1] = -speed[1];
            this->setSpeed(speed);
            // top side collision
        } else if (position[1] - this->sphere_mesh->getRadius() <= -1.0) {
            speed[1] = -speed[1];
            this->setSpeed(speed);
            // bottom side collision
        }

        Object::update(now, tick);
    }

    void onCollision (const Object *other) {

        if (other->getType() == "breakout_brick") {

            std::valarray<double> other_position = other->getPosition(), my_position = this->getPosition(), speed = this->getSpeed();
            const Rectangle2D *rect = static_cast<const Rectangle2D *>(other->getMesh());

            if ((my_position[0] >= other_position[0]) && (my_position[0] <= other_position[0] + rect->getWidth())) {
                speed[1] = -speed[1];
            } else {
                speed[0] = -speed[0];
            }

            this->setSpeed(speed);

        } else if (other->getType() == "breakout_paddler") {

        }

    }

    void setSpeed (const std::valarray<double> &_speed) {
        std::valarray<double> speed(_speed);
        double size = std::sqrt((speed * speed).sum());

        if (size > this->max_speed) {
            speed = (speed / size) * this->max_speed;
        }

        Object::setSpeed(speed);
    }

    std::string getType (void) const { return "breakout_ball"; }

};

#endif
