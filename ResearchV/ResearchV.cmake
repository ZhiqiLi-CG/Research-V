set(ResearchM_DIR ${CMAKE_CURRENT_LIST_DIR})
include(${ResearchM_DIR}/../zqVisualization/zqVisualization.cmake)
include_directories(${zqVisualization_INCLUDE_DIRS})
get_filename_component(ResearchV_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../ ABSOLUTE)
set(ResearchV_INCLUDE_DIRS
	${ResearchV_INCLUDE_DIR}
	${PROJECT_BINARY_DIR})
	
