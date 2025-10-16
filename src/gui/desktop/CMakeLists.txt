
project(desktop_gui LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)  
set(CMAKE_AUTORCC ON)

# Поиск Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Исходные файлы
set(DESKTOP_SOURCES
    main.cpp
    mainwidget.cpp
    gamescreen.cpp
    brickview.cpp
    ${INCLUDE_DIR}/gui/desktop/mainwidget.h   
    ${INCLUDE_DIR}/gui/desktop/gamescreen.h   
    ${INCLUDE_DIR}/gui/desktop/brickview.h    
    ${INCLUDE_DIR}/gui/desktop/confirmexit.h 
)

# UI файлы
set(DESKTOP_FORMS
    mainwidget.ui
    gamescreen.ui
)

# Создание исполняемого файла
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(desktop_gui
        MANUAL_FINALIZATION
        ${DESKTOP_SOURCES}
        ${DESKTOP_FORMS}
        
    )
else()
    add_executable(desktop_gui
        ${DESKTOP_SOURCES}
        ${DESKTOP_FORMS}
    )
endif()

# Директории включения 
target_include_directories(desktop_gui PRIVATE                              
    ${INCLUDE_DIR}/gui/desktop           
    ${INCLUDE_DIR}/brick_game
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Связывание библиотек
target_link_libraries(desktop_gui PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
)

# Установка выходного файла
set_target_properties(desktop_gui PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
    OUTPUT_NAME "BrickGameDesktop"
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(desktop_gui)
endif()