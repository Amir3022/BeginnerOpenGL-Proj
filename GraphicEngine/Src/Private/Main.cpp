#include "GameModules/BloomGame.h"

int main()
{
    std::unique_ptr<BloomGame> game = std::make_unique<BloomGame>(1280, 720);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
