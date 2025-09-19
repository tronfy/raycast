#include "tray.h"
#include <linux/input.h>

static int inputFd = -1;
static int inputInitialized = 0;
static char keys[KEY_MAX] = {0};
static struct termios oldTermios;

void init_input(void) {
  // disable terminal echo and canonical mode
  struct termios newTermios;
  tcgetattr(STDIN_FILENO, &oldTermios);
  newTermios = oldTermios;
  newTermios.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);

  inputFd = open(INPUT_DEVICE, O_RDONLY | O_NONBLOCK);
  if (inputFd == -1) {
    perror("Failed to open /dev/input/event6");
    fprintf(stderr, "Note: You may need to run with sudo or add your user to "
                    "the 'input' group\n");
    exit(1);
  }

  // clear key states
  memset(keys, 0, sizeof(keys));
  inputInitialized = 1;
}

void cleanup_input(void) {
  if (inputInitialized && inputFd != -1) {
    close(inputFd);
    inputFd = -1;
    inputInitialized = 0;
  }

  printf("\033[?25h"); // show cursor
  fflush(stdout);

  // restore terminal settings
  tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);
}

void handle_input(Player *player, double moveSpeed, double rotSpeed) {
  struct input_event ev;
  while (read(inputFd, &ev, sizeof(ev)) == sizeof(ev)) {
    if (ev.type == EV_KEY) {
      if (ev.code < KEY_MAX) {
        keys[ev.code] = (ev.value == 1 || ev.value == 2) ? 1 : 0; // 1 = press, 2 = held, 0 = release
      }
    }
  }

  // forward/backward
  if (keys[KEY_W])
    move_player(player, player->dirX * moveSpeed, player->dirY * moveSpeed);
  if (keys[KEY_S])
    move_player(player, -player->dirX * moveSpeed, -player->dirY * moveSpeed);

  // rotation
  if (keys[KEY_D])
    rotate_player(player, rotSpeed);
  if (keys[KEY_A])
    rotate_player(player, -rotSpeed);

  // strafing
  if (keys[KEY_Q])
    move_player(player, player->dirY * moveSpeed, -player->dirX * moveSpeed);
  if (keys[KEY_E])
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
  player->planeX =
      player->planeX * cos(rotSpeed) - player->planeY * sin(rotSpeed);
  player->planeY = oldPlaneX * sin(rotSpeed) + player->planeY * cos(rotSpeed);
}

void init_player(Player *player, double startX, double startY) {
  player->posX = startX;
  player->posY = startY;
  player->dirX = 1.0;
  player->dirY = 0.0;
  player->planeX = 0.0;
  player->planeY = 0.66;
}
