#include "stage.h"

namespace Breakout {

    Engine::Shader::Program Stage::shader_wave_rotate;
    double Stage::value_wave = 0.0, Stage::value_rotate = 0.0;

    void Stage::activateBonusWave (const Stage::BonusType type) {

        constexpr double
            max_time = 20.0,
            max_value = 5.0,
            mid_time = max_time * 0.5,
            delta_time = max_time - mid_time,
            timeout_step = 0.01;

        const double
            start_value = Stage::value_wave,
            delta_value = max_value - start_value;

        std::shared_ptr<double> now_ptr(new double);
        *now_ptr = 0.0;

        this->clearBonusTimeouts(type);

        this->timeouts[type] = {

            this->window.setTimeout([=] () {

                const double now = *now_ptr;
                *now_ptr += timeout_step;

                if (now < mid_time) {
                    Stage::value_wave = Engine::Easing::Quad::InOut(now, start_value, delta_value, mid_time);
                } else if (now < max_time) {
                    Stage::value_wave = Engine::Easing::Quad::InOut(now - mid_time, max_value, -max_value, delta_time);
                } else {
                    this->deactivateBonus(type);
                    return false;
                }

                return true;
            }, timeout_step, true)
        };
    }

    void Stage::activateBonusRotate (const Stage::BonusType type) {

        constexpr double
            max_time = 25.0,
            max_value = Engine::Mesh::PI,
            mid_time = max_time * 0.05,
            delta_time = max_time - mid_time,
            timeout_step = 0.01;

        const double
            start_value = Stage::value_rotate,
            delta_value = max_value - start_value;

        std::shared_ptr<double> now_ptr(new double);
        *now_ptr = 0.0;

        this->clearBonusTimeouts(type);

        if (start_value != max_value) {
            this->window.pause();
        }

        this->timeouts[type] = {
            this->window.setTimeout([=] () {

                const double now = *now_ptr;
                *now_ptr += timeout_step;

                if (now < mid_time) {
                    Stage::value_rotate = Engine::Easing::Back::Out(now, start_value, delta_value, mid_time);
                } else {
                    this->window.unpause();
                    Stage::value_rotate = max_value;

                    this->timeouts[type].push_back(
                        this->window.setTimeout([=] () {

                            *now_ptr = 0.0;
                            this->window.pause();

                            this->timeouts[type].push_back(
                                this->window.setTimeout([=] () {

                                    const double now = *now_ptr;
                                    *now_ptr += timeout_step;

                                    if (now < mid_time) {
                                        Stage::value_rotate = Engine::Easing::Sine::Out(now, max_value, -max_value, mid_time);
                                    } else {
                                        this->window.unpause();
                                        this->deactivateBonus(type);
                                        return false;
                                    }

                                    return true;
                                }, timeout_step)
                            );

                            return false;
                        }, delta_time - mid_time, true)
                    );
                    return false;
                }

                return true;
            }, timeout_step)
        };
    }

    void Stage::activateBonusBall (const Stage::BonusType type) {

        constexpr double
            max_time = 15.0,
            mid_time = max_time * 0.05,
            max_radius = Ball::DefaultRadius() * 2.0,
            delta_time = max_time - mid_time,
            back_radius = Ball::DefaultRadius() - max_radius,
            timeout_step = 0.01;

        const double
            start_radius = this->getBall()->getRadius(),
            delta_radius = max_radius - start_radius;

        std::shared_ptr<double> now_ptr(new double);
        *now_ptr = 0.0;

        this->clearBonusTimeouts(type);

        this->timeouts[type] = {

            this->window.setTimeout([=] () {

                const double now = *now_ptr;
                *now_ptr += timeout_step;

                if (now < mid_time) {
                    this->getBall()->setRadius(Engine::Easing::Elastic::InOut(now, start_radius, delta_radius, mid_time));
                } else if (now < max_time) {
                    this->getBall()->setRadius(Engine::Easing::Expo::In(now - mid_time, max_radius, back_radius, delta_time));
                } else {
                    this->deactivateBonus(type);
                    return false;
                }

                return true;
            }, timeout_step, true)

        };
    }

    void Stage::activateBonusPaddler (const Stage::BonusType type) {

        constexpr double
            max_time = 15.0,
            mid_time = max_time * 0.05,
            max_width = Paddler::DefaultWidth() * 1.5,
            delta_time = max_time - mid_time,
            back_width = Paddler::DefaultWidth() - max_width,
            timeout_step = 0.01;

        const double
            start_width = this->getPaddler()->getWidth(),
            delta_width = max_width - start_width;

        std::shared_ptr<double> now_ptr(new double);
        *now_ptr = 0.0;

        this->clearBonusTimeouts(type);

        this->timeouts[type] = {

            this->window.setTimeout([=] () {

                const double now = *now_ptr;
                *now_ptr += timeout_step;

                if (now < mid_time) {
                    this->getPaddler()->setWidth(Engine::Easing::Back::In(now, start_width, delta_width, mid_time));
                } else if (now < max_time) {
                    this->getPaddler()->setWidth(Engine::Easing::Expo::In(now - mid_time, max_width, back_width, delta_time));
                } else {
                    this->deactivateBonus(type);
                    return false;
                }

                return true;
            }, timeout_step, true)
        };
    }

};
