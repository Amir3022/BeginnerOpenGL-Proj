#include "GameModules/DeferredLightingGame.h"

int main()
{
    std::unique_ptr<DeferredLightingGame> game = std::make_unique<DeferredLightingGame>(1280, 720);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
