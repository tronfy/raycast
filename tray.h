#ifndef TRAY_H
#define TRAY_H

#define _DEFAULT_SOURCE // usleep

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define INPUT_DEVICE "/dev/input/event6"

#define MAP_WIDTH 60
#define MAP_HEIGHT 30

#define ROOM_ATTEMPTS 300
#define ROOM_MIN_SIZE 3
#define ROOM_MAX_SIZE 6

#define SCREEN_WIDTH 120
#define SCREEN_HEIGHT 40

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
double get_frame_time(struct timeval *oldTime, struct timeval *time);

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
void move_player(Player *player, double deltaX, double deltaY);
void rotate_player(Player *player, double rotSpeed);
void init_player(Player *player, double startX, double startY);
void init_input(void);
void cleanup_input(void);

// map
void display_map();
void generate_map();
void get_random_empty_position(int *x, int *y);

#endif // TRAY_H
