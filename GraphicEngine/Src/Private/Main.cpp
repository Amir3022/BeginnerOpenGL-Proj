#include "GameModules/AdvLightGame.h"

int main()
{
    std::unique_ptr<AdvLightGame> game = std::make_unique<AdvLightGame>(1280, 720);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
