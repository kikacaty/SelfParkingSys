cmake_minimum_required(VERSION 2.8)
project( Detector )
find_package( OpenCV REQUIRED )
add_executable( Detector detector.cpp )
target_link_libraries( Detector ${OpenCV_LIBS} )
