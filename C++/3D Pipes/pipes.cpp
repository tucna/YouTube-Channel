#define T_PGE_APPLICATION
#include "engine/tPixelGameEngine.h"
#include <cmath>
#include <algorithm>
#include <vector>
#include <array>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <cstring>

struct Vec3
{
  float x, y, z;

  Vec3 operator+(const Vec3& o) const
  {
    return { x + o.x, y + o.y, z + o.z };
  }

  Vec3 operator-(const Vec3& o) const
  {
    return { x - o.x, y - o.y, z - o.z };
  }

  Vec3 operator*(float s) const
  {
    return { x * s, y * s, z * s };
  }

  bool operator==(const Vec3& o) const
  {
    return x == o.x && y == o.y && z == o.z;
  }

  bool operator!=(const Vec3& o) const
  {
    return !(*this == o);
  }

  float Length() const
  {
    return std::sqrt(x * x + y * y + z * z);
  }
};

float Dot(const Vec3& a, const Vec3& b)
{
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 Cross(const Vec3& a, const Vec3& b)
{
  return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}

Vec3 Normalize(Vec3 v)
{
  float l = v.Length();
  return (l > 0.0001f) ? Vec3{ v.x / l, v.y / l, v.z / l } : Vec3{ 0.0f, 0.0f, 0.0f };
}

struct Int3
{
  int x, y, z;

  bool operator==(const Int3& o) const
  {
    return x == o.x && y == o.y && z == o.z;
  }
};

const int GRID_SIZE = 18;
const float SPACING = 20.0f;
const float PIPE_RADIUS = 6.0f;
const float SPHERE_RADIUS = 7.0f;
const float DRAW_SPEED = 0.2f;
const int PIPE_COUNT = 3;

const tDX::Pixel COLORS[] = {
    tDX::Pixel(255, 60, 60), tDX::Pixel(60, 255, 60), tDX::Pixel(60, 60, 255),
    tDX::Pixel(255, 255, 60), tDX::Pixel(255, 60, 255), tDX::Pixel(60, 255, 255),
    tDX::Pixel(200, 200, 200)
};

const Vec3 DIRECTIONS[] = {
    {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
    {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}
};

struct Joint
{
  Vec3 pos;
  tDX::Pixel color;
};

struct Segment
{
  Vec3 start, end;
  tDX::Pixel color;
};

struct ActivePipe
{
  Int3 gridPos;
  Vec3 worldPos;
  Vec3 currentDir;
  tDX::Pixel color;
  float progress;
  bool active;
};

class Pipes3D : public tDX::PixelGameEngine
{
private:
  bool grid[GRID_SIZE][GRID_SIZE][GRID_SIZE];
  std::vector<Joint> staticJoints;
  std::vector<Segment> staticSegments;
  std::vector<ActivePipe> activePipes;
  int restartTimer = 0;

  std::vector<float> depthBuffer;

  float camRotX = 0.5f;
  float camRotY = -0.7f;
  float camDistance = 150.0f;

  // Cached trigonometry values
  float sinCamX, cosCamX, sinCamY, cosCamY;

public:
  Pipes3D()
  {
    sAppName = "3D Pipes (Z-Buffered Raycaster)";
  }

  void ResetSimulation()
  {
    std::memset(grid, 0, sizeof(grid));
    staticJoints.clear();
    staticSegments.clear();
    activePipes.clear();
    for (int i = 0; i < PIPE_COUNT; i++)
    {
      SpawnPipe();
    }
    restartTimer = 0;
  }

  Vec3 GridToWorld(Int3 g)
  {
    float offset = (GRID_SIZE - 1) * 0.5f * SPACING;
    return { g.x * SPACING - offset, g.y * SPACING - offset, g.z * SPACING - offset };
  }

  bool IsOccupied(Int3 g)
  {
    if (g.x < 0 || g.x >= GRID_SIZE || g.y < 0 || g.y >= GRID_SIZE || g.z < 0 || g.z >= GRID_SIZE)
    {
      return true;
    }
    return grid[g.x][g.y][g.z];
  }

  void SpawnPipe()
  {
    ActivePipe p;
    p.active = true;
    p.color = COLORS[rand() % std::size(COLORS)];
    p.progress = 0.0f;

    int attempts = 0;
    do
    {
      p.gridPos = { rand() % GRID_SIZE, rand() % GRID_SIZE, rand() % GRID_SIZE };
      attempts++;
    } while (IsOccupied(p.gridPos) && attempts < 100);

    if (attempts >= 100)
    {
      return;
    }

    grid[p.gridPos.x][p.gridPos.y][p.gridPos.z] = true;
    p.worldPos = GridToWorld(p.gridPos);
    p.currentDir = DIRECTIONS[rand() % 6];
    staticJoints.push_back({ p.worldPos, p.color });
    TryStartSegment(p);
    activePipes.push_back(p);
  }

  void TryStartSegment(ActivePipe& p)
  {
    std::array<Vec3, 6> validDirs;
    int validCount = 0;

    bool keepStraight = (rand() % 100) < 80;

    if (keepStraight)
    {
      Int3 nextGrid = { p.gridPos.x + (int)p.currentDir.x, p.gridPos.y + (int)p.currentDir.y, p.gridPos.z + (int)p.currentDir.z };
      if (!IsOccupied(nextGrid))
      {
        validDirs[validCount++] = p.currentDir;
      }
    }

    if (validCount == 0)
    {
      for (int i = 0; i < 6; i++)
      {
        if (DIRECTIONS[i].x + p.currentDir.x == 0 && DIRECTIONS[i].y + p.currentDir.y == 0 && DIRECTIONS[i].z + p.currentDir.z == 0)
        {
          continue;
        }

        Int3 nextGrid = { p.gridPos.x + (int)DIRECTIONS[i].x, p.gridPos.y + (int)DIRECTIONS[i].y, p.gridPos.z + (int)DIRECTIONS[i].z };
        if (!IsOccupied(nextGrid))
        {
          validDirs[validCount++] = DIRECTIONS[i];
        }
      }
    }

    if (validCount == 0)
    {
      p.active = false;
      staticJoints.push_back({ p.worldPos, p.color });
      return;
    }

    Vec3 newDir = validDirs[rand() % validCount];
    if (newDir != p.currentDir)
    {
      staticJoints.push_back({ p.worldPos, p.color });
      p.currentDir = newDir;
    }

    Int3 targetGrid = { p.gridPos.x + (int)p.currentDir.x, p.gridPos.y + (int)p.currentDir.y, p.gridPos.z + (int)p.currentDir.z };
    grid[targetGrid.x][targetGrid.y][targetGrid.z] = true;
  }

  Vec3 WorldToCamera(Vec3 v)
  {
    float nx = v.x * cosCamY - v.z * sinCamY;
    float nz = v.x * sinCamY + v.z * cosCamY;
    v.x = nx;
    v.z = nz;

    float ny = v.y * cosCamX - v.z * sinCamX;
    nz = v.y * sinCamX + v.z * cosCamX;
    v.y = ny;
    v.z = nz;

    v.z += camDistance;
    return v;
  }

  tDX::Pixel ComputeLighting(tDX::Pixel baseColor, Vec3 normal)
  {
    Vec3 lightDir = Normalize({ 0.7f, 0.7f, -1.0f });
    float ambient = 0.25f;

    float baseSpec = std::max(0.0f, Dot(normal, lightDir));
    float diffuse = baseSpec;
    float spec = std::pow(std::max(0.0f, Dot(normal, lightDir)), 35.0f) * 1.5f;
    float intensity = ambient + diffuse * 0.7f;

    int r = std::min(255, (int)(baseColor.r * intensity + spec * 255.0f));
    int g = std::min(255, (int)(baseColor.g * intensity + spec * 255.0f));
    int b = std::min(255, (int)(baseColor.b * intensity + spec * 255.0f));

    return tDX::Pixel(r, g, b);
  }

  void DrawSphereRaycastZ(Vec3 C, float R, tDX::Pixel color)
  {
    if (C.z - R < 1.0f)
    {
      return;
    }

    float fov = (float)ScreenHeight();
    float sx = (C.x / C.z) * fov + ScreenWidth() / 2.0f;
    float sy = (C.y / C.z) * fov + ScreenHeight() / 2.0f;
    float r_screen = (R / (C.z - R)) * fov * 1.2f;

    int minX = std::max(0, (int)(sx - r_screen));
    int maxX = std::min(ScreenWidth() - 1, (int)(sx + r_screen));
    int minY = std::max(0, (int)(sy - r_screen));
    int maxY = std::min(ScreenHeight() - 1, (int)(sy + r_screen));

    for (int y = minY; y <= maxY; y++)
    {
      for (int x = minX; x <= maxX; x++)
      {
        Vec3 D = Normalize({ (float)x - ScreenWidth() / 2.0f, (float)y - ScreenHeight() / 2.0f, fov });
        float b = -2.0f * Dot(D, C);
        float c = Dot(C, C) - R * R;
        float det = b * b - 4.0f * 1.0f * c;

        if (det >= 0.0f)
        {
          float t = (-b - std::sqrt(det)) / 2.0f;
          if (t > 1.0f)
          {
            int idx = y * ScreenWidth() + x;
            if (t < depthBuffer[idx])
            {
              depthBuffer[idx] = t;
              Vec3 Hit = D * t;
              Draw(x, y, ComputeLighting(color, Normalize(Hit - C)));
            }
          }
        }
      }
    }
  }

  void DrawCylinderRaycastZ(Vec3 A, Vec3 B, float R, tDX::Pixel color)
  {
    Vec3 V = B - A;
    float L = V.Length();
    if (L < 0.01f)
    {
      return;
    }
    V = V * (1.0f / L);

    float fov = (float)ScreenHeight();

    float sxA = (A.x / std::max(1.0f, A.z)) * fov + ScreenWidth() / 2.0f;
    float syA = (A.y / std::max(1.0f, A.z)) * fov + ScreenHeight() / 2.0f;
    float sxB = (B.x / std::max(1.0f, B.z)) * fov + ScreenWidth() / 2.0f;
    float syB = (B.y / std::max(1.0f, B.z)) * fov + ScreenHeight() / 2.0f;
    float minZ = std::max(1.0f, std::min(A.z, B.z) - R);
    float pad = (R / minZ) * fov * 1.5f;

    int minX = std::max(0, (int)(std::min(sxA, sxB) - pad));
    int maxX = std::min(ScreenWidth() - 1, (int)(std::max(sxA, sxB) + pad));
    int minY = std::max(0, (int)(std::min(syA, syB) - pad));
    int maxY = std::min(ScreenHeight() - 1, (int)(std::max(syA, syB) + pad));

    for (int y = minY; y <= maxY; y++)
    {
      for (int x = minX; x <= maxX; x++)
      {
        Vec3 D = Normalize({ (float)x - ScreenWidth() / 2.0f, (float)y - ScreenHeight() / 2.0f, fov });
        Vec3 P = Cross(D, V);
        Vec3 Q = Cross(Vec3{ 0.0f, 0.0f, 0.0f } - A, V);

        float a = Dot(P, P);
        if (a < 0.0001f)
        {
          continue;
        }

        float b = 2.0f * Dot(P, Q);
        float c = Dot(Q, Q) - R * R;
        float det = b * b - 4.0f * a * c;

        if (det >= 0.0f)
        {
          float t = (-b - std::sqrt(det)) / (2.0f * a);
          if (t > 1.0f)
          {
            Vec3 Hit = D * t;
            float k = Dot(Hit - A, V);
            if (k >= 0.0f && k <= L)
            {
              int idx = y * ScreenWidth() + x;
              if (t < depthBuffer[idx])
              {
                depthBuffer[idx] = t;
                Vec3 CenterAxis = A + V * k;
                Draw(x, y, ComputeLighting(color, Normalize(Hit - CenterAxis)));
              }
            }
          }
        }
      }
    }
  }

  bool OnUserCreate() override
  {
    srand((unsigned int)time(NULL));
    depthBuffer.resize(ScreenWidth() * ScreenHeight());
    ResetSimulation();
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override
  {
    sinCamX = std::sin(camRotX);
    cosCamX = std::cos(camRotX);
    sinCamY = std::sin(camRotY);
    cosCamY = std::cos(camRotY);

    bool allDead = true;

    for (auto& p : activePipes)
    {
      if (!p.active)
      {
        continue;
      }

      allDead = false;
      p.progress += DRAW_SPEED;

      if (p.progress >= 1.0f)
      {
        Vec3 endPos = p.worldPos + p.currentDir * SPACING;
        staticSegments.push_back({ p.worldPos, endPos, p.color });
        p.worldPos = endPos;
        p.gridPos = { p.gridPos.x + (int)p.currentDir.x, p.gridPos.y + (int)p.currentDir.y, p.gridPos.z + (int)p.currentDir.z };
        p.progress = 0.0f;
        TryStartSegment(p);
      }
    }

    if (allDead)
    {
      restartTimer++;
      if (restartTimer > 60)
      {
        ResetSimulation();
      }
    }

    Clear(tDX::Pixel(0, 0, 0));

    std::fill(depthBuffer.begin(), depthBuffer.end(), std::numeric_limits<float>::max());

    for (const auto& j : staticJoints)
    {
      DrawSphereRaycastZ(WorldToCamera(j.pos), SPHERE_RADIUS, j.color);
    }

    for (const auto& s : staticSegments)
    {
      DrawCylinderRaycastZ(WorldToCamera(s.start), WorldToCamera(s.end), PIPE_RADIUS, s.color);
    }

    for (const auto& p : activePipes)
    {
      if (!p.active)
      {
        continue;
      }

      Vec3 p1 = p.worldPos;
      Vec3 p2 = p.worldPos + p.currentDir * (SPACING * p.progress);
      DrawCylinderRaycastZ(WorldToCamera(p1), WorldToCamera(p2), PIPE_RADIUS, p.color);
      DrawSphereRaycastZ(WorldToCamera(p2), PIPE_RADIUS, p.color);
    }

    return true;
  }
};

int main()
{
  Pipes3D demo;
  if (demo.Construct(320, 240, 2, 2))
  {
    demo.Start();
  }
  return 0;
}
