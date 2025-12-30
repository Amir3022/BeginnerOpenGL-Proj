#include "Game.h"

int main()
{
    std::unique_ptr<Game> game = std::make_unique<Game>(1024, 768);
    if (game)
    {
        return game->Process();
    }
    return 0;
}
