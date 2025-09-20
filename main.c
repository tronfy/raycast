#include "tray.h"

int main() {
  printf("\033[?25l"); // hide cursor

  srand(time(NULL));
  generate_map();

  struct timeval time,
      oldTime;
  gettimeofday(&oldTime, NULL);

  Player player;
  int startX, startY;
  get_random_empty_position(&startX, &startY);
  init_player(&player, startX + 0.5, startY + 0.5);
  init_renderer();
  init_input();

  // game loop
  while (1) {
    double frameTime = get_frame_time(&oldTime, &time);

    clear_screen_buffer();

    // cast rays for each screen column
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      RayResult result;
      cast_ray(x, &player, &result);
      draw_column(x, &result);
    }

    display_screen();
    display_debug_info(&player);
    update_game(&player, frameTime);

    // cap frame rate
    double targetFrameTime = 1.0 / FPS_LIMIT;
    if (frameTime < targetFrameTime) {
      gettimeofday(&oldTime, NULL);
      usleep((unsigned int)((targetFrameTime - frameTime) * 1000000));
    } else {
      oldTime = time;
      gettimeofday(&oldTime, NULL);
    }
  }

  cleanup_input();
  return 0;
}
