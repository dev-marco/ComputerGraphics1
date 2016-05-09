#ifndef SRC_BREAKOUT_GAME_H_
#define SRC_BREAKOUT_GAME_H_

#include <queue>
#include "stage.h"
#include "../engine/window.h"

namespace Breakout {

    class Game {

        Engine::Window &window;
        std::queue<Stage *> stages;

        inline void nextStage (void) const {
            if (!this->stages.empty()) {
                this->stages.front()->start();
            } else {
                std::cout << "YOU WIN" << std::endl;
            }
        }

    public:

        Game(Engine::Window &_window, std::vector<std::string> _stages);

        inline ~Game (void) { this->clear(); }

        void clear(void);

        inline void start (void) const { this->nextStage(); }

        inline void update (void) {
            if (!this->stages.empty()) {
                Stage *stage = this->stages.front();
                stage->update();
                if (stage->won()) {
                    this->stages.pop();
                    this->nextStage();
                    delete stage;
                } else if (stage->lost()) {
                    std::cout << "YOU LOSE" << std::endl;
                    this->clear();
                }
            }
        }

    };

}

#endif
