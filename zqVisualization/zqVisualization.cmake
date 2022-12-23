set(zqVisualization_DIR ${CMAKE_CURRENT_LIST_DIR})
include(${CMAKE_CURRENT_LIST_DIR}/../zqBasicMath/zqBasicMath/zqBasicMath.cmake)
get_filename_component(zqVisualization_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../ ABSOLUTE)
set(zqVisualization_INCLUDE_DIRS
	${zqVisualization_INCLUDE_DIR}
        ${zqBasicMath_INCLUDE_DIRS}
	${PROJECT_BINARY_DIR})
	
