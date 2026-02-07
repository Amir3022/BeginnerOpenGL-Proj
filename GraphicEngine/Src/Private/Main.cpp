#include "GameModules/GSGame.h"

int main()
{
    std::unique_ptr<GSGame> game = std::make_unique<GSGame>(1280, 720);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
