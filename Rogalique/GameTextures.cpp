#include "GameTextures.h"
#include <string>

// как и шрифт, файл ищем в двух местах: рабочая папка бывает и Rogalique/, и Resources/
bool LoadTextureFile(sf::Texture &texture, const std::string &fileName)
{
    if (texture.loadFromFile("Textures/" + fileName))
    {
        return true;
    }
    return texture.loadFromFile("Resources/Textures/" + fileName);
}

void GameTextures::LoadTextures()
{
    wallLoaded = LoadTextureFile(wallTexture, "wall.png");
    floorLoaded = LoadTextureFile(floorTexture, "floor.png");
    doorLoaded = LoadTextureFile(doorTexture, "door.png");
    potionLoaded = LoadTextureFile(potionTexture, "potion.png");
    playerLoaded = LoadTextureFile(playerTexture, "player.png");
    shatunLoaded = LoadTextureFile(shatunTexture, "shatun.png");
}
