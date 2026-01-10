#include "GameModules/ModelGame.h"

int main()
{
    std::unique_ptr<ModelGame> game = std::make_unique<ModelGame>(1280, 720);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
