#include "GameModules/AAGame.h"

int main()
{
    std::unique_ptr<AAGame> game = std::make_unique<AAGame>(1280, 720);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
