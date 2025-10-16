
#include "test.h"

int main() {
  int failed = 0, total = 0;

  Suite *Tests[] = {controller_suite(), queue_suite(),
                    // highscore_suite(),
                    tetromino_suite(), tetromino_mover_suite(), NULL};
  for (int i = 0; Tests[i] != NULL; i++) {
    SRunner *sr = srunner_create(Tests[i]);
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_run_all(sr, CK_NORMAL);

    int current_failed = srunner_ntests_failed(sr);
    if (current_failed > 0) {
      failed += current_failed;
      printf(DARK_RED "================ FAILED =================\n" NOCOLOR);
    } else {
      printf(LIGHT_GREEN "================ SUCCESS ================\n" NOCOLOR);
    }

    total += srunner_ntests_run(sr);
    srunner_free(sr);
  }

  printf("\nTEST RESULT " LIGHT_GREEN "\nTOTAL:\t%d" DARK_RED
         "\nFAILED:\t"
         "%d" NOCOLOR "\n",
         total, failed);

  return failed == 0 ? 0 : 1;
}
