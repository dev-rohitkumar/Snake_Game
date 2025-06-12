#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <stack> // Add this

// Size of each block in the snake and food
const int blockSize = 20;
const int width = 800;
const int height = 600;

struct SnakeSegment {
    int x, y;
    SnakeSegment(int x, int y) : x(x), y(y) {}
};

enum Direction { Up, Down, Left, Right };
enum GameState { MENU, PLAYING, PAUSED, GAMEOVER };

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

// Add wall grid
const int gridCols = width / blockSize;
const int gridRows = height / blockSize;
std::vector<std::vector<bool>> wallGrid(gridRows, std::vector<bool>(gridCols, false));

// Example: Add a border wall
void setupWalls() {
    for (int i = 0; i < gridCols; ++i) {
        wallGrid[0][i] = true;
        wallGrid[gridRows-1][i] = true;
    }
    for (int i = 0; i < gridRows; ++i) {
        wallGrid[i][0] = true;
        wallGrid[i][gridCols-1] = true;
    }
    // You can add more wall patterns here for levels
}

// Modified food generation to avoid walls
sf::Vector2i generateFoodPosition(const Snake& snake) {
    sf::Vector2i pos;
    bool invalid;
    do {
        invalid = false;
        pos.x = rand() % gridCols;
        pos.y = rand() % gridRows;

        // Check wall
        if (wallGrid[pos.y][pos.x]) {
            invalid = true;
            continue;
        }
        // Check snake
        for (const auto& segment : snake.body) {
            if (segment.x == pos.x && segment.y == pos.y) {
                invalid = true;
                break;
            }
        }
    } while (invalid);
    return pos;
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    setupWalls();

    sf::RenderWindow window(sf::VideoMode(width, height), "Snake Game");
    int baseSpeed = 5;

    Snake snake;
    sf::Vector2i food = generateFoodPosition(snake);

    sf::RectangleShape block(sf::Vector2f(blockSize - 2, blockSize - 2));
    block.setFillColor(sf::Color::Green);

    sf::RectangleShape foodBlock(sf::Vector2f(blockSize - 2, blockSize - 2));
    foodBlock.setFillColor(sf::Color::Red);

    sf::RectangleShape wallBlock(sf::Vector2f(blockSize - 2, blockSize - 2));
    wallBlock.setFillColor(sf::Color(100, 100, 100));

    sf::Font font;
    if (!font.loadFromFile("consolas.ttf")) {
        return -1;
    }

    sf::Text menuText;
    menuText.setFont(font);
    menuText.setCharacterSize(40);
    menuText.setFillColor(sf::Color::White);
    menuText.setString("SNAKE GAME\n\nPress ENTER to Start");

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
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::Yellow);

    sf::Text highScoreText;
    highScoreText.setFont(font);
    highScoreText.setCharacterSize(24);
    highScoreText.setFillColor(sf::Color::Green);
    highScoreText.setPosition(10, 40);

    GameState state = MENU;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (state == MENU && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                // Start game
                snake = Snake();
                food = generateFoodPosition(snake);
                score = 0;
                state = PLAYING;
            }
            else if (state == PLAYING && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up && snake.dir != Down) snake.dir = Up;
                else if (event.key.code == sf::Keyboard::Down && snake.dir != Up) snake.dir = Down;
                else if (event.key.code == sf::Keyboard::Left && snake.dir != Right) snake.dir = Left;
                else if (event.key.code == sf::Keyboard::Right && snake.dir != Left) snake.dir = Right;
                else if (event.key.code == sf::Keyboard::Space) state = PAUSED;
            }
            else if (state == PAUSED && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                state = PLAYING;
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
            if (nextHead.x < 0 || nextHead.y < 0 || nextHead.x >= gridCols || nextHead.y >= gridRows || wallGrid[nextHead.y][nextHead.x]) {
                if (score > highScore) highScore = score;
                scoreHistory.push(score);
                state = GAMEOVER;
            } else {
                snake.move();
                if (snake.body.front().x == food.x && snake.body.front().y == food.y) {
                    snake.grow();
                    food = generateFoodPosition(snake);
                    score++;
                }
                if (snake.checkCollision()) {
                    if (score > highScore) highScore = score;
                    scoreHistory.push(score);
                    state = GAMEOVER;
                }
            }
        }

        window.clear();

        if (state == MENU) {
            menuText.setPosition(width / 2.f - 200, height / 2.f - 100);
            window.draw(menuText);
        }
        else if (state == PLAYING || state == PAUSED || state == GAMEOVER) {
            // Draw walls
            for (int y = 0; y < gridRows; ++y) {
                for (int x = 0; x < gridCols; ++x) {
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
            foodBlock.setPosition(food.x * blockSize, food.y * blockSize);
            window.draw(foodBlock);
            // Draw score
            scoreText.setString("Score: " + std::to_string(score));
            scoreText.setPosition(10, 10);
            window.draw(scoreText);
            // Draw high score
            highScoreText.setString("High Score: " + std::to_string(highScore));
            window.draw(highScoreText);
        }

        if (state == PAUSED) {
            pauseText.setPosition(width / 2.f - 100, height / 2.f - 50);
            window.draw(pauseText);
        }

        if (state == GAMEOVER) {
            gameOverText.setPosition(width / 2.f - 180, height / 2.f - 50);
            window.draw(gameOverText);

            // Show last 5 scores
            sf::Text histText;
            histText.setFont(font);
            histText.setCharacterSize(20);
            histText.setFillColor(sf::Color::Cyan);
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
