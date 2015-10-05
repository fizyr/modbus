cmake_minimum_required(VERSION 2.8.3)
project(modbus)

find_package(catkin REQUIRED COMPONENTS
	dr_base
)

find_package(Boost REQUIRED COMPONENTS system thread)

catkin_package(
	INCLUDE_DIRS include
	LIBRARIES ${PROJECT_NAME}
	DEPENDS Boost
)

dr_include_directories(
	include/${PROJECT_NAME}
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
