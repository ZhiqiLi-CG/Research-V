set(ResearchV_DIR ${CMAKE_CURRENT_LIST_DIR})
include(${ResearchV_DIR}/../zqVisualization/zqVisualization.cmake)
get_filename_component(ResearchV_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/../ ABSOLUTE)
set(ResearchV_INCLUDE_DIRS
	${ResearchV_INCLUDE_DIR}
	${zqVisualization_INCLUDE_DIRS}
	${PROJECT_BINARY_DIR})

macro(Set_ResearchV_Options)
	#----------- Set Option-------
	option(CUDA_ENABLE "whether use cuda" OFF)

endmacro()

macro(Set_ResearchV_Env)
	set(ResearchV_INCLUDE_DIRS
		${ResearchV_INCLUDE_DIR}
		${zqVisualization_INCLUDE_DIRS}
		${PROJECT_BINARY_DIR})
	# ---------- CUDA ----------
	if(CUDA_ENABLE)
		find_package(CUDA  REQUIRED)
		if(CUDA_FOUND)
			message(STATUS "CUDA version: ${CUDA_VERSION}")
			include_directories(${CUDA_INCLUDE_DIRS})  
			link_directories(${CUDA_TOOLKIT_ROOT_DIR}/lib/x64)
			set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} --extended-lambda")
		else()
			message(ERROR "CUDA not found, you need to install CUDA, or disable CUDA_ENABL")
		endif()
	endif()
	#--------------- Config-----------------
	set(RESEARCHV_ENABLE_CUDA ${CUDA_ENABLE})
	set(RESEARCHM_ENABLE_CUDA ${CUDA_ENABLE})
	configure_file(
		"${ResearchM_DIR}/ResearchM_config.h.in"
		"${PROJECT_BINARY_DIR}/ResearchM_config.h")
	message(STATUS "${PROJECT_BINARY_DIR}/ResearchM_config.h created")
	configure_file(
		"${ResearchV_DIR}/ResearchV_config.h.in"
		"${PROJECT_BINARY_DIR}/ResearchV_config.h")
	message(STATUS "${PROJECT_BINARY_DIR}/ResearchV_config.h created")
endmacro()

macro(ResearchV_Deps project_name)
	set(CMAKE_CXX_STANDARD 17)
	link_directories(${ResearchV_DIR}/../deps/lib/win64)	
	if(NOT EXISTS ${ResearchV_DIR}/../deps/inc)
		message(FATAL_ERROR "deps not found, update submodule to get zqLibraryDeps")
	else()
		message(STATUS "zqLibraryDeps found as submodule")
		include_directories(${ResearchV_DIR}/../deps/inc)
		if(${MSVC}) 
			set(BIN_PATH "${ResearchV_DIR}/../deps/bin/win64;${ResearchV_DIR}/../deps/bin/win32")
		endif()
	endif()
	# begin to find opengl

	if(${MSVC})
		link_directories(${ResearchV_DIR}/../deps/lib/win64)
		# we already included OpenGL, so don't need to do anything more
	else()
		# add fmt lib
		find_package(fmt REQUIRED)
		target_link_libraries(${project_name} ${FMT_LIBRARIES})
		#target_link_libraries(${project_name} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
		#target_link_libraries(${project_name} ${GLEW_LIBRARIES})
		find_package(OpenGL REQUIRED)
		find_package(GLUT REQUIRED)
		find_package(GLEW REQUIRED)
		target_link_libraries(${project_name} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
		target_link_libraries(${project_name} ${GLEW_LIBRARIES})
	endif()
endmacro()