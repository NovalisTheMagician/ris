#pragma once

namespace RIS
{
    namespace Game
    {
        class GameLoop
        {
        public:
            GameLoop() = default;
            ~GameLoop() = default;
            GameLoop(const GameLoop &) = delete;
            GameLoop &operator=(const GameLoop &) = delete;
            GameLoop(GameLoop &&) = default;
            GameLoop &operator=(GameLoop &&) = default;

            int Start();

        private:

        };
    }
}
