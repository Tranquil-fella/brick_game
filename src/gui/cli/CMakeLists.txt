# CLI GUI executable
project(cli_gui LANGUAGES C)

# Поиск ncurses
find_library(NCURSES_LIB ncurses)
find_library(MENU_LIB menu)

if(NOT NCURSES_LIB OR NOT MENU_LIB)
    message(WARNING "ncurses or menu library not found. CLI GUI will not be built.")
    return()
endif()

# Исходные файлы
set(CLI_SOURCES
    frontend.c
    game_field.c
    games_finder.c
    input.c
    interface_loader.c
    menu_navigation.c
    menus.c
    path_utils.c
)

# set(CLI_HEADERS
#     frontend.h
#     game_field.h
#     games_finder.h
#     input.h
#     interface_loader.h
#     path_utils.h
#     menus.h
# )

# Создание исполняемого файла
add_executable(cli_gui ${CLI_SOURCES})

# Настройки компилятора
target_compile_options(cli_gui PRIVATE
    -Wall
    -Werror
    -Wextra
)

# Директории включения - используем глобальные переменные
target_include_directories(cli_gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${INCLUDE_DIR}/brick_game
    ${INCLUDE_DIR}/gui/cli

)

target_link_libraries(cli_gui PRIVATE
    ${MENU_LIB} 
    ${NCURSES_LIB}
)

# Установка выходного файла
set_target_properties(cli_gui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
    OUTPUT_NAME "BrickGameCLI"
)