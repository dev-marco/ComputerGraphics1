#ifndef SRC_BREAKOUT_STAGE_H_
#define SRC_BREAKOUT_STAGE_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <unordered_map>
#include "brick.h"

namespace Breakout {

    class Stage {

        std::unordered_set<std::shared_ptr<Brick *>> destructable, indestructable;
        double max_speed = 0.01;

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

    public:

        Stage (const std::string &file) {

            std::ifstream input(file, std::ios::in);
            std::string fragment;
            std::stringstream ss(Stage::nextLine(input));

            ss >> this->max_speed;

            

        }

    };

}

#endif
