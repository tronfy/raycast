#include "raycast.h"

int main() {
  LARGE_INTEGER frequency, time, oldTime;
  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&oldTime);

  Player player;
  init_player(&player);
  init_renderer();

  // game loop
  while (1) {
    double frameTime = get_frame_time(&frequency, &oldTime, &time);

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
      Sleep((DWORD)((targetFrameTime - frameTime) * 1000));
      QueryPerformanceCounter(&oldTime);
    } else {
      oldTime = time;
      QueryPerformanceCounter(&oldTime);
    }
  }

  return 0;
}
