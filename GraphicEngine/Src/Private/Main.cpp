#include "GameModules/HDRGame.h"

int main()
{
    std::unique_ptr<HDRGame> game = std::make_unique<HDRGame>(1280, 720);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
