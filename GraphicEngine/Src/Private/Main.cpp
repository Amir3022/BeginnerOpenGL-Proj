#include "GameModules/PointShadowGame.h"

int main()
{
    std::unique_ptr<PointShadowGame> game = std::make_unique<PointShadowGame>(1280, 720);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
