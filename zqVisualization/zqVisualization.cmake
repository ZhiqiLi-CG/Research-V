set(zqVisualization_DIR ${CMAKE_CURRENT_LIST_DIR})
include(${CMAKE_CURRENT_LIST_DIR}/../Research-M/ResearchM/ResearchM.cmake)
get_filename_component(zqVisualization_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../ ABSOLUTE)
set(zqVisualization_INCLUDE_DIRS
	${zqVisualization_INCLUDE_DIR}
        ${ResearchM_INCLUDE_DIRS}
	${PROJECT_BINARY_DIR})

	
