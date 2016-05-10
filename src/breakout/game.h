#ifndef SRC_BREAKOUT_GAME_H_
#define SRC_BREAKOUT_GAME_H_

#include <queue>
#include "stage.h"
#include "../engine/window.h"

namespace Breakout {

    class Game {

        Engine::Window &window;
        std::queue<Stage *> stages;
        bool won = false, lost = false;
        Engine::Audio::Sound sound_win, sound_lose;
        GLuint texture_win, texture_lose;

        inline void nextStage (void) {
            if (!this->stages.empty()) {
                this->stages.front()->start();
            } else {
                this->sound_win.play();
                this->won = true;
            }
        }

    public:

        Game(Engine::Window &_window, std::vector<std::string> _stages);

        inline ~Game (void) { this->clear(); }

        void clear(void);

        inline void start (void) { this->nextStage(); }

        inline void update (void) {
            if (!this->stages.empty()) {
                Stage *stage = this->stages.front();
                stage->update();
                if (stage->won()) {
                    this->stages.pop();
                    this->nextStage();
                    delete stage;
                } else if (stage->lost()) {
                    this->sound_lose.play();
                    this->lost = true;
                    this->clear();
                }
            } else if (this->won) {
                this->window.addTexture2D(this->texture_win, 1.0, 2.0, { -0.5, -1.0, 4.0 });
            } else if (this->lost) {
                this->window.addTexture2D(this->texture_lose, 2.0, 1.0, { -1.0, -0.5, 4.0 });
            }
        }

    };

}

#endif
