# Snake library
project(snake LANGUAGES CXX)

# Исходные файлы
set(SNAKE_SOURCES
    backend.cc
    fsm.cc
    snake.cc
    snake_model.cc
)

set(SNAKE_HEADERS
    backend.h
    controler.h
    events.h
    field.h
    fsm.h
    input_mapping.h
    mediator.h
    move_timer.h
    observable.h
    simple_file_storage.h
    snake.h
    snake_model.h
    stats_keeper.h
)

# Создание библиотеки
add_library(snake SHARED ${SNAKE_SOURCES})

# Настройки компилятора
target_compile_options(snake PRIVATE
    -Wall
    -Werror
    -Wextra
    -fPIC
)

# Директории включения - используем глобальные переменные
target_include_directories(snake PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${INCLUDE_DIR}/brick_game
    ${INCLUDE_DIR}/brick_game/snake
)

# Установка библиотеки
set_target_properties(snake PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${LIBS_DIR}
    ARCHIVE_OUTPUT_DIRECTORY ${LIBS_DIR}
)

