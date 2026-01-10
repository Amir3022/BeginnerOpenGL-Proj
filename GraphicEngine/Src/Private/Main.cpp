#include "GameModules/ModelGame.h"

int main()
{
    std::unique_ptr<ModelGame> game = std::make_unique<ModelGame>(1024, 768);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
