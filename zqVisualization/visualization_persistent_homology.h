/***********************************************************/
/**	\file
	\brief		draw persistent homology
	\details	must be included after <glew.h>
	\author		Zhiqi Li, based on yzLib of Dr. Yizhong Zhang
	\date		12/24/2013
*/
/***********************************************************/
#ifndef __VISUALIZATION_PERSISTENT_HOMOLOGY_H__
#define __VISUALIZATION_PERSISTENT_HOMOLOGY_H__

#include <iostream>
#include <fstream>
#include "GL/glew.h"
#include "GL/glut.h"
#include<zqVisualization/visualization_opengl.h>
#include<zqVisualization/visualization_opengl_utils.h>
#include<zqBasicMath/math_homology.h>
#include<zqBasicMath/math_vector.h>
#include<zqBasicMath/math_lookup_table.h>

namespace zq{	namespace homology{
	
	/// Incomplete: Only vec3f and vec3d is finished
	template<typename T>
	void DrawSimplicalComplex(
		const Simplical_Complex<T>& complex,
		const std::vector<std::vector<float>>& simplex_color =
		std::vector<std::vector<float>>{
			{1,0,0,1},
			{0,1.0,0,1},
			{0,0,1,1},
			{1,0,1,0.5},
		},
		float radius = 0.01f
	) {
		if (T::dims != 3) throw "Incomplete, only dim3 is completed";
		for (int i = 0; i < complex.SimplexNumber(); i++) {
			const Simplex<int>& simplex = complex.simplex[i];
			const std::vector<zq::Vec3f>& points = complex.points;
			if (simplex.Dim() == 1) {
				glColor4f(simplex_color[0][0], simplex_color[0][1], simplex_color[0][2], simplex_color[0][3]);
				zq::opengl::drawPointAsSphere(points[simplex.index(0)], radius);
			}
			else if(simplex.Dim() == 2) {
				glColor4f(simplex_color[1][0], simplex_color[1][1], simplex_color[1][2], simplex_color[1][3]);
				zq::opengl::drawLineSegment(points[simplex.index(0)], points[simplex.index(1)]);
			}
			else if (simplex.Dim() == 3) {
				glColor4f(simplex_color[2][0], simplex_color[2][1], simplex_color[2][2], simplex_color[2][3]);
				zq::opengl::drawTriangle(points[simplex.index(0)], points[simplex.index(1)], points[simplex.index(2)]);
			}
			else if (simplex.Dim() == 4) {
				glColor4f(simplex_color[2][0], simplex_color[2][1], simplex_color[2][2], simplex_color[2][3]);
				for (int i = 0; i < 4; i++) {
					std::vector<int> tem;
					for (int j = 0; j < 4; j++) {
						if (j != i) {
							tem.push_back(simplex.index(i));
						}
					}
					zq::opengl::drawTriangle(points[tem[0]], points[tem[1]], points[tem[2]]);
				}
			}
		}
	}
	
	void DrawPersistentBarCode(
		const std::vector<std::pair<float, float>>& epsilon_interval,
		const std::vector<int>& feture_type,
		float max_epsilon,
		float display_max_epsilon,
		float win_x,
		float win_y,
		float win_ox,
		float win_oy
	) {
		//for (int i = 0; i < feture_type.size(); i++) {
		//	printf("%d,start_epsilon, end_epsilon:%f %f\n", feture_type[i], epsilon_interval[i].first, epsilon_interval[i].second);
		//}
		/// First, split feture type
		std::vector<std::vector<std::pair<float, float>>> epsilon_interval_splited;
		std::vector<int> type_splited;
		std::set<int> type_exist;
		for (int i = 0; i < epsilon_interval.size(); i++) {
			auto iter = type_exist.find(feture_type[i]);
			if (iter == type_exist.end()) {
				type_exist.insert(feture_type[i]);
				type_splited.push_back(feture_type[i]);
				epsilon_interval_splited.push_back(std::vector<std::pair<float, float>>());
				if(epsilon_interval[i].second!= epsilon_interval[i].first)
					epsilon_interval_splited[epsilon_interval_splited.size() - 1].push_back(epsilon_interval[i]);
			}
			else {
				for (int j = 0; j < type_splited.size(); j++) {
					if (type_splited[j] == feture_type[i]) {
						if (epsilon_interval[i].second != epsilon_interval[i].first)
							epsilon_interval_splited[j].push_back(epsilon_interval[i]);
						break;
					}
				}
			}
		}
		//for (int j = 0; j < epsilon_interval_splited.size(); j++) {
		//	printf("Type is :%d", type_splited[j]);
		//	for (int i = 0; i < epsilon_interval_splited[j].size(); i++) {
		//		printf("start_epsilon, end_epsilon:%f %f\n", epsilon_interval_splited[j][i].first, epsilon_interval_splited[j][i].second);
		//	}
		//}
		/// Then split the y
		//float max_splited = -1;
		//for (int i = 0; i < epsilon_interval_splited.size(); i++)
		//	if (epsilon_interval_splited[i].size() > max_splited)
		//		max_splited = epsilon_interval_splited[i].size();
		float interval_y = win_y / type_splited.size();
		float start_x = win_ox;
		float start_y = win_oy;
		float rate = win_x / display_max_epsilon;
		for (int i = 0; i < type_splited.size(); i++) {
			float sub_interval_y = interval_y / myMax(epsilon_interval_splited[i].size(),10);
			float color[3];
			zq::opengl::getSequentialDisplayColor(color, type_splited[i]);
			glColor3f(color[0], color[1], color[2]);
			for (int j = 0; j < epsilon_interval_splited[i].size(); j++) {
				float start_line_x=start_x+ epsilon_interval_splited[i][j].first* rate;
				float end_line_x = start_x + epsilon_interval_splited[i][j].second * rate;
				float start_line_y = start_y + j * sub_interval_y , end_line_y=start_y+j* sub_interval_y;
				std::vector<Vec3f> bar{
					zq::Vec2f(start_line_x,start_line_y),
					zq::Vec2f(end_line_x,end_line_y),
					zq::Vec2f(end_line_x,end_line_y + sub_interval_y*0.9),
					zq::Vec2f(start_line_x, start_line_y + sub_interval_y * 0.9)
				};
				glBegin(GL_TRIANGLES);
				for (int i = 0; i < 3; i++) {
					glVertex3f(bar[i][0], bar[i][1], bar[i][2]);
				}
				for (int i = 0; i < 3; i++) {
					glVertex3f(bar[(i+2)%4][0], bar[(i + 2) % 4][1], bar[(i + 2) % 4][2]);
				}
				glEnd();
			}
			start_y += interval_y;
		}
	}

	void DrawPersistentDiagram(
		const std::vector<std::pair<float, float>>& epsilon_interval,
		const std::vector<int>& feture_type,
		float max_epsilon,
		float display_max_epsilon,
		float radius,
		float win_x,
		float win_y,
		float win_ox,
		float win_oy
	) {
		//for (int i = 0; i < feture_type.size(); i++) {
		//	printf("%d,start_epsilon, end_epsilon:%f %f\n", feture_type[i], epsilon_interval[i].first, epsilon_interval[i].second);
		//}
		/// First, split feture type
		std::vector<std::vector<std::pair<float, float>>> epsilon_interval_splited;
		std::vector<int> type_splited;
		std::set<int> type_exist;
		for (int i = 0; i < epsilon_interval.size(); i++) {
			auto iter = type_exist.find(feture_type[i]);
			if (iter == type_exist.end()) {
				type_exist.insert(feture_type[i]);
				type_splited.push_back(feture_type[i]);
				epsilon_interval_splited.push_back(std::vector<std::pair<float, float>>());
				epsilon_interval_splited[epsilon_interval_splited.size() - 1].push_back(epsilon_interval[i]);
			}
			else {
				for (int j = 0; j < type_splited.size(); j++) {
					if (type_splited[j] == feture_type[i]) {
						epsilon_interval_splited[j].push_back(epsilon_interval[i]);
						break;
					}
				}
			}
		}
		//for (int j = 0; j < epsilon_interval_splited.size(); j++) {
		//	printf("Type is :%d", type_splited[j]);
		//	for (int i = 0; i < epsilon_interval_splited[j].size(); i++) {
		//		printf("start_epsilon, end_epsilon:%f %f\n", epsilon_interval_splited[j][i].first, epsilon_interval_splited[j][i].second);
		//	}
		//}
		/// Then split the y
		float ratex = win_x / display_max_epsilon;
		float ratey = win_x / display_max_epsilon;
		for (int i = 0; i < type_splited.size(); i++) {
			float color[3];
			zq::opengl::getSequentialDisplayColor(color, type_splited[i]);
			glColor3f(color[0], color[1], color[2]);
			for (int j = 0; j < epsilon_interval_splited[i].size(); j++) {
				zq::opengl::drawCircle(
					zq::Vec2f(
						epsilon_interval_splited[i][j].first*ratex+win_ox,
						epsilon_interval_splited[i][j].second * ratey + win_oy
					),
					radius);
			}
		}
		float color[3];
		zq::opengl::getSequentialDisplayColor(color, type_splited.size());
		glColor3f(color[0], color[1], color[2]);
		zq::opengl::drawLineSegment(
			zq::Vec2f(win_ox, win_oy),
			zq::Vec2f(win_ox + win_x, win_oy + win_y)
		);
	}

}}	


#endif	
