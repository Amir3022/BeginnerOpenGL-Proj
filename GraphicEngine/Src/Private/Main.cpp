#include "GameModules/DepthTestGame.h"

int main()
{
    std::unique_ptr<DepthTestGame> game = std::make_unique<DepthTestGame>(1280, 720);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
