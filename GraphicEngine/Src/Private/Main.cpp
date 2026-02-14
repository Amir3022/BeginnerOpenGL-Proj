#include "GameModules/ShadowGame.h"

int main()
{
    std::unique_ptr<ShadowGame> game = std::make_unique<ShadowGame>(1280, 720);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
