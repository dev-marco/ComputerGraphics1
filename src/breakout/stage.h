#ifndef SRC_BREAKOUT_STAGE_H_
#define SRC_BREAKOUT_STAGE_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <unordered_map>
#include <random>
#include <chrono>
#include "brick.h"
#include "ball.h"
#include "paddler.h"
#include "../engine/window.h"
#include "../engine/shader.h"
#include "../engine/shaders/waveshader.h"

namespace Breakout {

    class Stage {

        static Engine::Shader::Program shader_wave, shader_rotation;
        static std::function<double()> wave_function;

        enum BonusType : int {

            BonusWave = 0,
            BonusRotate = 1,
            BonusBall = 2,
            BonusPaddler = 3,

            BonusTypeSize = 4

        };

        Engine::Window &window;
        std::unordered_set<Brick *> can_destroy, cannot_destroy;

        Ball *ball;
        Paddler *paddler;
        unsigned activeBonuses[static_cast<int>(BonusType::BonusTypeSize)] = { 0 };
        std::vector<unsigned> extraTimeouts[static_cast<int>(BonusType::BonusTypeSize)] = { { } };

        void deactivateBonus (BonusType type) {

            this->window.clearTimeout(this->activeBonuses[type]);

            for (unsigned u : this->extraTimeouts[type]) {
                this->window.clearTimeout(u);
            }

            this->extraTimeouts[type].clear();

            this->activeBonuses[type] = 0;

            switch (type) {
                case BonusType::BonusWave:
                    this->window.setShader(nullptr);
                break;
                case BonusType::BonusRotate:

                break;
                case BonusType::BonusBall:
                    this->getBall()->setRadius(Ball::DefaultRadius());
                break;
                case BonusType::BonusPaddler:
                    this->getPaddler()->setWidth(Paddler::DefaultWidth());
                break;
                default: break;
            }

        }

        void activateBonus (BonusType type) {

            double bonus_time = 0.0;

            switch (type) {
                case BonusType::BonusWave: {

                    constexpr double
                        max_time = 15.0,
                        max_value = 15.0,
                        mid_time = max_time * 0.5,
                        delta_time = max_time - mid_time;

                    const double
                        start_time = glfwGetTime(),
                        start_value = activeBonuses[type] ? Stage::wave_function() : 0.0,
                        delta_value = max_value - start_value;

                    bonus_time = max_time;

                    Stage::wave_function = [ start_value, max_value, delta_value, start_time, mid_time, delta_time, max_time ] () {

                        double now = glfwGetTime() - start_time;

                        if (now <= mid_time) {
                            return std::min(Engine::Easing::Quad::InOut(now, start_value, delta_value, mid_time), max_value);
                        } else {
                            return std::max(Engine::Easing::Quad::InOut(now - mid_time, max_value, -max_value, delta_time), 0.0);
                        }
                    };

                    this->window.setShader(&Stage::shader_wave);
                } break;
                case BonusType::BonusRotate:

                break;
                case BonusType::BonusBall: {

                    constexpr double
                        max_time = 15.0,
                        mid_time = max_time * 0.125,
                        max_radius = Ball::DefaultRadius() * 2.0,
                        delta_time = max_time - mid_time,
                        back_radius = Ball::DefaultRadius() - max_radius;

                    Ball *ball = this->getBall();

                    const double
                        start_time = glfwGetTime(),
                        start_radius = ball->getRadius(),
                        delta_radius = max_radius - start_radius;

                    bonus_time = max_time;

                    for (unsigned u : this->extraTimeouts[type]) {
                        this->window.clearTimeout(u);
                    }

                    this->extraTimeouts[type] = {

                        this->window.setTimeout([
                            ball,
                            start_time, mid_time, delta_time, max_time,
                            back_radius, start_radius, delta_radius, max_radius
                        ] () {

                            double now = glfwGetTime() - start_time;

                            if (now <= mid_time) {
                                ball->setRadius(std::min(
                                    Engine::Easing::Elastic::InOut(now, start_radius, delta_radius, mid_time),
                                    max_radius
                                ));
                            } else {
                                ball->setRadius(std::max(
                                    Engine::Easing::Expo::In(now - mid_time, max_radius, back_radius, delta_time),
                                    Ball::DefaultRadius()
                                ));
                            }

                            return now < max_time;
                        }, 0.1)

                    };
                } break;
                case BonusType::BonusPaddler:
                    bonus_time = 15.0;
                    this->getPaddler()->setWidth(0.8);
                break;
                default: return;
            }

            this->window.clearTimeout(activeBonuses[type]);

            activeBonuses[type] = this->window.setTimeout([this, type] (void) mutable {
                this->deactivateBonus(type);
                return false;
            }, bonus_time, true);
        }

        static inline bool not_space (int c) {
        	return !isspace(c);
        }

        static inline std::string &ltrim (std::string &s) {
        	s.erase(s.begin(), find_if(s.begin(), s.end(), not_space));
        	return s;
        }

        static inline std::string &rtrim (std::string &s) {
        	s.erase(find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
        	return s;
        }

        static inline std::string &trim (std::string &s) {
        	return ltrim(rtrim(s));
        }

        static std::string nextLine (
        	std::istream &in,
            bool &ok
        ) {
        	std::string line = "";
        	do {
        		std::getline(in, line);
        		line = line.substr(0, line.find_first_of('>'));
        	} while (!trim(line).size() && in.good());

            ok = line.size() > 0;

        	return line;
        }

        Brick *createBrickByID (Engine::Window &window, const std::string &id, const double x, const double y, const double width, const double height) {

            std::default_random_engine gen(glfwGetTime() * 1000000);
            std::uniform_int_distribution<int> rgb(0, 255);
            std::uniform_real_distribution<double> alpha(0.3, 1.0);
            unsigned divisor = id.find_first_of('#'), type = stoul(id.substr(0, divisor));
            Brick *brick = nullptr;

            Engine::BackgroundColor *bg = new Engine::BackgroundColor(Engine::Color::hex(id.substr(divisor)));

            if (type < 4) {
                brick = new Brick(window, { x, y, 4.0 }, bg, width, height, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, type);
            } else if (type < 7) {
                brick = new BonusBrick(window, { x, y, 4.0 }, bg, [this] (void) mutable {
                    std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
                    std::uniform_int_distribution<int> rand(0, BonusType::BonusTypeSize - 1);
                    this->activateBonus(static_cast<BonusType>(rand(gen)*0+3));
                }, width, height, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, type - 3);
            } else if (type <= 8) {
                brick = new AbstractBrick(window, { x, y, 4.0 }, bg, width, height, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, type - 7);
            }

            return brick;
        }

    public:

        static constexpr double DefaultVerticalSpace = 0.01, DefaultHorizontalSpace = 0.01;

        Stage (Engine::Window &_window, const std::string &file) : window(_window) {

            bool ok;
            std::ifstream input(file, std::ios::in);
            std::string block;
            std::stringstream ss(Stage::nextLine(input, ok));
            double max_speed, min_speed, width, height, x, y = 0.9 - (Stage::DefaultVerticalSpace / 2.0);

            ss >> max_speed;

            ss.str(Stage::nextLine(input, ok));
            ss.seekg(0) >> min_speed;

            ss.str(Stage::nextLine(input, ok));
            ss.seekg(0) >> width;

            ss.str(Stage::nextLine(input, ok));
            ss.seekg(0) >> height;

            this->ball = new Ball(max_speed, min_speed);
            this->window.addObject(this->ball);

            this->paddler = new Paddler(this->window, max_speed / 1.5);
            this->window.addObject(this->paddler);

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

            if (!Stage::shader_wave) {
                try {
                    Stage::shader_wave.attachVertexShader({ Engine::Shader::wave_vertex });
                    Stage::shader_wave.attachFragmentShader({ Engine::Shader::wave_fragment });
                } catch (std::string e) {
                    std::cerr << e << std::endl;
                }

                Stage::shader_wave.link();

                Stage::shader_wave.onAfterUse([] (Engine::Shader::Program *program) {
                    glUniform1fARB(program->getUniformLocationARB("parameter"), Stage::wave_function());
                    glUniform1fARB(program->getUniformLocationARB("time"), glfwGetTime());
                });
            }
        }

        ~Stage () {

        }

        void addBrick (const std::string &id, double x, double y, double width, double height) {

            Brick *brick = Stage::createBrickByID(this->window, id, x, y, width, height);

            if (brick) {
                if (brick->isDestructible()) {
                    this->can_destroy.insert(brick);
                } else {
                    this->cannot_destroy.insert(brick);
                }
                this->window.addObject(brick);
            }
        }

        Ball *getBall (void) const { return this->ball; }
        Paddler *getPaddler (void) const { return this->paddler; }

        inline Engine::Window &getWindow (void) const { return this->window; }

    };

}

#endif
