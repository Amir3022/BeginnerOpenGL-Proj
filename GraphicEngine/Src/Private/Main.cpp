#include "CubesGame.h"

int main()
{
    std::unique_ptr<CubesGame> game = std::make_unique<CubesGame>(1024, 768);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
