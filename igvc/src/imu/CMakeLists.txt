add_executable(imu main.cpp)
add_dependencies(imu igvc_msgs_gencpp)
target_link_libraries(imu ${catkin_LIBRARIES} SerialPort)
