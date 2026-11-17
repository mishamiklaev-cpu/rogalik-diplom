#include "GameMenu.h"
#include "Constants.h"

void BuildMenuItems(sf::Text items[], const std::string names[], int count,
                    const sf::Font &font, float yFirst)
{
    for (int i = 0; i < count; i++)
    {
        items[i].setFont(font);
        items[i].setString(names[i]);
        items[i].setCharacterSize(30);
        items[i].setPosition((WINDOW_WIDTH - items[i].getGlobalBounds().width) / 2.0f, yFirst + i * 42.f);
    }
}

// + count, чтобы из 0 попасть на последний пункт, а не в минус
void MoveMenuSelection(int &selected, int count, sf::Keyboard::Key key, GameSounds &sounds)
{
    if (key == sf::Keyboard::Up)
    {
        selected = (selected + count - 1) % count;
        sounds.clickSound.play();
    }
    else if (key == sf::Keyboard::Down)
    {
        selected = (selected + 1) % count;
        sounds.clickSound.play();
    }
    else if (key == sf::Keyboard::Enter)
    {
        sounds.selectSound.play();
    }
}

void DrawMenuItems(sf::RenderWindow &window, sf::Text items[], int count, int selected)
{
    for (int i = 0; i < count; i++)
    {
        if (i == selected)
        {
            items[i].setFillColor(sf::Color(240, 180, 40));
        }
        else
        {
            items[i].setFillColor(sf::Color::White);
        }
        window.draw(items[i]);
    }
}
