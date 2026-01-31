#include "GameModules/GLSLGame.h"

int main()
{
    std::unique_ptr<GLSLGame> game = std::make_unique<GLSLGame>(1280, 720);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
