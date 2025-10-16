#include "../../brick_game/colors.h"
#include "frontend.h"
#define REFRESH_RATE 60

int printGameScreen(void* arg);
void printField(WINDOW* win, const GameInfo_t* info);
void printNext(WINDOW* win, const GameInfo_t* info);
void printStats(WINDOW* stats_win, const GameInfo_t* info);
void printContext(WINDOW* context_win, const GameInfo_t* info);
void printCondition(WINDOW* win, int line, const GameInfo_t* info);
void printProgress(WINDOW* win, const int line, const GameInfo_t* info);

int printGameScreen(void* arg) {
  int exit_code = OK;
  GameData_t* data = (GameData_t*)arg;
  GameInfo_t info;

  do {
    mtx_lock(&data->controls.mutex);
    if (data->current_scr != GameScreen) {
      while (data->current_scr != GameScreen && data->controls.game_on)
        cnd_wait(&data->controls.cnd, &data->controls.mutex);
    } else {
      info = data->interface.updateCurrentState();
      printField(data->windows.field_win, &info);
      printNext(data->windows.stats_win, &info);
      printStats(data->windows.stats_win, &info);
      printContext(data->windows.context_win, &info);
      napms(REFRESH_RATE);
    }
    mtx_unlock(&data->controls.mutex);
  } while (data->controls.game_on);

  return exit_code;
}

void printField(WINDOW* win, const GameInfo_t* info) {
  box(win, 0, 0);
  if (info->field) {
    for (int y = 0; y < FIELD_LENGTH; y++) {
      int xx = 1;
      for (int x = 0; x < FIELD_WIDTH; x++, xx += 2) {
        int cell = info->field[y][x];
        wattron(win, COLOR_PAIR(cell));
        if (cell == Damaged) {
          mvwaddstr(win, y + 1, xx, "$");
        } else
          mvwaddstr(win, y + 1, xx, "  ");
        wattroff(win, COLOR_PAIR(cell));
      }
    }
  }
  wrefresh(win);
}

void printNext(WINDOW* win, const GameInfo_t* info) {
  wclear(win);
  box(win, 0, 0);
  mvwprintw(win, 1, 1, "NEXT:");
  if (info->next) {
    for (int y = 0; y < NEXTF_LENGTH; y++) {
      int xx = 2;
      for (int x = 0; x < NEXTF_WIDTH; x++, xx += 2) {
        wattron(win, COLOR_PAIR(info->next[y][x]));
        mvwaddstr(win, y + 3, xx, "  ");
        wattroff(win, COLOR_PAIR(info->next[y][x]));
      }
    }
  }
}

void printStats(WINDOW* stats_win, const GameInfo_t* info) {
  static const char* stats[4] = {"HIGHSCORE:", "SCORE:", "LEVEL:", "SPEED:"};
  const int vals[4] = {info->high_score, info->score, info->level, info->speed};

  for (int i = 0, y = 8; i < 4; i++, y += 2) {
    mvwprintw(stats_win, y, 1, "%s", stats[i]);
    mvwprintw(stats_win, y + 1, 1, "%d", vals[i]);
  }
  wrefresh(stats_win);
}

void printContext(WINDOW* context_win, const GameInfo_t* info) {
  printCondition(context_win, 1, info);
  printProgress(context_win, 2, info);
  wrefresh(context_win);
}

void printCondition(WINDOW* win, const int line, const GameInfo_t* info) {
  if (info->pause) {
    mvwprintw(win, line, 2, "PAUSE");
  } else if (info->level == 0) {
    mvwprintw(win, line, 2, "GAME OVER");
    if (info->score > info->high_score)
      mvwprintw(win, line, 2, "Congratulations! New highscore!");
  } else {
    wmove(win, line, 0);
    wclrtoeol(win);
    wclear(win);
    box(win, 0, 0);
  }
}

void printProgress(WINDOW* win, const int line, const GameInfo_t* info) {
  static int prev_score = 0;
  static int prev_level = 1;
  static int counter = 0;
  static int points_earned;
  static bool level_up = false;
  if (info->score > prev_score) {
    counter = 50;
    points_earned = info->score - prev_score;
    prev_score = info->score;
  }
  if (info->level > prev_level) {
    level_up = true;
    prev_level = info->level;
  }
  if (counter > 0) {
    mvwprintw(win, line, 5, "+%d points!", points_earned);
    if (level_up) mvwprintw(win, line + 1, 5, "NEW LEVEL!");
    --counter;
  } else
    level_up = false;
}