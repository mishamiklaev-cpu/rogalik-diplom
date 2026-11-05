#pragma once
#include <SFML/Audio.hpp>
#include <string>

// вся музыка и звуки игры: загружаются один раз при старте.
// Если файла нет, игра просто молчит: пустые Sound и Music при play() ничего не делают
class GameSounds
{
  public:
    // Music читает файл с диска по кусочкам: длинный трек не занимает память целиком
    sf::Music menuMusic;
    sf::Music gameMusic;

    // SoundBuffer хранит звук в памяти, а Sound его проигрывает.
    // Буферы объявлены раньше: удаляются позже звуков, которые на них ссылаются
    sf::SoundBuffer clickBuffer;
    sf::SoundBuffer selectBuffer;
    sf::SoundBuffer potionBuffer;
    sf::SoundBuffer doorBuffer;
    sf::SoundBuffer gameOverBuffer;
    sf::SoundBuffer damageBuffer;

    sf::Sound clickSound;
    sf::Sound selectSound;
    sf::Sound potionSound;
    sf::Sound doorSound;
    sf::Sound gameOverSound;
    sf::Sound damageSound;

    void LoadSounds()
    {
        LoadMusic(menuMusic, "menu.wav");
        LoadMusic(gameMusic, "game.wav");
        LoadSound(clickBuffer, clickSound, "click.wav");
        LoadSound(selectBuffer, selectSound, "click2.wav");
        LoadSound(potionBuffer, potionSound, "potion.wav");
        LoadSound(doorBuffer, doorSound, "door.wav");
        LoadSound(gameOverBuffer, gameOverSound, "gameOver.wav");
        LoadSound(damageBuffer, damageSound, "damage.wav");
    }

  private:
    // как текстуры и шрифт: рабочая папка бывает и Rogalique/, и Resources/
    void LoadMusic(sf::Music &music, const std::string &fileName)
    {
        if (!music.openFromFile("Sounds/" + fileName))
        {
            music.openFromFile("Resources/Sounds/" + fileName);
        }
        music.setLoop(true); // трек кончился — начинается заново
    }

    void LoadSound(sf::SoundBuffer &buffer, sf::Sound &sound, const std::string &fileName)
    {
        if (!buffer.loadFromFile("Sounds/" + fileName))
        {
            buffer.loadFromFile("Resources/Sounds/" + fileName);
        }
        sound.setBuffer(buffer);
    }
};
