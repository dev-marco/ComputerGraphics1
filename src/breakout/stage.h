#ifndef SRC_BREAKOUT_STAGE_H_
#define SRC_BREAKOUT_STAGE_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <unordered_map>
#include "window.h"
#include "brick.h"

namespace Breakout {

    class Stage {

        const Window *window;
        std::unordered_set<std::shared_ptr<Brick *>> can_destroy, cannot_destroy;
        double max_speed = 0.01;
        unsigned brick_lines;

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
        	std::istream &in
        ) {
        	std::string line;
        	do {
        		std::getline(in, line);
        		line = line.substr(0, line.find_first_of('#'));
        	} while (!trim(line).size());
        	return line;
        }

        static Brick *createBrickByID (const Window *window, unsigned id) {

            Brick *brick = nullptr;

            switch (id) {

            }

            return brick;
        }

    public:

        static constexpr double DefaultVerticalSpace = 0.005, DefaultHorizontalSpace = 0.005;

        Stage (const Window *_window, const std::string &file) : window(_window) {

            std::ifstream input(file, std::ios::in);
            std::string block;
            std::stringstream ss(Stage::nextLine(input));
            double x, y = 0.0;

            ss >> this->max_speed;

            ss.str(Stage::nextLine(input));

            ss.seekg(0) >> this->brick_lines;

            for (unsigned i = 0; i < this->brick_lines; ++i) {

                x = 0.0;

                ss.str(Stage::nextLine(input));
                ss.seekg(0);

                while (ss.good()) {

                    ss >> block;

                    if (block[0] != '-') {
                        this->addBrick(std::stoul(block), x, y);
                    }
                    x += Brick::DefaultWidth + Stage::DefaultHorizontalSpace;
                }
                y += Brick::DefaultHeight + Stage::DefaultVerticalSpace;
            }

        }

        void addBrick (unsigned id, double x, double y) {

            Brick *brick = Stage::createBrickByID(this->window, id);

            if (brick) {
                if (brick->isDestructible()) {
                    this->can_destroy.emplace(brick);
                } else {
                    this->cannot_destroy.emplace(brick);
                }
            }
        }

    };

}

#endif
