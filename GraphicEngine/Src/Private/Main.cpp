#include "CubesGame.h"
#include "LightGame.h"

int main()
{
    std::unique_ptr<LightGame> game = std::make_unique<LightGame>(1024, 768);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
