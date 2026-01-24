#include "GameModules/CubemapGame.h"

int main()
{
    std::unique_ptr<CubemapGame> game = std::make_unique<CubemapGame>(1280, 720);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
