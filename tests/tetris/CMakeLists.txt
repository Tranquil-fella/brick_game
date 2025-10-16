# Tetris tests
project(tetris_tests LANGUAGES C)

find_library(CHECK_LIB check)
find_library(SUBUNIT_LIB subunit)
    
if(NOT CHECK_LIB OR NOT SUBUNIT_LIB)
    message(WARNING "Check library not found. Tetris tests will not be built.")
    return()
endif()

# Исходные файлы тестов
set(TETRIS_TEST_SOURCES
    controller_test.c
    mv_queue_test.c
    tetr_mover_test.c
    tetromino_test.c
    test.c
)

set(TETRIS_SOURCES_DIRECT
    ${SRC_DIR}/brick_game/tetris/controller.c
    ${SRC_DIR}/brick_game/tetris/game_data.c
    ${SRC_DIR}/brick_game/tetris/highscore_keeper.c
    ${SRC_DIR}/brick_game/tetris/movement_queue.c
    ${SRC_DIR}/brick_game/tetris/tetromino.c
    ${SRC_DIR}/brick_game/tetris/tetromino_mover.c
)

# Создание тестового исполняемого файла
add_executable(tetris_tests 
    ${TETRIS_TEST_SOURCES}
    ${TETRIS_SOURCES_DIRECT}
    )

# Настройки компилятора
target_compile_options(tetris_tests PRIVATE
    -Wall
    -Werror
    -Wextra
)

# if(ENABLE_COVERAGE)
#     target_compile_options(tetris_tests PRIVATE -fprofile-arcs -ftest-coverage)
# endif()

# Директории включения - используем глобальные переменные
target_include_directories(tetris_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${INCLUDE_DIR}/brick_game/tetris
    ${INCLUDE_DIR}/brick_game
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Связывание библиотек
target_link_libraries(tetris_tests PRIVATE
    ${CHECK_LIB}
    ${SUBUNIT_LIB}
    m
)

# Добавление тестов
add_test(NAME tetris_all_tests COMMAND tetris_tests)

# Установка выходного файла
set_target_properties(tetris_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
)