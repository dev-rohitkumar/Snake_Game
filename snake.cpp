#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

// Size of each block in the snake and food
const int blockSize = 20;
const int width = 800;
const int height = 600;

struct SnakeSegment {
    int x, y;
    SnakeSegment(int x, int y) : x(x), y(y) {}
};

enum Direction { Up, Down, Left, Right };

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

sf::Vector2i generateFoodPosition(const Snake& snake) {
    sf::Vector2i pos;
    bool onSnake;
    do {
        onSnake = false;
        pos.x = rand() % (width / blockSize);
        pos.y = rand() % (height / blockSize);

        for (const auto& segment : snake.body) {
            if (segment.x == pos.x && segment.y == pos.y) {
                onSnake = true;
                break;
            }
        }
    } while (onSnake);
    return pos;
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(width, height), "Snake Game");
    window.setFramerateLimit(10);

    Snake snake;
    sf::Vector2i food = generateFoodPosition(snake);

    sf::RectangleShape block(sf::Vector2f(blockSize - 2, blockSize - 2));
    block.setFillColor(sf::Color::Green);

    sf::RectangleShape foodBlock(sf::Vector2f(blockSize - 2, blockSize - 2));
    foodBlock.setFillColor(sf::Color::Red);

    sf::Font font;
    if (!font.loadFromFile("D:/Mastering_in_DSA/Game/SFML-2.6.1/examples/resources/sansation.ttf")) {
        return -1;
    }

    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(40);
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setString("Game Over!\nPress Enter to Restart");

    bool gameOver = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (!gameOver && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up && snake.dir != Down) snake.dir = Up;
                else if (event.key.code == sf::Keyboard::Down && snake.dir != Up) snake.dir = Down;
                else if (event.key.code == sf::Keyboard::Left && snake.dir != Right) snake.dir = Left;
                else if (event.key.code == sf::Keyboard::Right && snake.dir != Left) snake.dir = Right;
            }
            if (gameOver && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                snake = Snake();
                food = generateFoodPosition(snake);
                gameOver = false;
            }
        }

        if (!gameOver) {
            snake.move();

            if (snake.body.front().x == food.x && snake.body.front().y == food.y) {
                snake.grow();
                food = generateFoodPosition(snake);
            }

            if (snake.checkCollision()) {
                gameOver = true;
            }
        }

        window.clear();

        // Draw snake
        for (const auto& segment : snake.body) {
            block.setPosition(segment.x * blockSize, segment.y * blockSize);
            window.draw(block);
        }

        // Draw food
        foodBlock.setPosition(food.x * blockSize, food.y * blockSize);
        window.draw(foodBlock);

        if (gameOver) {
            gameOverText.setPosition(width / 2.f - 180, height / 2.f - 50);
            window.draw(gameOverText);
        }

        window.display();
    }

    return 0;
}
