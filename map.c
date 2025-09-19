#include "tray.h"

#define rand_range(min, max) (rand() % ((max) - (min) + 1) + (min))

int map[MAP_WIDTH][MAP_HEIGHT];
char mapScreen[MAP_HEIGHT][MAP_WIDTH * 2 + 1]; // *2 for "# " or "  " per cell
char prevMapScreen[MAP_HEIGHT][MAP_WIDTH * 2 + 1];

void init_map_screen() {
  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH * 2; x++) {
      mapScreen[y][x] = ' ';
      prevMapScreen[y][x] = ' ';
    }
    mapScreen[y][MAP_WIDTH * 2] = '\0';
    prevMapScreen[y][MAP_WIDTH * 2] = '\0';
  }
}

void update_map_screen() {
  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH; x++) {
      int screenX = x * 2;
      if (map[x][y] == 1) {
        mapScreen[y][screenX] = '#';
        mapScreen[y][screenX + 1] = ' ';
      } else {
        mapScreen[y][screenX] = ' ';
        mapScreen[y][screenX + 1] = ' ';
      }
    }
  }
}

void display_map() {
  update_map_screen();

  int hasChanged = 0;
  int firstChangedLine = MAP_HEIGHT;
  int lastChangedLine = -1;

  for (int y = 0; y < MAP_HEIGHT; y++) {
    for (int x = 0; x < MAP_WIDTH * 2; x++) {
      if (mapScreen[y][x] != prevMapScreen[y][x]) {
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
      printf("\033[%d;1H%s", y + 1, mapScreen[y]);
    }

    for (int y = firstChangedLine; y <= lastChangedLine; y++) {
      for (int x = 0; x < MAP_WIDTH * 2; x++) {
        prevMapScreen[y][x] = mapScreen[y][x];
      }
    }
  }

  fflush(stdout);
}

void get_random_empty_position(int *x, int *y) {
  do {
    *x = rand_range(1, MAP_WIDTH - 2);
    *y = rand_range(1, MAP_HEIGHT - 2);
  } while (map[*x][*y] != 0);
}

// https://journal.stuffwithstuff.com/2014/12/21/rooms-and-mazes
void generate_map() {
  init_map_screen();

  // init map
  for (int x = 0; x < MAP_WIDTH; x++) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
      map[x][y] = 1; // wall
    }
  }
  display_map();

  // 1. sprinkle rooms
  for (int i = 0; i < ROOM_ATTEMPTS; i++) {
    int roomWidth = rand_range(ROOM_MIN_SIZE, ROOM_MAX_SIZE);
    int roomHeight = rand_range(ROOM_MIN_SIZE, ROOM_MAX_SIZE);
    int roomX = rand_range(1, MAP_WIDTH - roomWidth - 1);
    int roomY = rand_range(1, MAP_HEIGHT - roomHeight - 1);

    int overlaps = 0;
    for (int x = roomX - 1; x < roomX + roomWidth + 1; x++) {
      for (int y = roomY - 1; y < roomY + roomHeight + 1; y++) {
        if (map[x][y] == 0) {
          overlaps = 1;
          break;
        }
      }
    }

    if (!overlaps) {
      for (int x = roomX; x < roomX + roomWidth; x++) {
        for (int y = roomY; y < roomY + roomHeight; y++) {
          map[x][y] = 0; // carve out the room
        }
      }
      display_map();
      Sleep(100);
    }
  }

  // 2. fill the rest with mazes

  // 3. find connectors (walls adjacent to two unjoined regions)

  // 4. open connectors until all regions are joined

  // 5. fill dead ends (spaces surrounded by walls on 3 sides)
}
