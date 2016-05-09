#include "stage.h"

namespace Breakout {

    Engine::Shader::Program Stage::shader_wave_rotate;
    double Stage::value_wave = 0.0, Stage::value_rotate = 0.0, Stage::time_wave = 0.0;
    Engine::Audio::Sound Stage::bonus_sounds[static_cast<int>(BonusType::BonusTypeSize)];
    int Stage::music_volume = 8;
    std::default_random_engine Stage::random_generator(std::chrono::system_clock::now().time_since_epoch().count());

    Stage::Stage (
        Engine::Window &_window,
        const std::string &file
    ) : window(_window) {

        std::ifstream input(file, std::ios::in);

        if (input.is_open()) {

            bool ok;
            std::string block, music_path = "audio/themes/";
            std::stringstream ss(Stage::nextLine(input, ok));
            double width, height, x, y = 0.9 - (Stage::DefaultVerticalSpace / 2.0);

            this->cleared = false;

            ss >> this->max_speed;

            ss.str(Stage::nextLine(input, ok));
            ss.seekg(0) >> this->min_speed;

            ss.str(Stage::nextLine(input, ok));
            ss.seekg(0) >> width;

            ss.str(Stage::nextLine(input, ok));
            ss.seekg(0) >> height;

            ss.str(Stage::nextLine(input, ok));
            ss.seekg(0) >> this->ball_x;

            ss.str(Stage::nextLine(input, ok));
            ss.seekg(0) >> this->ball_y;

            music_path += Stage::nextLine(input, ok);

            this->music.load(music_path);
            this->music.start(-1);
            this->music.pause();

            for (std::string line = Stage::nextLine(input, ok); ok; line = Stage::nextLine(input, ok)) {

                x = -1.0 + (Stage::DefaultHorizontalSpace / 2.0);

                ss.str(line);
                ss.seekg(0);

                while (ss.good()) {

                    ss >> block;

                    if (block[0] != '-') {
                        this->addBrick(block, x, y, width, height);
                    }
                    x += width + Stage::DefaultHorizontalSpace;
                }
                y -= height + Stage::DefaultVerticalSpace;
            }

            input.close();

            if (!Stage::shader_wave_rotate) {
                try {
                    Stage::shader_wave_rotate.attachVertexShader({ Engine::Shader::wave_rotate_vertex });
                    Stage::shader_wave_rotate.attachFragmentShader({ Engine::Shader::wave_rotate_fragment });
                } catch (std::string e) {
                    std::cerr << e << std::endl;
                }

                Stage::shader_wave_rotate.link();

                Stage::shader_wave_rotate.onAfterUse([] (Engine::Shader::Program *program) {
                    glUniform1fARB(program->getUniformLocationARB("parameter_wave"), Stage::value_wave);
                    glUniform1fARB(program->getUniformLocationARB("parameter_rotate"), Stage::value_rotate);
                    glUniform1fARB(program->getUniformLocationARB("time"), Stage::time_wave);
                });

                Stage::bonus_sounds[BonusType::BonusWave].load("audio/bonus/onda_onda.ogg");
                Stage::bonus_sounds[BonusType::BonusRotate].load("audio/bonus/roda_roda_roda.ogg");
                Stage::bonus_sounds[BonusType::BonusBall].load("audio/bonus/gordinho_gostoso.ogg");
                Stage::bonus_sounds[BonusType::BonusPaddler].load("audio/bonus/mario_mushroom.ogg");
                Stage::bonus_sounds[BonusType::BonusFastSpeed].load("audio/bonus/acelera_acelera.ogg");
                Stage::bonus_sounds[BonusType::BonusSlowSpeed].load("audio/bonus/vai_devagar.ogg");
            }
        }
    }

    void Stage::start (void) {

        if (this->can_destroy.empty()) {

            this->win = true;

        } else {

            this->start_pause_context = this->window.pause();

            this->music.setVolume(Stage::music_volume);

            this->window.eraseEvent<Engine::Event::Keyboard>("keyboard.pause");

            this->window.event<Engine::Event::Keyboard>([ this ] (GLFWwindow *window, int key, int code, int action, int mods) {
                if (action == GLFW_PRESS || action == GLFW_REPEAT) {

                    if (action == GLFW_PRESS) {
                        if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_ENTER) {
                            if (this->window.isPaused()) {
                                this->window.unpause(this->start_pause_context);
                                this->music.play();
                            } else {
                                this->start_pause_context = this->window.pause();
                                this->music.pause();
                            }
                        }
                    }

                    if (key == GLFW_KEY_MINUS) {
                        if (mods & GLFW_MOD_SHIFT) {
                            this->music.mute();
                        } else {
                            Stage::music_volume = this->music.getVolume() - 4;
                            this->music.setVolume(Stage::music_volume);
                        }
                    } else if (key == GLFW_KEY_EQUAL) {
                        if (mods & GLFW_MOD_SHIFT) {
                            this->music.maxVolume();
                        } else {
                            Stage::music_volume = this->music.getVolume() + 4;
                            this->music.setVolume(Stage::music_volume);
                        }
                    }
                }
            }, "keyboard.pause");

            this->ball = new Ball(this->max_speed, this->min_speed, [ this ] () {

                if (this->lives > 1) {
                    this->ball->start();
                    --this->lives;
                } else {
                    this->loss = true;
                }

            }, [ this ] (const Breakout::Brick *destroyed) {

                this->can_destroy.erase(const_cast<Breakout::Brick *>(destroyed));

            }, { this->ball_x, this->ball_y, 4.0 });

            this->paddler = new Paddler(this->window, this->max_speed / 1.5);

            for (auto &brick : this->can_destroy) {
                this->window.addObject(brick);
            }

            for (auto &brick : this->cannot_destroy) {
                this->window.addObject(brick);
            }

            this->window.addObject(this->ball);
            this->window.addObject(this->paddler);
            this->ball->start();
        }
    }

    void Stage::clear (void) {

        for (int i = 0; i < static_cast<int>(BonusType::BonusTypeSize); ++i) {
            if (this->active_bonuses[i]) {
                this->deactivateBonus(static_cast<BonusType>(i));
            }
        }

        this->window.eraseEvent<Engine::Event::Keyboard>("keyboard.pause");

        if (!this->cleared) {

            this->cleared = true;

            if (this->ball) {

                this->music.fadeOut(1000);

                this->ball->destroy();
                this->paddler->destroy();

                for (auto &brick : this->can_destroy) {
                    brick->destroy();
                }

                for (auto &brick : this->cannot_destroy) {
                    brick->destroy();
                }

            }
        }

        this->window.unpause(this->start_pause_context);
    }

    void Stage::activateBonusWave (void) {

        std::uniform_real_distribution<double> random_time(15.0, 25.0);

        constexpr double
            max_value = 5.0,
            timeout_step = 0.01;

        const double
            max_time = random_time(Stage::random_generator),
            mid_time = max_time * 0.5,
            delta_time = max_time - mid_time,
            start_value = Stage::value_wave,
            delta_value = max_value - start_value;

        std::shared_ptr<double> now_ptr(new double);
        *now_ptr = 0.0;

        this->clearBonusTimeouts(BonusType::BonusWave);

        this->active_bonuses[BonusType::BonusWave] = true;
        Stage::bonus_sounds[BonusType::BonusWave].play();

        this->timeouts[BonusType::BonusWave] = {

            this->window.setTimeout([=] () {

                const double now = *now_ptr;
                *now_ptr += timeout_step;
                Stage::time_wave += timeout_step;

                if (now < mid_time) {
                    Stage::value_wave = Engine::Easing::Quad::InOut(now, start_value, delta_value, mid_time);
                } else if (now < max_time) {
                    Stage::value_wave = Engine::Easing::Quad::InOut(now - mid_time, max_value, -max_value, delta_time);
                } else {
                    this->deactivateBonus(BonusType::BonusWave);
                    return false;
                }

                return true;
            }, timeout_step, true)
        };
    }

    void Stage::activateBonusRotate (void) {

        std::uniform_real_distribution<double> random_time(20.0, 30.0);

        constexpr double
            max_value = Engine::Mesh::PI,
            timeout_step = 0.01;

        const double
            max_time = random_time(Stage::random_generator),
            mid_time = max_time * 0.05,
            delta_time = max_time - mid_time,
            start_value = Stage::value_rotate,
            delta_value = max_value - start_value;

        std::shared_ptr<double> now_ptr(new double);
        *now_ptr = 0.0;

        this->clearBonusTimeouts(BonusType::BonusRotate);

        this->active_bonuses[BonusType::BonusRotate] = true;
        Stage::bonus_sounds[BonusType::BonusRotate].play();

        if (start_value != max_value) {
            this->rotate_pause_context = this->window.pause();
        }

        this->timeouts[BonusType::BonusRotate] = {
            this->window.setTimeout([=] () {

                const double now = *now_ptr;
                *now_ptr += timeout_step;

                if (now < mid_time) {
                    Stage::value_rotate = Engine::Easing::Back::Out(now, start_value, delta_value, mid_time);
                } else {
                    this->window.unpause(this->rotate_pause_context);
                    Stage::value_rotate = max_value;

                    this->timeouts[BonusType::BonusRotate].push_back(
                        this->window.setTimeout([=] () {

                            *now_ptr = 0.0;
                            this->rotate_pause_context = this->window.pause();

                            this->timeouts[BonusType::BonusRotate].push_back(
                                this->window.setTimeout([=] () {

                                    const double now = *now_ptr;
                                    *now_ptr += timeout_step;

                                    if (now < mid_time) {
                                        Stage::value_rotate = Engine::Easing::Sine::Out(now, max_value, -max_value, mid_time);
                                    } else {
                                        this->window.unpause(this->rotate_pause_context);
                                        this->deactivateBonus(BonusType::BonusRotate);
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

    void Stage::activateBonusBall (void) {

        std::uniform_real_distribution<double> random_time(15.0, 25.0);

        constexpr double
            max_radius = Ball::DefaultRadius() * 2.0,
            back_radius = Ball::DefaultRadius() - max_radius,
            timeout_step = 0.01;

        const double
            max_time = random_time(Stage::random_generator),
            mid_time = max_time * 0.05,
            delta_time = max_time - mid_time,
            start_radius = this->getBall()->getRadius(),
            delta_radius = max_radius - start_radius;

        std::shared_ptr<double> now_ptr(new double);
        *now_ptr = 0.0;

        this->clearBonusTimeouts(BonusType::BonusBall);

        this->active_bonuses[BonusType::BonusBall] = true;
        Stage::bonus_sounds[BonusType::BonusBall].play();

        this->timeouts[BonusType::BonusBall] = {

            this->window.setTimeout([=] () {

                const double now = *now_ptr;
                *now_ptr += timeout_step;

                if (now < mid_time) {
                    this->getBall()->setRadius(Engine::Easing::Elastic::InOut(now, start_radius, delta_radius, mid_time));
                } else if (now < max_time) {
                    this->getBall()->setRadius(Engine::Easing::Expo::In(now - mid_time, max_radius, back_radius, delta_time));
                } else {
                    this->deactivateBonus(BonusType::BonusBall);
                    return false;
                }

                return true;
            }, timeout_step, true)

        };
    }

    void Stage::activateBonusPaddler (void) {

        std::uniform_real_distribution<double> random_time(15.0, 25.0);

        constexpr double
            max_width = Paddler::DefaultWidth() * 1.5,
            back_width = Paddler::DefaultWidth() - max_width,
            timeout_step = 0.01;

        const double
            max_time = random_time(Stage::random_generator),
            mid_time = max_time * 0.05,
            delta_time = max_time - mid_time,
            start_width = this->getPaddler()->getWidth(),
            delta_width = max_width - start_width;

        std::shared_ptr<double> now_ptr(new double);
        *now_ptr = 0.0;

        this->clearBonusTimeouts(BonusType::BonusPaddler);

        this->active_bonuses[BonusType::BonusPaddler] = true;
        Stage::bonus_sounds[BonusType::BonusPaddler].play();

        this->timeouts[BonusType::BonusPaddler] = {

            this->window.setTimeout([=] () {

                const double now = *now_ptr;
                *now_ptr += timeout_step;

                if (now < mid_time) {
                    this->getPaddler()->setWidth(Engine::Easing::Back::Out(now, start_width, delta_width, mid_time));
                } else if (now < max_time) {
                    this->getPaddler()->setWidth(Engine::Easing::Expo::In(now - mid_time, max_width, back_width, delta_time));
                } else {
                    this->deactivateBonus(BonusType::BonusPaddler);
                    return false;
                }

                return true;
            }, timeout_step, true)
        };
    }

    void Stage::activateBonusFastSpeed (double remaining_time) {

        if (remaining_time <= 0.0) {
            std::uniform_real_distribution<double> random_remaining_time(30.0, 40.0);
            remaining_time = random_remaining_time(Stage::random_generator);
        }

        std::uniform_real_distribution<double> random_time(2.0, 4.0);

        constexpr double
            max_value = 2.0,
            timeout_step = 0.01;

        const double
            max_time = random_time(Stage::random_generator),
            mid_time = max_time * 0.05,
            end_time = max_time + mid_time,
            start_value = this->window.getSpeed(),
            delta_value = max_value - start_value,
            back_value = 1.0 - max_value;

        remaining_time -= max_time;

        std::shared_ptr<double> now_ptr(new double);
        *now_ptr = 0.0;

        this->clearBonusTimeouts(BonusType::BonusFastSpeed);
        this->deactivateBonus(BonusType::BonusSlowSpeed);

        this->active_bonuses[BonusType::BonusFastSpeed] = true;
        Stage::bonus_sounds[BonusType::BonusFastSpeed].play();

        this->timeouts[BonusType::BonusFastSpeed] = {

            this->window.setTimeout([=] () {

                const double now = *now_ptr;
                *now_ptr += timeout_step;

                if (now < mid_time) {
                    this->window.setSpeed(Engine::Easing::Expo::Out(now, start_value, delta_value, mid_time));
                } else if (now >= max_time) {
                    if (remaining_time > 0.0) {
                        this->deactivateBonus(BonusType::BonusFastSpeed);
                        this->activateBonusSlowSpeed(remaining_time);
                        return false;
                    }
                    if (now < end_time) {
                        this->window.setSpeed(Engine::Easing::Expo::Out(now - max_time, max_value, back_value, mid_time));
                    } else {
                        this->deactivateBonus(BonusType::BonusFastSpeed);
                        return false;
                    }
                }

                return true;
            }, timeout_step, true)

        };
    }

    void Stage::activateBonusSlowSpeed (double remaining_time) {

        if (remaining_time <= 0.0) {
            std::uniform_real_distribution<double> random_remaining_time(30.0, 40.0);
            remaining_time = random_remaining_time(Stage::random_generator);
        }

        std::uniform_real_distribution<double> random_time(4.0, 8.0);

        constexpr double
            max_value = 0.5,
            timeout_step = 0.01;

        const double
            max_time = random_time(Stage::random_generator),
            mid_time = max_time * 0.05,
            end_time = max_time + mid_time,
            start_value = this->window.getSpeed(),
            delta_value = max_value - start_value,
            back_value = 1.0 - max_value;

        remaining_time -= max_time;

        std::shared_ptr<double> now_ptr(new double);
        *now_ptr = 0.0;

        this->clearBonusTimeouts(BonusType::BonusSlowSpeed);
        this->deactivateBonus(BonusType::BonusFastSpeed);

        this->active_bonuses[BonusType::BonusSlowSpeed] = true;
        Stage::bonus_sounds[BonusType::BonusSlowSpeed].play();

        this->timeouts[BonusType::BonusSlowSpeed] = {

            this->window.setTimeout([=] () {

                const double now = *now_ptr;
                *now_ptr += timeout_step;

                if (now < mid_time) {
                    this->window.setSpeed(Engine::Easing::Expo::Out(now, start_value, delta_value, mid_time));
                } else if (now >= max_time) {
                    if (remaining_time > 0.0) {
                        this->deactivateBonus(BonusType::BonusSlowSpeed);
                        this->activateBonusFastSpeed(remaining_time);
                        return false;
                    }
                    if (now < end_time) {
                        this->window.setSpeed(Engine::Easing::Expo::Out(now - max_time, max_value, back_value, mid_time));
                    } else {
                        this->deactivateBonus(BonusType::BonusSlowSpeed);
                        return false;
                    }
                }

                return true;
            }, timeout_step, true)

        };
    }

};
