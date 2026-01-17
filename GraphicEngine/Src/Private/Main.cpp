#include "GameModules/BlendingScene.h"

int main()
{
    std::unique_ptr<BlendingGame> game = std::make_unique<BlendingGame>(1280, 720);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
