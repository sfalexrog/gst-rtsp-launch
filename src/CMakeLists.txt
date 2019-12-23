add_executable(gst-rtsp-launch gst-rtsp-launch.cpp)
target_link_libraries(gst-rtsp-launch PUBLIC GST_RTSP_SERVER)

install(TARGETS gst-rtsp-launch
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
)
