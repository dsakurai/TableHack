
add_library(html_clipboard)

target_sources(html_clipboard
    PRIVATE
    clipboard_win32.cpp
    clipboard_win32.h
)

target_include_directories(html_clipboard
    PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(html_clipboard PUBLIC Qt6::Core)
