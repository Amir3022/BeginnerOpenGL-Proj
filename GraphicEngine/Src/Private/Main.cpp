#include "GameModules/ParallaxMapGame.h"

int main()
{
    std::unique_ptr<ParallaxMapGame> game = std::make_unique<ParallaxMapGame>(1280, 720);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
