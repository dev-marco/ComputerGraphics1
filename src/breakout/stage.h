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

namespace Breakout {

    class Stage {

        enum BonusType : int {

            BonusWave,
            BonusRotate,
            BonusBall,
            BonusPaddler,

            BonusTypeSize

        };

        Window &window;
        std::unordered_set<Brick *> can_destroy, cannot_destroy;

        std::unordered_map<int, unsigned> activeBonuses;

        void deactivateBonus (BonusType type) {

        }

        void activateBonus (BonusType type) {

            if (activeBonuses[type]) {
                this->window.clearTimeout(activeBonuses[type]);
            }

            switch (type) {
                case BonusType::BonusWave:

                break;
                case BonusType::BonusRotate:

                break;
                case BonusType::BonusBall:

                break;
                case BonusType::BonusPaddler:

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

        Brick *createBrickByID (Window &window, const std::string &id, double x, double y, double width, double height) {

            std::default_random_engine gen(glfwGetTime() * 1000000);
            std::uniform_int_distribution<int> rgb(0, 255);
            std::uniform_real_distribution<double> alpha(0.3, 1.0);
            unsigned divisor = id.find_first_of('#'), type = stoul(id.substr(0, divisor));
            Brick *brick = nullptr;

            BackgroundColor *bg = new BackgroundColor(Color::hex(id.substr(divisor)));

            if (type < 4) {
                brick = new Brick(window, { x, y, 4.0 }, bg, width, height, { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, type);
            } else if (type < 7) {
                brick = new BonusBrick(window, { x, y, 4.0 }, bg, [this] (void) mutable {
                    std::mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());
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

        Stage (Window &_window, const std::string &file) : window(_window) {

            bool ok;
            std::ifstream input(file, std::ios::in);
            std::string block;
            std::stringstream ss(Stage::nextLine(input, ok));
            double max_speed, min_speed, width, height, x, y = 0.9 - (Stage::DefaultVerticalSpace / 2.0) - Brick::DefaultHeight;

            ss >> max_speed;

            ss.str(Stage::nextLine(input, ok));
            ss.seekg(0) >> min_speed;

            ss.str(Stage::nextLine(input, ok));
            ss.seekg(0) >> width;

            ss.str(Stage::nextLine(input, ok));
            ss.seekg(0) >> height;

            this->window.addObject(new Ball(max_speed, min_speed));
            this->window.addObject(new Paddler(this->window, max_speed / 1.5));

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

        inline Window &getWindow (void) const { return this->window; }

    };

}

#endif
