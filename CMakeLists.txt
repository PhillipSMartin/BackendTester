cmake_minimum_required(VERSION 3.0.0)
project(BackendTester VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(BackendTester 
    src/AMQClient.cpp
    src/AMQTopic.cpp
    src/Dashboard.cpp
    src/JsonTest.cpp 
    src/JsonTreeStore.cpp
    src/JsonViewer.cpp
    src/Logger.cpp 
    src/Parameters.cpp 
    src/TemplateChooser.cpp
    src/TemplateListStore.cpp
    src/TemplateMap.cpp
    src/TopicChooser.cpp)

target_link_directories(BackendTester PUBLIC lib ../Bridge/backend/lib)

target_include_directories(BackendTester 
PRIVATE
    include
PUBLIC 
     ../Bridge/backend/include/amq
     ../Bridge/backend/include/apr-1
     ../Bridge/backend/include
    /usr/include/gtk-3.0 
    /usr/include/atk-1.0 
    /usr/include/at-spi2-atk/2.0 
    /usr/include/pango-1.0
    /usr/include/gio-unix-2.0/ 
    /usr/include/cairo 
    /usr/include/gdk-pixbuf-2.0 
    /usr/include/glib-2.0 
    /usr/lib64/glib-2.0/include 
    /usr/include/at-spi-2.0 
    /usr/include/dbus-1.0 
    /usr/lib64/dbus-1.0/include 
    /usr/include/harfbuzz 
    /usr/include/freetype2 
    /usr/include/fribidi 
    /usr/include/libpng15 
    /usr/include/uuid 
    /usr/include/pixman-1 
    /usr/include/libdrm
    )

target_link_libraries(BackendTester 
PUBLIC
    logging
    activemq-cpp 
    apr-1
    gtk-3 
    gdk-3 
    atk-1.0 
    gio-2.0 
    pangocairo-1.0 
    gdk_pixbuf-2.0 
    cairo-gobject 
    pango-1.0 
    cairo 
    gobject-2.0 
    glib-2.0
    )

configure_file(src/Event.tb.templates.txt Event.tb.templates.txt COPYONLY)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
