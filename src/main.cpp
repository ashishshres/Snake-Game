#include <raylib.h>
#include <deque>
#include <raymath.h>

const int blockSize{20};
const int blockCount{25};
const char title[]{"Esnake"};
const int padding{50};
double lastUpdateTime{0};

bool slowDown(double interval)
{
  double currentUpdateTime{GetTime()};
  if (currentUpdateTime - lastUpdateTime >= interval)
  {
    lastUpdateTime = currentUpdateTime;
    return true;
  }
  return false;
}

bool elementInDeque(Vector2 elem, std::deque<Vector2> deque)
{
  for (unsigned int i = 0; i < deque.size(); i++)
  {
    if (Vector2Equals(deque[i], elem))
    {
      return true;
    }
  }
  return false;
}

bool isPaused{false};

class Food
{
public:
  Vector2 position;
  Texture2D texture;

  Food(std::deque<Vector2> snakeBody)
  {
    Image image = LoadImage("apple.png");
    texture = LoadTextureFromImage(image);
    texture.height = {blockSize};
    texture.width = {blockSize};
    UnloadImage(image);
    position = randomPosition(snakeBody);
  }

  void draw()
  {
    // DrawRectangle(padding + position.x * blockSize, padding + position.y * blockSize, blockSize, blockSize, RED);
    DrawTexture(texture, padding + position.x * blockSize, padding + position.y * blockSize, WHITE);
  }

  Vector2 randomBlock()
  {
    float x = GetRandomValue(0, blockCount - 1);
    float y = GetRandomValue(0, blockCount - 1);
    return Vector2{x, y};
  }

  Vector2 randomPosition(std::deque<Vector2> snakeBody)
  {

    Vector2 position{randomBlock()};
    while (elementInDeque(position, snakeBody))
    {
      position = randomBlock();
    }
    return position;
  }

  ~Food()
  {
    UnloadTexture(texture);
  }
};

class Snake
{
public:
  Vector2 direction{1, 0};
  std::deque<Vector2> snakeBody = {Vector2{6, 5}, Vector2{5, 5}, Vector2{4, 5}};
  bool growSnake{false};

  void draw()
  {
    for (unsigned int i = 0; i < snakeBody.size(); i++)
    {
      float x = snakeBody[i].x;
      float y = snakeBody[i].y;
      if (i == 0)
      {
        DrawRectangle(padding + x * blockSize, padding + y * blockSize, blockSize, blockSize, DARKBLUE);
      }
      else
      {
        DrawRectangle(padding + x * blockSize, padding + y * blockSize, blockSize, blockSize, BLUE);
      }
    }
  }

  void moveSnake()
  {
    snakeBody.push_front(Vector2Add(snakeBody[0], direction));
    if (growSnake == true)
    {
      growSnake = false;
    }
    else
    {
      snakeBody.pop_back();
    }
  }

  void reset()
  {
    snakeBody = {Vector2{6, 5}, Vector2{5, 5}, Vector2{4, 5}};
    direction = {1, 0};
  }
};

class InitGame
{
public:
  Snake snake;
  Food food = Food(snake.snakeBody);
  bool isRunning = true;
  int score{0};
  void draw()
  {
    food.draw();
    snake.draw();
  }

  void update()
  {
    if (isRunning)
    {
      snake.moveSnake();
      checkFoodCollision();
      checkEdgeCollision();
      checkTailCollision();
    }
  }

  void checkFoodCollision()
  {
    if (Vector2Equals(snake.snakeBody[0], food.position))
    {
      food.position = food.randomPosition(snake.snakeBody);
      snake.growSnake = true;
      score += 5;
    }
  }

  void checkEdgeCollision()
  {
    if (snake.snakeBody[0].x == blockCount || snake.snakeBody[0].x == -1)
    {
      gameOver();
    }
    if (snake.snakeBody[0].y == blockCount || snake.snakeBody[0].y == -1)
    {
      gameOver();
    }
  }

  void checkTailCollision()
  {
    std::deque<Vector2> headlessBody = snake.snakeBody;
    headlessBody.pop_front();
    if (elementInDeque(snake.snakeBody[0], headlessBody))
    {
      gameOver();
    }
  }

  void gameOver()
  {
    snake.reset();
    food.position = food.randomPosition(snake.snakeBody);
    isRunning = false;
    score = 0;
  }
};

int main()
{

  InitWindow(2 * padding + blockSize * blockCount, 2 * padding + blockSize * blockCount, title);
  InitGame game;
  SetTargetFPS(60);

  while (!WindowShouldClose())
  {
    if (IsKeyPressed(KEY_SPACE))
    {
      isPaused = !isPaused;
    }

    if (isPaused)
    {
      DrawText("Paused", 210, 245, 50, DARKGREEN);
    }

    BeginDrawing();
    if (!isPaused)
    {
      ClearBackground(WHITE);
      if (slowDown(0.13))
      {
        game.update();
      }

      if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
      {
        game.snake.direction = {0, -1};
        game.isRunning = true;
      }

      if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
      {
        game.snake.direction = {0, 1};
        game.isRunning = true;
      }

      if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
      {
        game.snake.direction = {-1, 0};
        game.isRunning = true;
      }

      if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
      {
        game.snake.direction = {1, 0};
        game.isRunning = true;
      }
    }

    DrawRectangleLinesEx(Rectangle{(float)padding - 5, (float)padding - 5, (float)blockSize * (float)blockCount + 10, (float)blockSize * (float)blockCount + 10}, 5, SKYBLUE);
    DrawText("Esnake", padding, 10, 30, BLACK);
    DrawText(TextFormat("Score: %i", game.score), padding + 360, 10, 30, BLACK);
    game.draw();
    EndDrawing();
  }

  CloseWindow();
  return 0;
}