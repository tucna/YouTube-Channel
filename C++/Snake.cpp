#include <iostream>  // For input/output operations
#include <conio.h>    // For _kbhit() and _getch() to detect keyboard input
#include <windows.h>  // For Sleep() function

// Global variables to store the game state
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN }; // Enumeration to handle direction

eDirection dir;  // Variable to store current direction
bool gameOver;   // Flag to indicate if the game is over
const int width = 20;   // Width of the game board
const int height = 20;  // Height of the game board
int x, y;       // Position of the snake's head
int fruitX, fruitY;  // Position of the fruit
int score;      // Player's score
int tailX[100], tailY[100];  // Arrays to store the positions of the snake's tail
int nTail;      // Length of the snake's tail
HANDLE hConsole;  // Handle to the console for cursor control


// Function to setup the initial game state
void Setup()
{
  gameOver = false;  // Game starts as not over
  dir = STOP;  // Snake starts stationary
  x = width / 2;  // Start the snake in the middle of the game board
  y = height / 2;
  fruitX = rand() % width;  // Place the fruit at a random position
  fruitY = rand() % height;
  score = 0;  // Initial score is 0
  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get the console handle for later use
}

// Function to move the cursor to a specific position in the console
void GotoXY(int x, int y)
{
  COORD coord;
  coord.X = x;
  coord.Y = y;
  SetConsoleCursorPosition(hConsole, coord);
}

void Draw()
{
  GotoXY(0,0);

  // Draw the top border
  for (int i = 0; i < width + 2; i++)
    std::cout << "#";
  std::cout << std::endl;

  // Draw the game board
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      if (j == 0)
        std::cout << "#";  // Left border

      if (i == y && j == x)
        std::cout << "O";  // Draw the snake's head
      else if (i == fruitY && j == fruitX)
        std::cout << "F";  // Draw the fruit
      else
      {
        bool printTail = false;
        for (int k = 0; k < nTail; k++)
        {
          if (tailX[k] == j && tailY[k] == i)
          {
            std::cout << "o";  // Draw the snake's tail
            printTail = true;
          }
        }
        if (!printTail)
          std::cout << " ";  // Empty space
      }

      if (j == width - 1)
        std::cout << "#";  // Right border
    }
    std::cout << std::endl;
  }

  // Draw the bottom border
  for (int i = 0; i < width + 2; i++)
    std::cout << "#";
  std::cout << std::endl;

  // Display the score
  std::cout << "Score: " << score << std::endl;
}

void Input()
{
  if (_kbhit()) // Check if a key was pressed
  {
    switch (_getch())
    {
    case 'a':
      dir = LEFT;
      break;
    case 'd':
      dir = RIGHT;
      break;
    case 'w':
      dir = UP;
      break;
    case 's':
      dir = DOWN;
      break;
    case 'x':
      gameOver = true;
      break;
    }
  }
}

void Logic()
{
  // Store the previous position of the head
  int prevX = tailX[0];
  int prevY = tailY[0];
  int prev2X, prev2Y;
  tailX[0] = x;
  tailY[0] = y;

  // Move the tail
  for (int i = 1; i < nTail; i++)
  {
    prev2X = tailX[i];
    prev2Y = tailY[i];
    tailX[i] = prevX;
    tailY[i] = prevY;
    prevX = prev2X;
    prevY = prev2Y;
  }

  // Update the position of the head
  switch (dir)
  {
  case LEFT:
    x--;
    break;
  case RIGHT:
    x++;
    break;
  case UP:
    y--;
    break;
  case DOWN:
    y++;
    break;
  default:
    break;
  }

  // Check if the snake hits the wall
  if (x >= width) x = 0; else if (x < 0) x = width - 1;
  if (y >= height) y = 0; else if (y < 0) y = height - 1;

  // Check if the snake hits its tail
  for (int i = 0; i < nTail; i++)
    if (tailX[i] == x && tailY[i] == y)
      gameOver = true;

  // Check if the snake eats the fruit
  if (x == fruitX && y == fruitY)
  {
    score += 10;  // Increase score
    fruitX = rand() % width;  // Place a new fruit
    fruitY = rand() % height;
    nTail++;  // Increase the length of the snake's tail
  }
}

int main()
{
  Setup();

  while(!gameOver)
  {
    Draw();
    Input();
    Logic();

    Sleep(100);
  }
  
  return 0;
}