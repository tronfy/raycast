#include "raycast.h"

void handle_input(Player *player, double moveSpeed, double rotSpeed) {
  // forward/backward
  if (GetAsyncKeyState('W') & 0x8000) {
    if (map[(int)(player->posX + player->dirX * moveSpeed)][(int)player->posY] == 0)
      player->posX += player->dirX * moveSpeed;
    if (map[(int)player->posX][(int)(player->posY + player->dirY * moveSpeed)] == 0)
      player->posY += player->dirY * moveSpeed;
  }
  if (GetAsyncKeyState('S') & 0x8000) {
    if (map[(int)(player->posX - player->dirX * moveSpeed)][(int)player->posY] == 0)
      player->posX -= player->dirX * moveSpeed;
    if (map[(int)player->posX][(int)(player->posY - player->dirY * moveSpeed)] == 0)
      player->posY -= player->dirY * moveSpeed;
  }

  // rotation
  if (GetAsyncKeyState('D') & 0x8000) {
    rotate_player(player, rotSpeed);
  }
  if (GetAsyncKeyState('A') & 0x8000) {
    rotate_player(player, -rotSpeed);
  }

  // strafing
  if (GetAsyncKeyState('Q') & 0x8000) {
    double strafeX = -player->dirY;
    double strafeY = player->dirX;
    if (map[(int)(player->posX + strafeX * moveSpeed)][(int)player->posY] == 0)
      player->posX += strafeX * moveSpeed;
    if (map[(int)player->posX][(int)(player->posY + strafeY * moveSpeed)] == 0)
      player->posY += strafeY * moveSpeed;
  }
  if (GetAsyncKeyState('E') & 0x8000) {
    double strafeX = player->dirY;
    double strafeY = -player->dirX;
    if (map[(int)(player->posX + strafeX * moveSpeed)][(int)player->posY] == 0)
      player->posX += strafeX * moveSpeed;
    if (map[(int)player->posX][(int)(player->posY + strafeY * moveSpeed)] == 0)
      player->posY += strafeY * moveSpeed;
  }
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
