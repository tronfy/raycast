// filepath: map.c
#include "tray.h"

#define rand_range(min, max) (rand() % ((max) - (min) + 1) + (min))

enum { CELL_AIR = 0,
       CELL_PENDING = -1,
       CELL_WALL = -2 };

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
      if (map[x][y] == CELL_PENDING) {
        mapScreen[y][screenX] = '@';
      } else if (map[x][y] == CELL_WALL) {
        mapScreen[y][screenX] = '#';
      } else {
        mapScreen[y][screenX] = ' ';
        // mapScreen[y][screenX] = map[x][y] + 'A'; // debug
      }
      mapScreen[y][screenX + 1] = ' ';
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
  } while (map[*x][*y] != CELL_AIR);
}

// https://journal.stuffwithstuff.com/2014/12/21/rooms-and-mazes
void generate_map() {
  init_map_screen();

  // init map
  for (int x = 0; x < MAP_WIDTH; x++) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
      if (x == 0 || x == MAP_WIDTH - 1 || y == 0 || y == MAP_HEIGHT - 1) {
        map[x][y] = CELL_WALL;
      } else {
        map[x][y] = CELL_PENDING;
      }
    }
  }
  display_map();

  int idx = 0;

  // === 1. sprinkle rooms ===
  for (int i = 0; i < ROOM_ATTEMPTS; i++) {
    int roomWidth = rand_range(ROOM_MIN_SIZE, ROOM_MAX_SIZE);
    int roomHeight = rand_range(ROOM_MIN_SIZE, ROOM_MAX_SIZE);
    int roomX = rand_range(1, MAP_WIDTH - roomWidth - 1);
    int roomY = rand_range(1, MAP_HEIGHT - roomHeight - 1);

    int overlaps = 0;
    for (int x = roomX - 1; x < roomX + roomWidth + 1; x++) {
      for (int y = roomY - 1; y < roomY + roomHeight + 1; y++) {
        if (map[x][y] != CELL_PENDING) {
          overlaps = 1;
          break;
        }
      }
    }

    if (!overlaps) {
      for (int x = roomX - 1; x < roomX + roomWidth + 1; x++) {
        for (int y = roomY - 1; y < roomY + roomHeight + 1; y++) {
          if (map[x][y] == CELL_PENDING) {
            map[x][y] = CELL_WALL; // mark room walls
          }
        }
      }

      for (int x = roomX; x < roomX + roomWidth; x++) {
        for (int y = roomY; y < roomY + roomHeight; y++) {
          map[x][y] = idx; // mark room region with unique id
        }
      }

      idx++;

      display_map();
      usleep(20 * 1000);
    }
  }

  // === 2. fill the rest with mazes ===
  int stack[MAP_WIDTH * MAP_HEIGHT][2];
  int stackSize = 0;

  // find a starting pending cell
  int startX = -1, startY = -1;
  for (int y = 1; y < MAP_HEIGHT - 1 && startX == -1; y++) {
    for (int x = 1; x < MAP_WIDTH - 1 && startX == -1; x++) {
      if (map[x][y] == CELL_PENDING) {
        startX = x;
        startY = y;
      }
    }
  }

  if (startX != -1) {
    // push starting position to stack
    stack[stackSize][0] = startX;
    stack[stackSize][1] = startY;
    stackSize++;
    map[startX][startY] = idx;

    while (stackSize > 0) {
      int currentX = stack[stackSize - 1][0];
      int currentY = stack[stackSize - 1][1];

      // find unvisited pending neighbors
      int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
      int validDirections[4][2];
      int validCount = 0;

      for (int i = 0; i < 4; i++) {
        int nx = currentX + directions[i][0];
        int ny = currentY + directions[i][1];
        if (nx > 0 && nx < MAP_WIDTH - 1 && ny > 0 && ny < MAP_HEIGHT - 1 &&
            map[nx][ny] == CELL_PENDING) {
          validDirections[validCount][0] = directions[i][0];
          validDirections[validCount][1] = directions[i][1];
          validCount++;
        }
      }

      if (validCount > 0) {
        int randomDir = rand() % validCount;
        int dx = validDirections[randomDir][0];
        int dy = validDirections[randomDir][1];

        int midX = currentX + dx / 2;
        int midY = currentY + dy / 2;
        int nextX = currentX + dx;
        int nextY = currentY + dy;

        map[midX][midY] = idx;
        map[nextX][nextY] = idx;

        // mark all other valid directions as wall
        for (int i = 0; i < validCount; i++) {
          if (i != randomDir) {
            int otherX = currentX + validDirections[i][0];
            int otherY = currentY + validDirections[i][1];
            map[otherX][otherY] = CELL_WALL;
          }
        }

        // push new position to stack
        stack[stackSize][0] = nextX;
        stack[stackSize][1] = nextY;
        stackSize++;

        display_map();
        usleep(5 * 1000);
      } else {
        stackSize--;

        // if stack is empty but there are still unvisited walls, start new maze
        if (stackSize == 0) {
          idx++;
          for (int y = 1; y < MAP_HEIGHT - 1 && stackSize == 0; y++) {
            for (int x = 1; x < MAP_WIDTH - 1 && stackSize == 0; x++) {
              if (map[x][y] == CELL_PENDING) {
                stack[stackSize][0] = x;
                stack[stackSize][1] = y;
                stackSize++;
                map[x][y] = idx;
              }
            }
          }
        }
      }
    }
  }

  // === 3. find connectors (walls adjacent to two unjoined regions) ===

  int connectors[MAP_WIDTH * MAP_HEIGHT][2];
  int connectorCount = 0;

  for (int y = 1; y < MAP_HEIGHT - 1; y++) {
    for (int x = 1; x < MAP_WIDTH - 1; x++) {
      if (map[x][y] == CELL_WALL) {
        int regionIds[4] = {-1, -1, -1, -1};
        int regionCount = 0;
        int wallCount = 0;

        // check neighbors
        if (map[x - 1][y] >= 0) {
          regionIds[regionCount++] = map[x - 1][y];
        } else if (map[x - 1][y] == CELL_WALL) {
          wallCount++;
        }

        if (map[x + 1][y] >= 0) {
          regionIds[regionCount++] = map[x + 1][y];
        } else if (map[x + 1][y] == CELL_WALL) {
          wallCount++;
        }

        if (map[x][y - 1] >= 0) {
          regionIds[regionCount++] = map[x][y - 1];
        } else if (map[x][y - 1] == CELL_WALL) {
          wallCount++;
        }

        if (map[x][y + 1] >= 0) {
          regionIds[regionCount++] = map[x][y + 1];
        } else if (map[x][y + 1] == CELL_WALL) {
          wallCount++;
        }

        // check for two walls on opposite sides and two different regions
        if (wallCount == 2 && regionCount == 2 &&
            ((map[x - 1][y] == CELL_WALL && map[x + 1][y] == CELL_WALL &&
              regionIds[0] != regionIds[1]) ||
             (map[x][y - 1] == CELL_WALL && map[x][y + 1] == CELL_WALL &&
              regionIds[0] != regionIds[1]))) {
          connectors[connectorCount][0] = x;
          connectors[connectorCount][1] = y;
          connectorCount++;
        }
      }
    }
  }

  // === 4. open connectors at random until all regions are joined ===

  // shuffle connectors
  for (int i = 0; i < connectorCount; i++) {
    int r = rand() % connectorCount;
    int tempX = connectors[i][0];
    int tempY = connectors[i][1];
    connectors[i][0] = connectors[r][0];
    connectors[i][1] = connectors[r][1];
    connectors[r][0] = tempX;
    connectors[r][1] = tempY;
  }

  while (connectorCount > 0) {
    int cx = connectors[connectorCount - 1][0];
    int cy = connectors[connectorCount - 1][1];
    connectorCount--;

    // find the two regions it connects
    int regionA = -1, regionB = -1;
    if (map[cx - 1][cy] >= 0) {
      regionA = map[cx - 1][cy];
      regionB = map[cx + 1][cy];
    } else if (map[cx + 1][cy] >= 0) {
      regionA = map[cx + 1][cy];
      regionB = map[cx - 1][cy];
    } else if (map[cx][cy - 1] >= 0) {
      regionA = map[cx][cy - 1];
      regionB = map[cx][cy + 1];
    } else if (map[cx][cy + 1] >= 0) {
      regionA = map[cx][cy + 1];
      regionB = map[cx][cy - 1];
    }

    if (regionA == regionB || regionA < 0 || regionB < 0) {
      continue; // already joined or invalid
    }

    // turn the connector and region B cells into region A
    map[cx][cy] = regionA;
    for (int x = 1; x < MAP_WIDTH - 1; x++)
      for (int y = 1; y < MAP_HEIGHT - 1; y++)
        if (map[x][y] == regionB)
          map[x][y] = regionA;

    // check if all regions are joined
    int regionId = -1;
    for (int x = 1; x < MAP_WIDTH - 1; x++) {
      for (int y = 1; y < MAP_HEIGHT - 1; y++) {
        if (map[x][y] >= 0) {
          if (regionId == -1) {
            regionId = map[x][y];
          } else if (map[x][y] != regionId) {
            regionId = -1;
            break;
          }
        }
      }
      if (regionId == -1) {
        break;
      }
    }

    if (regionId >= 0) {
      break; // all joined
    }

    display_map();
    usleep(30 * 1000);
  }

  // === 5. fill dead ends (spaces surrounded by walls on 3 sides) ===
  while (1) {
    int deadEndFound = 0;
    for (int y = 1; y < MAP_HEIGHT - 1; y++) {
      for (int x = 1; x < MAP_WIDTH - 1; x++) {
        if (map[x][y] >= 0) {
          int wallCount = 0;
          if (map[x - 1][y] == CELL_WALL)
            wallCount++;
          if (map[x + 1][y] == CELL_WALL)
            wallCount++;
          if (map[x][y - 1] == CELL_WALL)
            wallCount++;
          if (map[x][y + 1] == CELL_WALL)
            wallCount++;

          if (wallCount >= 3) {
            map[x][y] = CELL_WALL;
            deadEndFound = 1;
          }
        }
      }
    }

    if (!deadEndFound)
      break;

    display_map();
    usleep(50 * 1000);
  }

  // cleanup
  for (int x = 0; x < MAP_WIDTH; x++) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
      if (map[x][y] >= 0) {
        map[x][y] = CELL_AIR;
      } else if (map[x][y] == CELL_PENDING) {
        map[x][y] = CELL_WALL;
      }
    }
  }

  display_map();
  usleep(1000 * 1000);
}
