#include "raycast.h"

void handle_input(Player *player, double moveSpeed, double rotSpeed) {
  // forward/backward
  if (GetAsyncKeyState('W') & 0x8000)
    move_player(player, player->dirX * moveSpeed, player->dirY * moveSpeed);
  if (GetAsyncKeyState('S') & 0x8000)
    move_player(player, -player->dirX * moveSpeed, -player->dirY * moveSpeed);

  // rotation
  if (GetAsyncKeyState('D') & 0x8000)
    rotate_player(player, rotSpeed);
  if (GetAsyncKeyState('A') & 0x8000)
    rotate_player(player, -rotSpeed);

  // strafing
  if (GetAsyncKeyState('Q') & 0x8000)
    move_player(player, player->dirY * moveSpeed, -player->dirX * moveSpeed);
  if (GetAsyncKeyState('E') & 0x8000)
    move_player(player, -player->dirY * moveSpeed, player->dirX * moveSpeed);
}

void move_player(Player *player, double deltaX, double deltaY) {
  if (map[(int)(player->posX + deltaX)][(int)player->posY] == 0)
    player->posX += deltaX;

  if (map[(int)player->posX][(int)(player->posY + deltaY)] == 0)
    player->posY += deltaY;
}

void rotate_player(Player *player, double rotSpeed) {
  double oldDirX = player->dirX;
  player->dirX = player->dirX * cos(rotSpeed) - player->dirY * sin(rotSpeed);
  player->dirY = oldDirX * sin(rotSpeed) + player->dirY * cos(rotSpeed);

  double oldPlaneX = player->planeX;
  player->planeX = player->planeX * cos(rotSpeed) - player->planeY * sin(rotSpeed);
  player->planeY = oldPlaneX * sin(rotSpeed) + player->planeY * cos(rotSpeed);
}

void init_player(Player *player) {
  player->posX = 1.5;
  player->posY = 1.5;
  player->dirX = 1.0;
  player->dirY = 0.0;
  player->planeX = 0.0;
  player->planeY = 0.66;
}
