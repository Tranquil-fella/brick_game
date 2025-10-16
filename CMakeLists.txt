cmake_minimum_required(VERSION 3.16)
project(BrickGame VERSION 1.0 LANGUAGES C CXX)

# Основные настройки
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Определение корневой директории проекта
get_filename_component(ROOT_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
set(PROJECT_ROOT_DIR "${ROOT_PROJECT_DIR}" CACHE PATH "Root directory of the project")

# Основные пути проекта
set(INCLUDE_DIR "${PROJECT_ROOT_DIR}/include")
set(SRC_DIR "${PROJECT_ROOT_DIR}/src")

# ОПЦИИ ДЛЯ ТИПОВ СБОРКИ
option(BUILD_TESTS "Build tests" OFF)
option(BUILD_DESKTOP_GUI "Build desktop GUI" ON)
option(BUILD_CLI_GUI "Build CLI GUI" ON)
option(BUILD_SNAKE_LIB "Build libsnake" ON)
option(BUILD_TETRIS_LIB "Build libtetris" ON)

# Основная опция типа сборки
set(BUILD_TYPE "Release" CACHE STRING "Build type (Debug, Release, Coverage)")

# Установить допустимые значения для BUILD_TYPE
set_property(CACHE BUILD_TYPE PROPERTY STRINGS Debug Release Coverage)

# АВТОМАТИЧЕСКАЯ СТРУКТУРА ПАПОК build/<type>/
set(BUILD_BASE_DIR "${PROJECT_ROOT_DIR}/build")
set(BUILD_DIR "${BUILD_BASE_DIR}/${BUILD_TYPE}")

# Поддиректории внутри папки сборки
set(LIBS_DIR "${BUILD_DIR}/lib")
set(BIN_DIR "${BUILD_DIR}/bin")
set(GCOV_DIR "${BUILD_DIR}/gcov_misc")
set(REPORT_DIR "${BUILD_DIR}/coverage_report")

# Вывод информации
message(STATUS "Project root: ${PROJECT_ROOT_DIR}")
message(STATUS "Include directory: ${INCLUDE_DIR}")
message(STATUS "Source directory: ${SRC_DIR}")
message(STATUS "Build base directory: ${BUILD_BASE_DIR}")
message(STATUS "Build directory: ${BUILD_DIR}")
message(STATUS "Build type: ${BUILD_TYPE}")

# Настройки компилятора на основе типа сборки
if(BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    message(STATUS "Building in DEBUG mode")
    
elseif(BUILD_TYPE STREQUAL "Release")
    set(CMAKE_BUILD_TYPE Release)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
    message(STATUS "Building in RELEASE mode")
    
elseif(BUILD_TYPE STREQUAL "Coverage")
    set(CMAKE_BUILD_TYPE Debug)
    # ИСПОЛЬЗУЕМ GCOV_DIR ДЛЯ СОХРАНЕНИЯ .gcda ФАЙЛОВ
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage ")
    set(BUILD_TESTS ON CACHE BOOL "Build tests" FORCE)  # Автоматически включаем тесты для coverage
    message(STATUS "Building in COVERAGE mode")
    message(STATUS "Coverage data will be stored in: ${GCOV_DIR}")
    
endif()

# Отдельные опции для санитайзеров
option(ENABLE_SANITIZER "Enable AddressSanitizer" OFF)

if(ENABLE_SANITIZER)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize=undefined")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined")
    
    # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=thread")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
    message(STATUS "AddressSanitizer enabled")
endif()


# Создание директорий после определения BUILD_DIR
file(MAKE_DIRECTORY ${LIBS_DIR})
file(MAKE_DIRECTORY ${BIN_DIR})
file(MAKE_DIRECTORY ${GCOV_DIR})  # Создаем папку для coverage данных

# Добавление подпроектов
if(BUILD_SNAKE_LIB)
    add_subdirectory(src/brick_game/snake)
endif()

if(BUILD_TETRIS_LIB)
    add_subdirectory(src/brick_game/tetris)
endif()

if(BUILD_CLI_GUI)
    add_subdirectory(src/gui/cli)
endif()

if(BUILD_DESKTOP_GUI)
    add_subdirectory(src/gui/desktop)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests/tetris)
    add_subdirectory(tests/snake)
    
    add_custom_target(tests_all
        DEPENDS tetris_tests snake_tests_all
        COMMENT "Building all tests"
    )
endif()

# Установка
install(DIRECTORY ${BIN_DIR}/ DESTINATION bin)
install(DIRECTORY ${LIBS_DIR}/ DESTINATION lib)

# Покрытие кода - только если в режиме Coverage
if(BUILD_TYPE STREQUAL "Coverage")
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)
    
    if(LCOV_PATH AND GENHTML_PATH)
       add_custom_target(coverage
            COMMAND ctest -Q --test-dir ./..
            COMMAND /bin/sh -c 'find .. -name "*.gcno" | xargs -I {} cp {} ${GCOV_DIR}/'  || true
            COMMAND /bin/sh -c 'find .. -name "*.gcda" | xargs -I {} cp  {} ${GCOV_DIR}/'  || true
            COMMAND ${LCOV_PATH} -q --capture --directory ${GCOV_DIR} --output-file coverage.info --branch-coverage --ignore-errors inconsistent,path,mismatch,empty,negative 2>/dev/null
            COMMAND ${LCOV_PATH} -q --remove coverage.info '*/tests/*' '*/usr/*' --output-file coverage.info --branch-coverage --ignore-errors inconsistent,path,mismatch,empty,negative 2>/dev/null || true
            COMMAND ${GENHTML_PATH} coverage.info  -q --output-directory ${REPORT_DIR} --branch-coverage --ignore-errors inconsistent,path,mismatch,empty,negative 2>/dev/null || true
            WORKING_DIRECTORY ${BUILD_DIR}
            COMMAND open ${REPORT_DIR}/index.html
            COMMENT "Running tests and generating coverage report"
        )
    else()
        message(WARNING "lcov or genhtml not found. Coverage report will not be available.")
    endif()
endif()

add_custom_target(build_all COMMENT "Building all targets")

if(BUILD_SNAKE_LIB AND TARGET snake)
    add_dependencies(build_all snake)
endif()

if(BUILD_TETRIS_LIB AND TARGET tetris)
    add_dependencies(build_all tetris)
endif()

if(BUILD_CLI_GUI AND TARGET cli_gui)
    add_dependencies(build_all cli_gui)
endif()

if(BUILD_DESKTOP_GUI AND TARGET desktop_gui)
    add_dependencies(build_all desktop_gui)
endif()

if(BUILD_TESTS AND TARGET tests_all)
    add_dependencies(build_all tests_all)
endif()