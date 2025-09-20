#include "tray.h"

char screen[SCREEN_HEIGHT][SCREEN_WIDTH + 1];
char prevScreen[SCREEN_HEIGHT][SCREEN_WIDTH + 1];

void init_screen(void) {
  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      screen[y][x] = ' ';
    }
    screen[y][SCREEN_WIDTH] = '\0';
  }
}

char get_depth_char(double distance) {
  if (distance < 1.0)
    return '@';
  else if (distance < 2.0)
    return '#';
  else if (distance < 3.0)
    return '+';
  else if (distance < 4.0)
    return '=';
  else
    return '.';
}

void display_screen(void) {
  int hasChanged = 0;
  int firstChangedLine = SCREEN_HEIGHT;
  int lastChangedLine = -1;

  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      if (screen[y][x] != prevScreen[y][x]) {
        if (!hasChanged) {
          hasChanged = 1;
          firstChangedLine = y;
        }
        lastChangedLine = y;
        break;
      }
    }
  }

  if (hasChanged) {
    for (int y = firstChangedLine; y <= lastChangedLine; y++) {
      printf("\033[%d;1H%s", y + 1, screen[y]);
    }

    for (int y = firstChangedLine; y <= lastChangedLine; y++) {
      for (int x = 0; x < SCREEN_WIDTH; x++) {
        prevScreen[y][x] = screen[y][x];
      }
    }
  }

  fflush(stdout);
}

void clear_screen_buffer(void) {
  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      screen[y][x] = ' ';
    }
  }
}

void draw_column(int x, RayResult *result) {
  for (int y = result->drawStart; y <= result->drawEnd; y++) {
    screen[y][x] = result->wallChar;
  }
}

void init_renderer(void) {
  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      prevScreen[y][x] = ' ';
    }
    prevScreen[y][SCREEN_WIDTH] = '\0';
  }

  // clear screen
  printf("\033[2J\033[H");
}

void display_debug_info(Player *player) {
  printf("\033[%d;1HPos: (%.2f, %.2f) Dir: (%.2f, %.2f) Plane: (%.2f, %.2f)",
         SCREEN_HEIGHT + 2, player->posX, player->posY, player->dirX, player->dirY,
         player->planeX, player->planeY);
}
