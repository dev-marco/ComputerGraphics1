#ifndef SRC_BREAKOUT_BALL_H_
#define SRC_BREAKOUT_BALL_H_

#include <valarray>
#include <memory>
#include <random>
#include <chrono>
#include <cmath>
#include "brick.h"
#include "../engine/object.h"
#include "../engine/audio.h"

namespace Breakout {

    class Ball : public Engine::Object {

        std::valarray<double> start_position;
        std::unique_ptr<Engine::Sphere2D> sphere_mesh, sphere_collider;
        std::unique_ptr<Engine::BackgroundColor> background_color;
        double max_speed, min_speed;
        Engine::Audio::Sound
            sound_pop = Engine::Audio::Sound("audio/effects/ball_pop.ogg"),
            sound_brick = Engine::Audio::Sound("audio/effects/ball_brick.ogg");

        std::function<void(void)> touch_bottom;
        std::function<void(const Brick *)> on_destroy;

    public:

        static constexpr double DefaultRadius (void) {
            return 0.025;
        }

        Ball (
            double _max_speed,
            double _min_speed,
            std::function<void(void)> _touch_bottom,
            std::function<void(const Brick *)> _on_destroy,
            const std::valarray<double> &_start_position = { 0.0, 0.0, 0.0 }
        ) : Engine::Object(
            _start_position,
            true,
            new Engine::Sphere2D({ 0.0, 0.0, 0.0 }, Ball::DefaultRadius()),
            new Engine::Sphere2D({ 0.0, 0.0, 0.0 }, Ball::DefaultRadius()),
            new Engine::BackgroundColor(Engine::Color::rgba(255, 255, 255, 0.5))
        ), start_position(_start_position), max_speed(_max_speed), min_speed(_min_speed), touch_bottom(_touch_bottom), on_destroy(_on_destroy) {

            this->sphere_mesh.reset(static_cast<Engine::Sphere2D *>(this->getMesh()));
            this->sphere_collider.reset(static_cast<Engine::Sphere2D *>(this->getCollider()));
            this->background_color.reset(static_cast<Engine::BackgroundColor *>(this->getBackground()));
        }

        inline void start (void) {

            std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
            std::uniform_real_distribution<double> speed_x(-1.0, 1.0), speed_y(0.0, 1.0);
            this->setSpeed(Engine::Mesh::resize({ speed_x(gen), speed_y(gen), 0.0 }, this->min_speed));

        }

        inline void afterUpdate (double now, double delta_time, unsigned tick) {

            std::valarray<double> position = this->getPosition(), speed = this->getSpeed();
            double radius = this->sphere_mesh->getRadius();

            if ((position[1] - radius) <= -1.0) {
                this->stop();
                this->touch_bottom();
            } else {
                if ((position[1] + radius) >= 1.0) {
                    speed[1] = -std::abs(speed[1]);
                }

                if ((position[0] + radius) >= 1.0) {
                    speed[0] = -std::abs(speed[0]);
                } else if ((position[0] - radius) <= -1.0) {
                    speed[0] = std::abs(speed[0]);
                }

                if ((this->getSpeed() != speed).max()) {
                    this->sound_pop.play();
                    this->setSpeed(speed);
                }
            }
        }

        inline void setRadius (double _radius) {
            this->sphere_mesh->setRadius(_radius);
            this->sphere_collider->setRadius(_radius);
        }

        inline double getRadius (void) const {
            return this->sphere_mesh->getRadius();
        }

        void onCollision(const Object *other, const std::valarray<double> &point);

        void stop (void) {
            this->setSpeed({ 0.0, 0.0, 0.0 });
            this->setPosition(this->start_position);
        }

        void setSpeed (const std::valarray<double> &_speed, double update = 0.0) {
            std::valarray<double> speed(_speed);
            double size = Engine::Mesh::norm2(speed);

            if (size != 0.0) {

                constexpr double min_dir_speed = 0.001;

                if (speed[0] == 0.0) {
                    speed[0] = min_dir_speed;
                }

                if (std::abs(speed[1] / speed[0]) < 0.5) {
                    if (speed[1] < 0) {
                        speed[1] = std::abs(speed[0]) * -0.5;
                    } else {
                        speed[1] = std::abs(speed[0]) * 0.5;
                    }
                }

                size = Engine::Mesh::norm(speed);

                if (size > this->max_speed) {
                    speed = Engine::Mesh::resize(speed, size, this->max_speed);
                } else if (size < this->min_speed) {
                    if (size != 0.0) {
                        speed = Engine::Mesh::resize(speed, size, this->min_speed);
                    }
                }

                this->setPosition(this->getPosition() + speed * update);
            }

            Object::setSpeed(speed);
        }

        inline std::string getType (void) const { return "breakout_ball"; }

    };
}

#endif
