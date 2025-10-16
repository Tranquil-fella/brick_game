# Tetris library
project(tetris LANGUAGES C)

# Исходные файлы
set(TETRIS_SOURCES
    controller.c
    game_data.c
    highscore_keeper.c
    movement_queue.c
    tetromino.c
    tetromino_mover.c
)

# Заголовочные файлы
set(TETRIS_HEADERS
    controller.h
    game_data.h
    highscore_keeper.h
    movement_queue.h
    tetromino.h
    tetromino_mover.h
    backend.h
)

# Создание библиотеки
add_library(tetris SHARED ${TETRIS_SOURCES})

# Настройки компилятора
target_compile_options(tetris PRIVATE
    -Wall
    -Werror
    -Wextra
    -fPIC
)

# Директории включения
target_include_directories(tetris PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${INCLUDE_DIR}/brick_game
    ${INCLUDE_DIR}/brick_game/tetris
)

# Установка библиотеки
set_target_properties(tetris PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${LIBS_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${LIBS_DIR}
)