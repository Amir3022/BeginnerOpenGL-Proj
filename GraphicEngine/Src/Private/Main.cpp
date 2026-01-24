#include "GameModules/PostProcessGame.h"

int main()
{
    std::unique_ptr<PostProcessGame> game = std::make_unique<PostProcessGame>(1280, 720);
    if (game)
    {
        return game->Process();
    }
    return -1;
}
