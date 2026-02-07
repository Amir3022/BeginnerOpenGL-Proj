#include "GameModules/InstancingGame.h"

int main()
{
    std::unique_ptr<InstancingGame> game = std::make_unique<InstancingGame>(1280, 720);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
