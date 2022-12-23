/***********************************************************/
/**	\file
	\brief		OpenGL related functions in yzLib
	\details	This file must be included after <gl.h>, or error 
				at compile time. 

				OpenGL module is not an essential part of yzLib,
				so if <gl.h> is not included ahead, this file is 
				simply ignored to insure compile time safety.

				yz_vector_oengl_utils.h will include yz_vector,
				be cautious if your implementation have vector 
				implementation too.
	\author		Zhiqi Li, based on yzLib of Dr. Yizhogn Zhang
	\date		12/24/2022
*/
/***********************************************************/
#ifndef __VISUALIZATION_OPENGL_H__
#define __VISUALIZATION_OPENGL_H__

#pragma warning(push)
#pragma warning(disable: 4267)	//	disable warning of size_t int conversion


//	for x64 programs
#ifdef _WIN64
#	pragma comment(lib, "glut64.lib")
#endif

#include<zqvisualization/visualization_fbo.h>
#include<zqvisualization/visualization_vbo.h>
#include<zqvisualization/visualization_shader.h>

#include<zqvisualization/visualization_opengl_utils.h>
#include<zqvisualization/visualization_glut_window.h>
#include<zqvisualization/visualization_texture.h>
#include<zqvisualization/visualization_ascii_displayer.h>

#include<zqvisualization/visualization_vector_opengl_utils.h>

#pragma warning(pop)

#endif	