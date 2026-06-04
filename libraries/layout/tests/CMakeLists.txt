mssm_add_gtest(layout_sizebound_tests
    SOURCES
        sizebound_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../sizebound.cpp
)

mssm_add_gtest(window_event_mask_tests
    SOURCES
        event_mouse_test.cpp
        ${LIBRARY_ABSOLUTE_PATH}/window/windowevents.cpp
)

target_include_directories(window_event_mask_tests PRIVATE
    ${LIBRARY_ABSOLUTE_PATH}/window
)
