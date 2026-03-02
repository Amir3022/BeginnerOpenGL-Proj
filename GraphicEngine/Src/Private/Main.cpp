#include "GameModules/NormalMapGame.h"

int main()
{
    std::unique_ptr<NormalMapGame> game = std::make_unique<NormalMapGame>(1280, 720);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
