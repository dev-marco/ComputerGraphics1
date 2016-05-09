#include "ball.h"

namespace Breakout {

    void Ball::onCollision (const Object *other, const std::valarray<double> &point) {

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
                this->on_destroy(static_cast<const Brick *>(other));
                this->sound_brick.play();
                // deacelerate
                this->setSpeed(speed * 0.9);
            } else {
                this->sound_pop.play();
                // add paddler speed
                this->setSpeed(speed + (other->getSpeed() * 0.8));
            }
        }
    }

}
