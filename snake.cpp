#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <stack>

const int blockSize = 20;
const int width = 800;
const int height = 600;

// --- Snake segment struct ---
struct SnakeSegment {
    int x, y;
    SnakeSegment(int x, int y) : x(x), y(y) {}
};

enum Direction { Up, Down, Left, Right };
enum GameState { MENU, PLAYING, PAUSED, GAMEOVER, HIGHSCORES };

// --- Snake class ---
class Snake {
public:
    std::vector<SnakeSegment> body;
    Direction dir;

    Snake() {
        body.push_back(SnakeSegment(width / (2 * blockSize), height / (2 * blockSize)));
        dir = Right;
    }

    void move() {
        SnakeSegment head = body.front();
        switch (dir) {
            case Up: head.y--; break;
            case Down: head.y++; break;
            case Left: head.x--; break;
            case Right: head.x++; break;
        }
        body.insert(body.begin(), head);
        body.pop_back();
    }

    void grow() {
        SnakeSegment head = body.front();
        switch (dir) {
            case Up: head.y--; break;
            case Down: head.y++; break;
            case Left: head.x--; break;
            case Right: head.x++; break;
        }
        body.insert(body.begin(), head);
    }

    bool checkCollision() {
        SnakeSegment head = body.front();
        if (head.x < 0 || head.y < 0 || head.x * blockSize >= width || head.y * blockSize >= height)
            return true;
        for (size_t i = 1; i < body.size(); i++) {
            if (body[i].x == head.x && body[i].y == head.y)
                return true;
        }
        return false;
    }
};

// --- Wall grid and setup ---
const int gridCols = width / blockSize;
const int gridRows = height / blockSize;
std::vector<std::vector<bool>> wallGrid(gridRows, std::vector<bool>(gridCols, false));

void setupWalls() {
    for (int i = 0; i < gridCols; ++i) {
        wallGrid[0][i] = true;
        wallGrid[gridRows-1][i] = true;
    }
    for (int i = 0; i < gridRows; ++i) {
        wallGrid[i][0] = true;
        wallGrid[i][gridCols-1] = true;
    }
}

// --- Food generation ---
sf::Vector2i generateFoodPosition(const Snake& snake) {
    sf::Vector2i pos;
    bool invalid;
    do {
        invalid = false;
        pos.x = rand() % gridCols;
        pos.y = rand() % gridRows;
        if (wallGrid[pos.y][pos.x]) {
            invalid = true;
            continue;
        }
        for (const auto& segment : snake.body) {
            if (segment.x == pos.x && segment.y == pos.y) {
                invalid = true;
                break;
            }
        }
    } while (invalid);
    return pos;
}

// --- Button hover helper ---
bool isMouseOver(const sf::RectangleShape& rect, sf::Vector2f mousePos) {
    return rect.getGlobalBounds().contains(mousePos);
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    setupWalls();

    sf::RenderWindow window(sf::VideoMode(width, height), "Serpent Rush");
    int baseSpeed = 5;
    bool musicOn = true;
    bool canContinue = false;

    // --- Font ---
    sf::Font font;
    if (!font.loadFromFile("consolas.ttf")) return -1;

    // --- Music ---
    sf::Music music;
    if (music.openFromFile("../resources/background.ogg")) {
        music.setLoop(true);
        music.play();
    }

    // --- Menu Buttons ---
    sf::RectangleShape startBtn(sf::Vector2f(300, 60));
    startBtn.setFillColor(sf::Color(0, 150, 0));
    startBtn.setOrigin(150, 30);
    startBtn.setPosition(width / 2.f, height / 2.f);

    sf::Text startBtnText("Start Game", font, 32);
    startBtnText.setFillColor(sf::Color::White);
    sf::FloatRect sbt = startBtnText.getLocalBounds();
    startBtnText.setOrigin(sbt.left + sbt.width / 2, sbt.top + sbt.height / 2);
    startBtnText.setPosition(width / 2.f, height / 2.f);

    sf::RectangleShape continueBtn(sf::Vector2f(300, 60));
    continueBtn.setFillColor(sf::Color(200, 150, 0));
    continueBtn.setOrigin(150, 30);
    continueBtn.setPosition(width / 2.f, height / 2.f + 80);

    sf::Text continueBtnText("Continue", font, 32);
    continueBtnText.setFillColor(sf::Color::White);
    sf::FloatRect cbt = continueBtnText.getLocalBounds();
    continueBtnText.setOrigin(cbt.left + cbt.width / 2, cbt.top + cbt.height / 2);
    continueBtnText.setPosition(width / 2.f, height / 2.f + 80);

    sf::RectangleShape highBtn(sf::Vector2f(300, 60));
    highBtn.setFillColor(sf::Color(0, 100, 200));
    highBtn.setOrigin(150, 30);
    highBtn.setPosition(width / 2.f, height / 2.f + 170);

    sf::Text highBtnText("High Scores", font, 32);
    highBtnText.setFillColor(sf::Color::White);
    sf::FloatRect hbt = highBtnText.getLocalBounds();
    highBtnText.setOrigin(hbt.left + hbt.width / 2, hbt.top + hbt.height / 2);
    highBtnText.setPosition(width / 2.f, height / 2.f + 170);

    sf::RectangleShape musicBtn(sf::Vector2f(200, 50));
    musicBtn.setFillColor(sf::Color(100, 100, 100));
    musicBtn.setOrigin(100, 25);
    musicBtn.setPosition(width / 2.f, height - 80);

    sf::Text musicBtnText("Music: ON", font, 28);
    musicBtnText.setFillColor(sf::Color::White);
    sf::FloatRect mbt = musicBtnText.getLocalBounds();
    musicBtnText.setOrigin(mbt.left + mbt.width / 2, mbt.top + mbt.height / 2);
    musicBtnText.setPosition(width / 2.f, height - 80);

    // --- High Score Window ---
    sf::Text highTitle("High Scores", font, 40);
    highTitle.setFillColor(sf::Color::White);
    sf::FloatRect ht = highTitle.getLocalBounds();
    highTitle.setOrigin(ht.left + ht.width / 2, ht.top + ht.height / 2);
    highTitle.setPosition(width / 2.f, 80);

    sf::RectangleShape backBtn(sf::Vector2f(200, 50));
    backBtn.setFillColor(sf::Color(150, 0, 0));
    backBtn.setOrigin(100, 25);
    backBtn.setPosition(width / 2.f, height - 100);

    sf::Text backBtnText("Back", font, 28);
    backBtnText.setFillColor(sf::Color::White);
    sf::FloatRect bbt = backBtnText.getLocalBounds();
    backBtnText.setOrigin(bbt.left + bbt.width / 2, bbt.top + bbt.height / 2);
    backBtnText.setPosition(width / 2.f, height - 100);

    // --- Menu Title ---
    sf::Text menuTitle;
    menuTitle.setFont(font);
    menuTitle.setCharacterSize(48);
    menuTitle.setFillColor(sf::Color::White);
    menuTitle.setString("SERPENT RUSH");

    // --- Pause/Game Over/Menu Texts ---
    sf::Text pauseText;
    pauseText.setFont(font);
    pauseText.setCharacterSize(40);
    pauseText.setFillColor(sf::Color::Yellow);
    pauseText.setString("PAUSED\nPress Space to Resume");

    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(40);
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setString("Game Over!\nPress Enter to Restart");

    int score = 0;
    int highScore = 0;
    std::stack<int> scoreHistory;
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(28);
    scoreText.setFillColor(sf::Color::Black);

    GameState state = MENU;

    // --- Sound: food spawn ---
    sf::SoundBuffer spawnBuffer;
    sf::Sound spawnSound;
    if (spawnBuffer.loadFromFile("../resources/spawn.wav")) {
        spawnSound.setBuffer(spawnBuffer);
    }

    // --- Sound: eating ---
    sf::SoundBuffer eatBuffer;
    sf::Sound eatSound;
    if (eatBuffer.loadFromFile("../resources/eat.wav")) {
        eatSound.setBuffer(eatBuffer);
    }

    // --- Apple image ---
    sf::Texture appleTexture;
    bool appleImgLoaded = appleTexture.loadFromFile("../resources/apple.png");
    sf::Sprite appleSprite;
    if (appleImgLoaded) {
        appleSprite.setTexture(appleTexture);
        appleSprite.setScale(
            (blockSize - 2) / static_cast<float>(appleTexture.getSize().x),
            (blockSize - 2) / static_cast<float>(appleTexture.getSize().y)
        );
    }

 

    Snake snake;
    sf::Vector2i food = generateFoodPosition(snake);

    sf::RectangleShape block(sf::Vector2f(blockSize - 2, blockSize - 2));
    block.setFillColor(sf::Color(0, 100, 0)); // Dark green

    sf::RectangleShape foodBlock(sf::Vector2f(blockSize - 2, blockSize - 2));
    foodBlock.setFillColor(sf::Color::Red);

    sf::RectangleShape wallBlock(sf::Vector2f(blockSize - 2, blockSize - 2));
    wallBlock.setFillColor(sf::Color(100, 100, 100));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            if (state == MENU || state == PAUSED) {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    if (isMouseOver(startBtn, mousePos)) {
                        snake = Snake();
                        food = generateFoodPosition(snake);
                        score = 0;
                        state = PLAYING;
                        canContinue = true;
                        spawnSound.play();
                    } else if (canContinue && isMouseOver(continueBtn, mousePos)) {
                        state = PLAYING;
                    } else if (isMouseOver(highBtn, mousePos)) {
                        state = HIGHSCORES;
                    } else if (isMouseOver(musicBtn, mousePos)) {
                        musicOn = !musicOn;
                        if (musicOn) {
                            music.play();
                            musicBtnText.setString("Music: ON");
                        } else {
                            music.pause();
                            musicBtnText.setString("Music: OFF");
                        }
                        sf::FloatRect mbt2 = musicBtnText.getLocalBounds();
                        musicBtnText.setOrigin(mbt2.left + mbt2.width / 2, mbt2.top + mbt2.height / 2);
                    }
                }
            }
            else if (state == HIGHSCORES) {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    if (isMouseOver(backBtn, mousePos)) {
                        state = MENU;
                    }
                }
            }
            else if (state == PLAYING && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up && snake.dir != Down) snake.dir = Up;
                else if (event.key.code == sf::Keyboard::Down && snake.dir != Up) snake.dir = Down;
                else if (event.key.code == sf::Keyboard::Left && snake.dir != Right) snake.dir = Left;
                else if (event.key.code == sf::Keyboard::Right && snake.dir != Left) snake.dir = Right;
                else if (event.key.code == sf::Keyboard::Space) {
                    state = PAUSED;
                }
            }
            else if (state == GAMEOVER && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                state = MENU;
            }
        }

        if (state == PLAYING) {
            int newSpeed = baseSpeed + score / 5;
            window.setFramerateLimit(newSpeed);

            SnakeSegment nextHead = snake.body.front();
            switch (snake.dir) {
                case Up: nextHead.y--; break;
                case Down: nextHead.y++; break;
                case Left: nextHead.x--; break;
                case Right: nextHead.x++; break;
            }
            if (nextHead.x < 0 || nextHead.y < 0 || nextHead.x >= width / blockSize || nextHead.y >= height / blockSize || wallGrid[nextHead.y][nextHead.x]) {
                if (score > highScore) highScore = score;
                scoreHistory.push(score);
                state = GAMEOVER;
            } else {
                snake.move();
                if (snake.body.front().x == food.x && snake.body.front().y == food.y) {
                    snake.grow();
                    food = generateFoodPosition(snake);
                    score++;
                    eatSound.play();
                    spawnSound.play();
                }
                if (snake.checkCollision()) {
                    if (score > highScore) highScore = score;
                    scoreHistory.push(score);
                    state = GAMEOVER;
                }
            }
        }

        window.clear(sf::Color(200, 200, 200)); // Light gray background

        if (state == MENU || state == PAUSED) {
            // Centered title
            sf::FloatRect titleRect = menuTitle.getLocalBounds();
            menuTitle.setOrigin(titleRect.left + titleRect.width / 2.0f, titleRect.top + titleRect.height / 2.0f);
            menuTitle.setPosition(width / 2.0f, height / 2.0f - 120);
            window.draw(menuTitle);

            window.draw(startBtn);
            window.draw(startBtnText);

            if (canContinue) {
                window.draw(continueBtn);
                window.draw(continueBtnText);
            }

            window.draw(highBtn);
            window.draw(highBtnText);

            window.draw(musicBtn);
            window.draw(musicBtnText);
        }
        else if (state == HIGHSCORES) {
            window.draw(highTitle);

            // Show last 5 scores
            sf::Text histText;
            histText.setFont(font);
            histText.setCharacterSize(32);
            histText.setFillColor(sf::Color::Black);
            histText.setStyle(sf::Text::Bold);
            histText.setPosition(width / 2.f - 80, 160);

            std::stack<int> temp = scoreHistory;
            std::string histStr = "";
            int count = 0;
            while (!temp.empty() && count < 5) {
                histStr += std::to_string(temp.top()) + "\n";
                temp.pop();
                count++;
            }
            if (histStr.empty()) histStr = "No scores yet!";
            histText.setString(histStr);
            window.draw(histText);

            window.draw(backBtn);
            window.draw(backBtnText);
        }
        else if (state == PLAYING || state == GAMEOVER) {
            // Draw walls
            for (int y = 0; y < height / blockSize; ++y) {
                for (int x = 0; x < width / blockSize; ++x) {
                    if (wallGrid[y][x]) {
                        wallBlock.setPosition(x * blockSize, y * blockSize);
                        window.draw(wallBlock);
                    }
                }
            }
            // Draw snake
            for (const auto& segment : snake.body) {
    block.setPosition(segment.x * blockSize, segment.y * blockSize);
    window.draw(block);
}
            // Draw food
            if (appleImgLoaded) {
                appleSprite.setPosition(food.x * blockSize, food.y * blockSize);
                window.draw(appleSprite);
            } else {
                foodBlock.setPosition(food.x * blockSize, food.y * blockSize);
                window.draw(foodBlock);
            }

            // --- Centered score and high score with boundary ---
            std::string scoreStr = "Score: " + std::to_string(score) + "   High Score: " + std::to_string(highScore);
            scoreText.setString(scoreStr);

            sf::FloatRect textRect = scoreText.getLocalBounds();
            scoreText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
            scoreText.setPosition(width / 2.0f, 20);

            sf::RectangleShape scoreBox;
            scoreBox.setSize(sf::Vector2f(textRect.width + 40, textRect.height + 20));
            scoreBox.setFillColor(sf::Color(255, 255, 255, 180));
            scoreBox.setOutlineColor(sf::Color::Black);
            scoreBox.setOutlineThickness(2);
            scoreBox.setOrigin(scoreBox.getSize().x / 2.0f, scoreBox.getSize().y / 2.0f);
            scoreBox.setPosition(width / 2.0f, 20 + textRect.height / 2.0f);

            window.draw(scoreBox);
            window.draw(scoreText);
        }

        if (state == GAMEOVER) {
            sf::FloatRect overRect = gameOverText.getLocalBounds();
            gameOverText.setOrigin(overRect.left + overRect.width / 2.0f, overRect.top + overRect.height / 2.0f);
            gameOverText.setPosition(width / 2.0f, height / 2.0f - 40);
            window.draw(gameOverText);

            // Show last 5 scores
            sf::Text histText;
            histText.setFont(font);
            histText.setCharacterSize(20);
            histText.setFillColor(sf::Color::Black); // Changed to black
            histText.setPosition(width / 2.f - 100, height / 2.f + 50);

            std::stack<int> temp = scoreHistory;
            std::string histStr = "Last Scores:\n";
            int count = 0;
            while (!temp.empty() && count < 5) {
                histStr += std::to_string(temp.top()) + "\n";
                temp.pop();
                count++;
            }
            histText.setString(histStr);
            window.draw(histText);
        }

        window.display();
    }

    return 0;
}
