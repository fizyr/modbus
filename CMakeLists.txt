cmake_minimum_required(VERSION 3.1)
project(modbus)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -Wpedantic)

find_package(catkin REQUIRED COMPONENTS)
find_package(Boost REQUIRED COMPONENTS system thread)

catkin_package(
	INCLUDE_DIRS include
	LIBRARIES ${PROJECT_NAME}
	DEPENDS Boost
)

include_directories(include/${PROJECT_NAME})
include_directories(SYSTEM
	${catkin_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME}
	src/client.cpp
	src/error.cpp
)

add_executable(${PROJECT_NAME}_test_client
	src/test.cpp
)

target_link_libraries(${PROJECT_NAME}
	${catkin_LIBRARIES}
	${Boost_LIBRARIES}
)

target_link_libraries(${PROJECT_NAME}_test_client
	${PROJECT_NAME}
)

install(TARGETS ${PROJECT_NAME}
	ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY include/${PROJECT_NAME}/
	DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
	FILES_MATCHING PATTERN "*.hpp"
)
