#include "GameModules/AsteroidsGame.h"

int main()
{
    std::unique_ptr<AsteroidsGame> game = std::make_unique<AsteroidsGame>(1280, 720);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
