#include "raycast.h"

// pre-compute screen constants
double invScreenWidth = 2.0 / SCREEN_WIDTH;
double halfScreenHeight = SCREEN_HEIGHT / 2.0;

void cast_ray(int x, Player *player, RayResult *result) {
  double cameraX = x * invScreenWidth - 1.0;
  double rayDirX = player->dirX + player->planeX * cameraX;
  double rayDirY = player->dirY + player->planeY * cameraX;

  int mapX = (int)player->posX;
  int mapY = (int)player->posY;

  // calculate ray unit step sizes
  double deltaDistX = (rayDirX == 0) ? 1e30 : fabs(1.0 / rayDirX);
  double deltaDistY = (rayDirY == 0) ? 1e30 : fabs(1.0 / rayDirY);

  double sideDistX, sideDistY;
  int stepX, stepY;

  // calculate step and initial sideDist
  if (rayDirX < 0) {
    stepX = -1;
    sideDistX = (player->posX - mapX) * deltaDistX;
  } else {
    stepX = 1;
    sideDistX = (mapX + 1.0 - player->posX) * deltaDistX;
  }
  if (rayDirY < 0) {
    stepY = -1;
    sideDistY = (player->posY - mapY) * deltaDistY;
  } else {
    stepY = 1;
    sideDistY = (mapY + 1.0 - player->posY) * deltaDistY;
  }

  // DDA algorithm
  int hit = 0;
  int side;

  while (hit == 0) {
    if (sideDistX < sideDistY) {
      sideDistX += deltaDistX;
      mapX += stepX;
      side = 0;
    } else {
      sideDistY += deltaDistY;
      mapY += stepY;
      side = 1;
    }
    if (map[mapX][mapY] > 0)
      hit = 1;
  }

  // calculate perpendicular wall distance
  if (side == 0)
    result->perpWallDist = (mapX - player->posX + (1 - stepX) * 0.5) / rayDirX;
  else
    result->perpWallDist = (mapY - player->posY + (1 - stepY) * 0.5) / rayDirY;

  // calculate draw positions
  result->lineHeight = (int)(SCREEN_HEIGHT / result->perpWallDist);
  result->drawStart = (int)(halfScreenHeight - result->lineHeight * 0.5);
  result->drawEnd = (int)(halfScreenHeight + result->lineHeight * 0.5);

  // clamp to screen
  if (result->drawStart < 0)
    result->drawStart = 0;
  if (result->drawEnd >= SCREEN_HEIGHT)
    result->drawEnd = SCREEN_HEIGHT - 1;

  result->wallChar = get_depth_char(result->perpWallDist);
}

void update_game(Player *player, double frameTime) {
  double moveSpeed = frameTime * 2.5;
  double rotSpeed = frameTime * 2.0;

  handle_input(player, moveSpeed, rotSpeed);
}

double get_frame_time(LARGE_INTEGER *frequency, LARGE_INTEGER *oldTime, LARGE_INTEGER *time) {
  *oldTime = *time;
  QueryPerformanceCounter(time);
  return (double)(time->QuadPart - oldTime->QuadPart) / frequency->QuadPart;
}
