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
#include "../engine/audio.h"
#include "../engine/shader.h"
#include "../engine/shaders/waverotate.h"

namespace Breakout {

    class Stage {

        enum BonusType : int {

            BonusWave = 0,
            BonusRotate = 1,
            BonusBall = 2,
            BonusPaddler = 3,

            BonusTypeSize = 4

        };

        static Engine::Shader::Program shader_wave_rotate;
        static double value_wave, value_rotate;
        static bool active_wave, active_rotate;
        static Engine::Audio::Sound bonus_sounds[];

        Engine::Audio::Sound music;
        Engine::Window &window;
        std::unordered_set<Brick *> can_destroy, cannot_destroy;
        Ball *ball;
        Paddler *paddler;
        std::vector<unsigned> timeouts[static_cast<int>(BonusType::BonusTypeSize)] = { { } };
        bool active_bonuses[static_cast<int>(BonusType::BonusTypeSize)] = { false };

        void clearBonusTimeouts (const BonusType type) {
            for (unsigned u : this->timeouts[type]) {
                this->window.clearTimeout(u);
            }

            this->timeouts[type].clear();
        }

        void deactivateBonus (const BonusType type) {

            this->clearBonusTimeouts(type);
            this->active_bonuses[type] = false;

            switch (type) {
                case BonusType::BonusWave:
                    if (!this->active_bonuses[BonusType::BonusRotate]) {
                        this->window.setShader(nullptr);
                    }
                    Stage::value_wave = 0.0;
                break;
                case BonusType::BonusRotate:
                    if (!this->active_bonuses[BonusType::BonusWave]) {
                        this->window.setShader(nullptr);
                    }
                    Stage::value_rotate = 0.0;
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

        void activateBonusWave(const BonusType type);
        void activateBonusRotate(const BonusType type);
        void activateBonusBall(const BonusType type);
        void activateBonusPaddler(const BonusType type);

        void activateBonus (const BonusType type) {

            this->active_bonuses[type] = true;

            switch (type) {
                case BonusType::BonusWave:
                    activateBonusWave(type);
                    this->window.setShader(&Stage::shader_wave_rotate);
                break;
                case BonusType::BonusRotate:
                    activateBonusRotate(type);
                    this->window.setShader(&Stage::shader_wave_rotate);
                break;
                case BonusType::BonusBall:
                    activateBonusBall(type);
                break;
                case BonusType::BonusPaddler:
                    activateBonusPaddler(type);
                break;
                default: break;
            }
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

            unsigned divisor = id.find_first_of('#'), type = stoul(id.substr(0, divisor));
            Brick *brick = nullptr;

            Engine::BackgroundColor *bg = new Engine::BackgroundColor(Engine::Color::hex(id.substr(divisor)));

            if (type < 4) {
                brick = new Brick(window, { x, y, 4.0 }, bg, width, height, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, type);
            } else if (type < 7) {
                brick = new BonusBrick(window, { x, y, 4.0 }, bg, [=] (void) {
                    static std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());
                    std::uniform_int_distribution<int> rand(0, BonusType::BonusTypeSize - 1);
                    this->activateBonus(static_cast<BonusType>(rand(gen)));
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
            std::string block, music_path = "audio/";
            std::stringstream ss(Stage::nextLine(input, ok));
            double max_speed, min_speed, width, height, ball_x, ball_y, x, y = 0.9 - (Stage::DefaultVerticalSpace / 2.0);

            ss >> max_speed;

            ss.str(Stage::nextLine(input, ok));
            ss.seekg(0) >> min_speed;

            ss.str(Stage::nextLine(input, ok));
            ss.seekg(0) >> width;

            ss.str(Stage::nextLine(input, ok));
            ss.seekg(0) >> height;

            ss.str(Stage::nextLine(input, ok));
            ss.seekg(0) >> ball_x;

            ss.str(Stage::nextLine(input, ok));
            ss.seekg(0) >> ball_y;

            music_path += Stage::nextLine(input, ok);

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
                    glUniform1fARB(program->getUniformLocationARB("time"), glfwGetTime());
                });
            }

            this->music.load(music_path);
            this->music.fadeIn(1000, -1);

            this->ball = new Ball(max_speed, min_speed);
            this->window.addObject(this->ball);

            this->paddler = new Paddler(this->window, max_speed / 1.5);
            this->window.addObject(this->paddler);
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
