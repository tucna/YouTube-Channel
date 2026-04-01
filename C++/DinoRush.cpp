// OS and Input
#include <windows.h>
#include <conio.h>

// Containers and Data
#include <vector>
#include <string>
#include <span>

// Utilities
#include <memory>    // std::unique_ptr
#include <algorithm> // std::ranges, std::fill
#include <random>    // std::mt19937
#include <chrono>    // time units
#include <thread>    // std::this_thread

using namespace std::chrono_literals;

// =========================================================
// CONFIG
// =========================================================
constexpr int SCREEN_W = 120; // Increased width to fill window border
constexpr int SCREEN_H = 30;
constexpr int GROUND_Y = SCREEN_H - 4;
constexpr int FPS = 30;

// Colors
constexpr WORD COL_BG = 0;
constexpr WORD COL_GROUND = FOREGROUND_GREEN;
constexpr WORD COL_JEEP = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; // White/Silver
constexpr WORD COL_DINO = FOREGROUND_GREEN | FOREGROUND_INTENSITY; // Bright Green
constexpr WORD COL_DINO_CLOSE = FOREGROUND_RED | FOREGROUND_INTENSITY; // Red when close
constexpr WORD COL_OBSTACLE = FOREGROUND_RED | FOREGROUND_GREEN; // Brown-ish (Yellow)
constexpr WORD COL_TEXT = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
constexpr WORD COL_DANGER = FOREGROUND_RED | FOREGROUND_INTENSITY;

// =========================================================
// ASSETS
// =========================================================
const std::vector<std::string> SPRITE_JEEP = {
    "  ___  ",
    " |  _\\ ",
    "=|__|#|",
    " (o)(o)"
};

const std::vector<std::string> SPRITE_DINO = {
    "   __ ",
    "  /..\\",
    " |  _/",
    " / /  ",
    " ^ ^  "
};

const std::vector<std::string> SPRITE_DINO_OPEN = {
    "   __ ",
    "  /..\\",
    " |  O/",
    " / /  ",
    " ^ ^  "
};

// =========================================================
// CONSOLE SYSTEM (Based on Tron.cpp)
// =========================================================
struct ConsoleState
{
  HANDLE hOut;
  std::unique_ptr<CHAR_INFO[]> buffer;
  COORD bufferSize{ (SHORT)SCREEN_W, (SHORT)SCREEN_H };
  COORD bufferCoord{ 0, 0 };
  SMALL_RECT writeRegion{ 0, 0, (SHORT)SCREEN_W - 1, (SHORT)SCREEN_H - 1 };
};

std::mt19937 rng(std::random_device{}());

void SetupConsole(ConsoleState& state)
{
  state.hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  state.buffer = std::make_unique<CHAR_INFO[]>(SCREEN_W * SCREEN_H);

  // Robust setup: Shrink window -> Resize Buffer -> Expand Window
  // This prevents errors if the new buffer is smaller than current window
  // or new window is larger than current buffer.
  SMALL_RECT minRect = { 0, 0, 1, 1 };
  SetConsoleWindowInfo(state.hOut, TRUE, &minRect);

  SetConsoleScreenBufferSize(state.hOut, state.bufferSize);
  SetConsoleWindowInfo(state.hOut, TRUE, &state.writeRegion);

  // Hide Cursor
  CONSOLE_CURSOR_INFO ci;
  GetConsoleCursorInfo(state.hOut, &ci);
  ci.bVisible = FALSE;
  SetConsoleCursorInfo(state.hOut, &ci);

  // Disable resizing
  HWND hwnd = GetConsoleWindow();
  SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
}

void DrawChar(ConsoleState& state, int x, int y, char c, WORD col)
{
  if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H) return;
  auto& cell = state.buffer[y * SCREEN_W + x];
  cell.Char.AsciiChar = c;
  cell.Attributes = col;
}

void DrawString(ConsoleState& state, int x, int y, const std::string& text, WORD col)
{
  for (size_t i = 0; i < text.size(); ++i)
    DrawChar(state, x + (int)i, y, text[i], col);
}

void DrawSprite(ConsoleState& state, int x, int y, const std::vector<std::string>& sprite, WORD col)
{
  for (size_t i = 0; i < sprite.size(); ++i)
    for (size_t j = 0; j < sprite[i].size(); ++j)
      if (sprite[i][j] != ' ') // Simple transparency
        DrawChar(state, x + (int)j, y + (int)i, sprite[i][j], col);
}

// =========================================================
// GAME LOGIC
// =========================================================
struct Player
{
  float y;
  float vy;
  float gravity = 0.6f;
  float jumpForce = -1.3f; // Reduced jump height (was -2.0f)
  bool isGrounded = true;
  int x = SCREEN_W / 3; // Fixed X position
};

struct Obstacle
{
  float x;
  int y;
  bool isRock; // true = rock, false = log
};

struct Star
{
  float x;
  int y;
  char symbol;
};

int main()
{
  ConsoleState state;
  SetupConsole(state);

  while (true)
  {
    // Game State
    Player p{ (float)(GROUND_Y - 4), 0 }; // -4 is jeep height
    std::vector<Obstacle> obstacles;
    std::vector<Star> stars;

    float dinoDist = 40.0f; // Visual distance units
    float maxDinoDist = 60.0f;
    float gameSpeed = 2.0f; // Speed up (was 0.8f)
    float groundOffset = 0.0f; // For scrolling ground texture
    int score = 0;
    int frames = 0;
    bool running = true;
    std::string gameOverReason = "";

    // Init Stars
    for (int i = 0; i < 20; ++i)
      stars.push_back({ (float)(rng() % SCREEN_W), (int)(rng() % (GROUND_Y - 5)), (rng() % 2 == 0 ? '.' : '+') });

    while (running)
    {
      // --- INPUT ---
      if (_kbhit())
      {
        char c = _getch();
        if (c == ' ' || c == 'w' || c == 72) // Space, W, or Up
        { 
          if (p.isGrounded) 
          {
            p.vy = p.jumpForce;
            p.isGrounded = false;
          }
        }
        else if (c == 'q')
        {
          return 0; // Quit entirely
        }
      }

      // --- UPDATE ---

      // Player Physics
      p.vy += p.gravity * 0.2f;
      p.y += p.vy;

      // Ground Collision
      if (p.y >= GROUND_Y - 4)
      {
        p.y = (float)(GROUND_Y - 4);
        p.vy = 0;
        p.isGrounded = true;
      }

      // Dino Logic
      if (dinoDist < maxDinoDist) dinoDist += 0.05f; // Slowly recover
      if (dinoDist <= 0)
      {
        running = false;
        gameOverReason = "EATEN BY T-REX";
      }

      // Obstacles Spawning
      if (frames % (int)(50 / gameSpeed) == 0)
      {
        if (std::uniform_int_distribution<>(0, 100)(rng) > 40)
        {
          bool isRock = std::uniform_int_distribution<>(0, 1)(rng);
          obstacles.push_back({ (float)SCREEN_W, GROUND_Y - 1, isRock });
        }
      }

      // Update Obstacles
      for (size_t i = 0; i < obstacles.size(); )
      {
        obstacles[i].x -= gameSpeed;

        // Collision Detection (AABB)
        // Jeep is 7 wide, 4 high
        // Rock is 3x1, Log is 5x1
        bool hit = false;
        int ox = (int)obstacles[i].x;
        int oy = obstacles[i].y;
        int pw = 7;
        int ph = 4;
        int px = p.x;
        int py = (int)p.y;

        if (px < ox + (obstacles[i].isRock ? 3 : 5) &&
          px + pw > ox &&
          py < oy + 1 &&
          py + ph > oy)
        {
          hit = true;
        }

        if (hit)
        {
          dinoDist -= 15.0f; // Punishment
          obstacles.erase(obstacles.begin() + i); // Remove obstacle on hit
        }
        else if (obstacles[i].x < 0)
        {
          obstacles.erase(obstacles.begin() + i);
          score++;
        }
        else
        {
          ++i;
        }
      }

      // Update Stars (Parallax)
      for (auto& s : stars)
      {
        s.x -= gameSpeed * 0.2f;
        if (s.x < 0) s.x = SCREEN_W - 1;
      }

      // Difficulty scaling
      if (frames % 200 == 0) gameSpeed += 0.05f;

      // Update Ground Scroll
      groundOffset += gameSpeed;

      // --- RENDER ---

      // Clear
      std::ranges::fill(std::span(state.buffer.get(), SCREEN_W * SCREEN_H), CHAR_INFO{ (char)' ', COL_BG });

      // Draw Stars
      for (const auto& s : stars)
        DrawChar(state, (int)s.x, s.y, s.symbol, FOREGROUND_INTENSITY);

      // Draw Ground
      int gOff = (int)groundOffset;
      for (int x = 0; x < SCREEN_W; ++x)
      {
        DrawChar(state, x, GROUND_Y, 'T', COL_GROUND);
        // Dirt texture scrolling
        if ((x + gOff) % 3 == 0) DrawChar(state, x, GROUND_Y + 1, '.', COL_GROUND);
        if ((x + gOff + 1) % 5 == 0) DrawChar(state, x, GROUND_Y + 2, ',', COL_GROUND);
      }
      
      // Draw Obstacles
      for (const auto& o : obstacles)
      {
        if (o.isRock)
          DrawString(state, (int)o.x, o.y, "/#\\", COL_OBSTACLE);
        else
          DrawString(state, (int)o.x, o.y, "=====", COL_OBSTACLE);
      }

      // Draw Jeep
      DrawSprite(state, p.x, (int)p.y, SPRITE_JEEP, COL_JEEP);

      // Draw Dino
      int dinoX = p.x - (int)dinoDist;
      if (dinoX > -10)
      {
        int dy = GROUND_Y - 5 + (int)(sin(frames * 0.2) * 1.5);
        WORD dCol = (dinoDist < 15.0f) ? COL_DINO_CLOSE : COL_DINO;

        // Animate mouth
        bool mouthOpen = (frames % 10 < 5 && dinoDist < 25.0f);
        DrawSprite(state, dinoX, dy, mouthOpen ? SPRITE_DINO_OPEN : SPRITE_DINO, dCol);
      }

      // Draw UI
      std::string scoreStr = "DIST: " + std::to_string(score) + " obstacles";
      DrawString(state, 2, 1, scoreStr, COL_TEXT);

      if (dinoDist < 15.0f)
        DrawString(state, SCREEN_W / 2 - 10, 2, "!! DANGER !!", COL_DANGER | BACKGROUND_INTENSITY);
        
      // Flush to console
      WriteConsoleOutputA(state.hOut, state.buffer.get(), state.bufferSize, state.bufferCoord, &state.writeRegion);

      frames++;
      std::this_thread::sleep_for(1000ms / FPS);
    }

    // --- GAME OVER SCREEN ---
    // Clear background for text
    std::string title = "GAME OVER";
    std::string sub = gameOverReason;
    std::string restart = "PRESS [SPACE] TO RESTART";

    int cy = SCREEN_H / 2;
    int cx = SCREEN_W / 2;

    DrawString(state, cx - (title.length() / 2), cy - 2, title, COL_DANGER);
    DrawString(state, cx - (sub.length() / 2), cy, sub, COL_JEEP);
    DrawString(state, cx - (restart.length() / 2), cy + 2, restart, COL_TEXT);

    WriteConsoleOutputA(state.hOut, state.buffer.get(), state.bufferSize, state.bufferCoord, &state.writeRegion);

    // Wait for input
    while (true)
    {
      if (_kbhit())
      {
        char c = _getch();
        if (c == ' ') break;
        if (c == 'q') return 0;
      }
      std::this_thread::sleep_for(100ms);
    }
  }
}
