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
            BonusFastSpeed = 4,
            BonusSlowSpeed = 5,

            BonusTypeSize = 6

        };

        static Engine::Shader::Program shader_wave_rotate;
        static double value_wave, value_rotate, time_wave;
        static bool active_wave, active_rotate;
        static Engine::Audio::Sound bonus_sounds[];
        static int music_volume;
        static std::default_random_engine random_generator;

        Engine::Audio::Sound music;
        Engine::Window &window;
        std::unordered_set<Brick *> can_destroy, cannot_destroy;
        Ball *ball = nullptr;
        Paddler *paddler = nullptr;
        std::vector<unsigned> timeouts[static_cast<int>(BonusType::BonusTypeSize)] = { { } };
        bool cleared = true, win = false, loss = false, active_bonuses[static_cast<int>(BonusType::BonusTypeSize)] = { false };
        unsigned start_pause_context, rotate_pause_context = 0, lives = 3;
        double min_speed, max_speed, ball_x, ball_y;

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
                    Stage::time_wave = 0.0;
                break;
                case BonusType::BonusRotate:
                    this->window.unpause(this->rotate_pause_context);
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
                case BonusType::BonusFastSpeed:
                case BonusType::BonusSlowSpeed:
                    this->window.setSpeed(1.0);
                break;
                default: break;
            }
        }

        void activateBonusWave(void);
        void activateBonusRotate(void);
        void activateBonusBall(void);
        void activateBonusPaddler(void);
        void activateBonusFastSpeed(double remaining_time = 0.0);
        void activateBonusSlowSpeed(double remaining_time = 0.0);

        void activateBonus (const BonusType type) {

            if (!this->cleared) {

                switch (type) {
                    case BonusType::BonusWave:
                        activateBonusWave();
                        this->window.setShader(&Stage::shader_wave_rotate);
                    break;
                    case BonusType::BonusRotate:
                        activateBonusRotate();
                        this->window.setShader(&Stage::shader_wave_rotate);
                    break;
                    case BonusType::BonusBall:
                        activateBonusBall();
                    break;
                    case BonusType::BonusPaddler:
                        activateBonusPaddler();
                    break;
                    case BonusType::BonusFastSpeed:
                        activateBonusFastSpeed();
                    break;
                    case BonusType::BonusSlowSpeed:
                        activateBonusSlowSpeed();
                    break;
                    default: break;
                }
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
                    std::uniform_int_distribution<int> rand(0, BonusType::BonusTypeSize - 1);
                    this->activateBonus(static_cast<BonusType>(rand(Stage::random_generator)));
                }, width, height, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, type - 3);
            } else if (type <= 8) {
                brick = new AbstractBrick(window, { x, y, 4.0 }, bg, width, height, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, type - 7);
            }

            return brick;
        }

    public:

        static constexpr double DefaultVerticalSpace = 0.01, DefaultHorizontalSpace = 0.01;

        Stage (
            Engine::Window &_window,
            const std::string &file
        );

        inline ~Stage (void) { this->clear(); }

        void start(void);
        void clear(void);

        void update (void) {
            if (this->can_destroy.empty()) {
                this->clear();
                this->win = true;
            }
        }

        inline bool isClear (void) const { return this->cleared; }
        inline bool won (void) const { return this->win; }
        inline bool lost (void) const { return this->loss; }

        inline void addBrick (const std::string &id, double x, double y, double width, double height) {

            Brick *brick = Stage::createBrickByID(this->window, id, x, y, width, height);

            if (brick) {
                if (brick->isDestructible()) {
                    this->can_destroy.insert(brick);
                } else {
                    this->cannot_destroy.insert(brick);
                }
            }
        }

        Ball *getBall (void) const { return this->ball; }
        Paddler *getPaddler (void) const { return this->paddler; }

        inline Engine::Window &getWindow (void) const { return this->window; }

    };

}

#endif
