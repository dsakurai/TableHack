set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_subdirectory(html_clipboard)

add_executable(example)

target_sources(example
    PRIVATE
    main.cpp
    MinimalTableWidget.cpp
    MinimalTableWidget.h
    TableWidget.h
    TableWidget.cpp
)

target_link_libraries(example PUBLIC Qt6::Widgets html_clipboard)
