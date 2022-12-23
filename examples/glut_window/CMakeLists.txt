# Set project_name as the directory name
file(RELATIVE_PATH project_name ${CMAKE_CURRENT_LIST_DIR}/.. ${CMAKE_CURRENT_LIST_DIR})
file(RELATIVE_PATH folder_name ${CMAKE_CURRENT_LIST_DIR}/../.. ${CMAKE_CURRENT_LIST_DIR}/..)
###############################################
# Include deps
###############################################
include(${CMAKE_CURRENT_LIST_DIR}/../../zqVisualization/zqVisualization.cmake)
zqVisualization_Deps(${project_name})
###############################################
# Add the project 
###############################################
project(${project_name})

file(GLOB  SOURCES
	"./*.h"
	"./*.cpp"
)

add_executable(${project_name} ${SOURCES})
###############################################
# Add this project to corresponding folder
###############################################
set_target_properties(${project_name} PROPERTIES FOLDER ${folder_name})
message(STATUS "added project: ${folder_name}/${project_name}")
