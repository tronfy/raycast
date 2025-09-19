#ifndef RAYCAST_H
#define RAYCAST_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define MAP_WIDTH 10
#define MAP_HEIGHT 10

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 20

#define FPS_LIMIT 144

typedef struct {
  double posX, posY;
  double dirX, dirY;
  double planeX, planeY;
} Player;

typedef struct {
  double perpWallDist;
  int lineHeight;
  int drawStart, drawEnd;
  char wallChar;
} RayResult;

extern char screen[SCREEN_HEIGHT][SCREEN_WIDTH + 1];
extern char prevScreen[SCREEN_HEIGHT][SCREEN_WIDTH + 1];
extern int map[MAP_WIDTH][MAP_HEIGHT];
extern double invScreenWidth;
extern double halfScreenHeight;

// engine
void cast_ray(int x, Player *player, RayResult *result);
void update_game(Player *player, double frameTime);
double get_frame_time(LARGE_INTEGER *frequency, LARGE_INTEGER *oldTime, LARGE_INTEGER *time);

// renderer
void init_screen(void);
void display_screen(void);
void clear_screen_buffer(void);
void draw_column(int x, RayResult *result);
void init_renderer(void);
void display_debug_info(Player *player);
char get_depth_char(double distance);

// input
void handle_input(Player *player, double moveSpeed, double rotSpeed);
void rotate_player(Player *player, double rotSpeed);
void init_player(Player *player);

#endif // RAYCAST_H
