# Snake tests
project(snake_tests LANGUAGES CXX)

find_package(GTest REQUIRED)

# Исходные файлы тестов
file(GLOB SNAKE_TEST_SOURCES "*_test.cc")

# Object library для переиспользования кода
add_library(snake_test_objects OBJECT
    ${SRC_DIR}/brick_game/snake/backend.cc
    ${SRC_DIR}/brick_game/snake/fsm.cc
    ${SRC_DIR}/brick_game/snake/snake_model.cc
    ${SRC_DIR}/brick_game/snake/snake.cc
)

# # Современные настройки компилятора
# target_compile_options(snake_test_objects PRIVATE
#     $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
#         -Wall
#         -Wextra
#         -Werror
#     >
# )



# Современное указание путей включения
target_include_directories(snake_test_objects 
    PUBLIC
        ${SRC_DIR}/brick_game/snake
        ${INCLUDE_DIR}/brick_game/snake
        ${INCLUDE_DIR}/brick_game
)

# Создание тестов и сбор списка целей
set(SNAKE_TEST_TARGETS)
foreach(test_src ${SNAKE_TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    list(APPEND SNAKE_TEST_TARGETS ${test_name})
    
    add_executable(${test_name} 
        ${test_src}
        $<TARGET_OBJECTS:snake_test_objects>
    )
    
    # Современные настройки компилятора
    target_compile_options(${test_name} PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
            -Wall
            -Wextra
            -Werror
        >
    )
    
    # Пути включения
    target_include_directories(${test_name} PRIVATE
        ${INCLUDE_DIR}/brick_game/snake
        ${INCLUDE_DIR}/brick_game
        ${GTEST_INCLUDE_DIRS}
    )
    
    # Линковка
    target_link_libraries(${test_name} PRIVATE
        GTest::gtest
        GTest::gtest_main
    )
    
    # Добавление теста
    add_test(NAME ${test_name} COMMAND ${test_name})
    
    # Выходной путь
    set_target_properties(${test_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
    )
endforeach()

# Создать цель для всех snake тестов - ЗАВИСЕТЬ ОТ ЦЕЛЕЙ
add_custom_target(snake_tests_all 
    DEPENDS ${SNAKE_TEST_TARGETS}
    COMMENT "Building all Snake tests: ${SNAKE_TEST_TARGETS}"
)