#include "GameModules/GSModelGame.h"

int main()
{
    std::unique_ptr<GSModelGame> game = std::make_unique<GSModelGame>(1280, 720);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
