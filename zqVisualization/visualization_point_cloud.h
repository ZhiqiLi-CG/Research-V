/***********************************************************/
/**	\file
	\brief		OpenGL Utilities
	\details	util functions to make using OpenGL easier.
				must include after <glut.h>.
	\author		Zhiqi Li, based on yzLib of Dr. Yizhong Zhang
	\date		5/23/2012
*/
/***********************************************************/
#ifndef __VISUALIZATION_POINT_CLOUD_H__
#define __VISUALIZATION_POINT_CLOUD_H__

#pragma  warning(disable:4996)

#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <list>
#include <sstream>
#include <math.h>
#include <stdarg.h>
#include <zqBasicUtils/utils_json.h>
#include <zqBasicUtils/utils_lookup_table.h>
#include <zqBasicMath/math_utils.h>
#include <zqBasicMath/math_vector.h>
#include <zqBasicMath/math_matrix.h>
#include <zqVisualization/visualization_point_color.h>
#include <zqVisualization/visualization_color_data.h>
#include <zqBasicMath/math_io.h>
#include <zqBasicUtils/utils_io.h>
#include <map>
#include <set>
#include <vector>

namespace zq {
	namespace vis {
		// In this class, every point have several attributes, and accoriding to this attribute,
		// we take different draw strategies
		template<int d>
		class PointCloudDraw {
			Typedef_VectorDDi(d)
		public:
			// 0
			Array<VecD> base_point;
			std::string base_name;
			std::string base_path;
			float  base_color[3];
			bool use_base=true;
			std::map<std::string, char> base_key_map{
				{"use_base",'q'}
			};
			std::map<char, std::string> base_key_inverse_map{
				{'q',"use_base"}
			};
			// 1
			std::vector<std::string> vector_name;
			std::map<std::string, Array<VecD>> vector_list;
			std::map<std::string, float*> vector_color;
			std::map<std::string, real> vector_ratio;
			std::map<std::string, bool> vector_draw;
			std::map<std::string, std::string> vector_path;
			int vector_cur=-1;
			std::map<std::string, char> vector_key_map{
				{"move_cur_left",'a'},
				{"move_cur_right",'d'},
				{"enlarge_ratio",'w'},
				{"shrink_ratio",'s'},
				{"draw_vector",'q'}
			};
			std::map<char, std::string> vector_key_inverse_map{
					{'a',"move_cur_left"},
					{'d',"move_cur_right"},
					{'w',"enlarge_ratio"},
					{'s',"shrink_ratio"},
					{'q',"draw_vector"}
			};
			// 2
			std::vector<std::string> color_name;
			std::map<std::string, Array<real>> color_list;
			std::map<std::string, real> color_transparent;
			std::map<std::string, RealColor*> color_type;
			std::map<std::string, bool> color_draw;
			std::map<std::string, std::string> color_path;

			int color_cur = -1;
			std::map<std::string, char> color_key_map{
				{"move_cur_left",'a'},
				{"move_cur_right",'d'},
				{"use_this_color",'q'}
			};
			std::map<char, std::string> color_key_inverse_map{
					{'a',"move_cur_left"},
					{'d',"move_cur_right"},
					{'q',"use_this_color"}
			};
			/// 3
			Array<Vec<int,3>> triangle;
			std::string triangle_name;
			std::string triangle_path;
			bool have_triangle=false;
			bool use_triangle;
			std::map<std::string, char> triangle_key_map{
				{"use_triangle",'q'}
			};
			std::map<char, std::string> triangle_key_inverse_map{
				{'q',"use_triangle"}
			};
			int now_cur = 0;
			bool use_this = false;
			std::map<std::string, char> this_key_map{
					{"switch_data",'r'},
					{"click_this",'e'}
			};
			std::map<char, std::string> this_key_inverse_map{
					{'r',"switch_data"},
					{'e',"click_this"}
			};
			public:
				void readJson(json j) {
					///https://const.net.cn/719.html
					/// Here read the json and add attribute
					std::cout << j.dump(2) << std::endl;
					for (const auto& item : j.items()) {
						std::cout << item.key() << std::endl;
						if (item.key() != "type" && item.key() != "dim") {
							if (j[item.key()]["type"] == "base") {
								base_name = item.key();
								base_path = j[item.key()]["name"];
							}
							else if (j[item.key()]["type"] == "vector") {
								vector_name.push_back(item.key());
								vector_list.insert(std::pair<std::string, Array<VecD>>(item.key(), Array<VecD>()));
								vector_color.insert(std::pair<std::string, float*>(item.key(),new float[3]));
								vector_ratio.insert(std::pair<std::string, real>(item.key(), 1));
								vector_draw.insert(std::pair<std::string, bool>(item.key(), false));
								vector_path.insert(std::pair<std::string, std::string>(item.key(), j[item.key()]["name"]));
							}
							else if (j[item.key()]["type"] == "color") {
								color_name.push_back(item.key());
								color_list.insert(std::pair<std::string, Array<real>>(item.key(), Array<real>()));
								if (j[item.key()]["color_type"] == "film") {
									color_type.insert(std::pair<std::string, RealColor*>(item.key(), new FilmColor()));
								}
								else {
									color_type.insert(std::pair<std::string, RealColor*>(item.key(), nullptr));
								}
								color_transparent.insert(std::pair<std::string, real>(item.key(), j[item.key()]["transparent"]));
								color_draw.insert(std::pair<std::string, bool>(item.key(), false));
								color_path.insert(std::pair<std::string, std::string>(item.key(), j[item.key()]["name"]));
								std::map<std::string, std::string> color_path;
							}
							else if (j[item.key()]["type"] == "triangle") {
								triangle_name = item.key();
								triangle_path = j[item.key()]["name"];
								have_triangle = true;
							}
						}
					}
					/// Then, set the color
					setRGB(base_color,zq::SequentialDisplayColor[0]);
					for (int i = 0; i < vector_name.size(); i++) {
						std::string name = vector_name[i];
						setRGB(vector_color[name], zq::SequentialDisplayColor[i + 1]);
					}
				}
				void updateData(std::string root_name) {
					 zq::utils::Read_Vector_Array_3D<zq::real, d>(root_name+"/"+base_path, base_point);
					 for (int i = 0; i < vector_name.size(); i++) {
						 std::string name = vector_name[i];
						 zq::utils::Read_Vector_Array_3D<zq::real, d>(root_name + "/" + vector_path[name], vector_list[name]);
					 }
					 for (int i = 0; i < color_name.size(); i++) {
						 std::string name = color_name[i];
						 zq::utils::Read_Array<zq::real>(root_name + "/" + color_path[name], color_list[name]);
					 }
					 if (have_triangle) {
						 zq::utils::Read_Vector_Array_3D<int, d>(root_name + "/" + triangle_path,triangle);
					 }
				}
				void control(char key) {
					if (key == this_key_map["click_this"]) {
						use_this = !use_this;
					}
					if (use_this) {
						if (key == this_key_map["switch_data"]) {
							now_cur = (now_cur + 1) % 4;
						}
						/// now check each attribute
						if (now_cur == 0) {
							if (key == base_key_map["use_base"]) {
								use_base = !use_base;
							}
						}
						else if (now_cur == 1) {
							if (vector_list.size()!=0) {
								if (vector_cur == -1) vector_cur = 0;
								if (key == vector_key_map["move_cur_left"]) {
									vector_cur = (vector_cur - 1 + vector_list.size()) % vector_list.size();
								}
								else if (key == vector_key_map["move_cur_right"]) {
									vector_cur = (vector_cur + 1 + vector_list.size()) % vector_list.size();
								}
								else if (key == vector_key_map["enlarge_ratio"]) {
									std::string name = vector_name[vector_cur];
									vector_ratio[name] *= 1.01;
								}
								else if (key == vector_key_map["shrink_ratio"]) {
									std::string name = vector_name[vector_cur];
									vector_ratio[name] *= 0.99;
								}
								else if (key == vector_key_map["draw_vector"]) {
									std::string name = vector_name[vector_cur];
									vector_draw[name] =!vector_draw[name];
								}
							}
						}
						else if (now_cur == 2) {
							if (color_list.size() != 0) {
								if (color_cur == -1) color_cur = 0;
								if (key == color_key_map["move_cur_left"]) {
									color_cur = (color_cur - 1 + color_list.size()) % color_list.size();
								}
								else if (key == color_key_map["move_cur_right"]) {
									color_cur = (color_cur + 1 + color_list.size()) % color_list.size();
								}
								else if (key == color_key_map["use_this_color"]) {
									for (int i = 0; i < color_name.size(); i++) {
										std::string name = color_name[i];
										color_draw[name] = false;
									}
									std::string name = color_name[color_cur];
									color_draw[name] = true;
								}
							}
						}
						else if (now_cur == 3) {
							if (key == triangle_key_map["use_triangle"]) {
								use_triangle = !use_triangle;
							}
						}
					}
				}
				void outputStatus(std::vector<std::string>& status) {
					status.resize(4);
					if (use_this) {
						status[0] = "Base:";
						if (now_cur == 0) status[0] += "<-";
						if (use_base) status[0] += "(Y)";
						else status[0] += "(N)";
						status[1] = "Vector:";
						if (now_cur == 1) status[1] += "<-";
						for (int i = 0; i < vector_name.size(); i++) {
							std::string name = vector_name[i];
							status[1] += name;
							if (vector_draw[name]) status[1] += "(Y)";
							else status[1] += "(N)";
							if (i == vector_cur) status[1] += "<-";
							status[1] += ",";
						}
						status[2] = "Color:";
						if (now_cur == 2) status[2] += "<-";
						for (int i = 0; i < color_name.size(); i++) {
							std::string name = color_name[i];
							status[2] += name;
							if (color_draw[name]) status[2] += "(Y)";
							else status[2] += "(N)";
							if (i == color_cur) status[2] += "<-";
							status[2] += ",";
						}
						status[3] = "Triangle:";
						if (now_cur == 3) status[3] += "<-";
						if (use_triangle) status[3] += "(Y)";
						else status[3] += "(N)";
					}
					else {
						for (int i = 0; i < 4; i++) {
							status[i] = "";
						}
					}
				
				}
				void draw() {
					/// first, draw base
					RealColor* real_color = nullptr;
					std::string used_color_name;
					float transparent_data;
					for (int i = 0; i < color_name.size(); i++) {
						std::string name = color_name[i];
						if (color_draw[name]) {
							real_color = color_type[name];
							used_color_name = name;
							transparent_data = color_transparent[name];
							break;
						}
					}
					if (use_base) {
						if (real_color != nullptr) {
							for (int i = 0; i < base_point.size(); i++) {
								float color[3];
								real_color->getRGBColor(color_list[used_color_name][i], color);
								glColor3f(color[0], color[1], color[2]);
								zq::opengl::drawPoint(base_point[i]);
							}
						}
						else {
							for (int i = 0; i < base_point.size(); i++) {
								glColor3f(base_color[0], base_color[1], base_color[2]);
								zq::opengl::drawPoint(base_point[i]);
							}
						}
					}

					/// then draw vector
					for (int i = 0; i < vector_name.size(); i++) {
						std::string name = vector_name[i];
						if (vector_draw[name]) {
							for (int j = 0; j < vector_list[name].size(); j++) {
								VecD v = vector_list[name][j];
								VecD b = base_point[j];
								glColor3f(vector_color[name][0], vector_color[name][1], vector_color[name][2]);
								zq::opengl::drawLineSegment<real,real>(b,b+v*vector_ratio[name]);
							}
						}
					}
					/// then draw triangle
					if (use_triangle && real_color!=nullptr && have_triangle) {
						for (int i = 0; i < triangle.size(); i++) {
							if constexpr (d == 3) {
								
								if (transparent_data < 1e-5) {
									glBegin(GL_TRIANGLES);
									int idx1 = triangle[i][0];
									float color[3];
									real_color->getRGBColor(color_list[used_color_name][idx1], color);
									glColor3f(color[0], color[1], color[2]);
									glVertex3f(base_point[idx1].x, base_point[idx1].y, base_point[idx1].z);

									int idx2 = triangle[i][1];
									real_color->getRGBColor(color_list[used_color_name][idx2], color);
									glColor3f(color[0], color[1], color[2]);
									glVertex3f(base_point[idx2].x, base_point[idx2].y, base_point[idx2].z);

									int idx3 = triangle[i][2];
									real_color->getRGBColor(color_list[used_color_name][idx3], color);
									glColor3f(color[0], color[1], color[2]);
									glVertex3f(base_point[idx3].x, base_point[idx3].y, base_point[idx3].z);
									glEnd();
								}
								else {
									glEnable(GL_BLEND);
									glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
									glDepthMask(GL_FALSE);
									glDisable(GL_LIGHTING);

									glBegin(GL_TRIANGLES);
									int idx1 = triangle[i][0];
									float color[3];
									real_color->getRGBColor(color_list[used_color_name][idx1], color);
									glColor4f(color[0], color[1], color[2], transparent_data);
									glVertex3f(base_point[idx1].x, base_point[idx1].y, base_point[idx1].z);

									int idx2 = triangle[i][1];
									real_color->getRGBColor(color_list[used_color_name][idx2], color);
									glColor4f(color[0], color[1], color[2], transparent_data);
									glVertex3f(base_point[idx2].x, base_point[idx2].y, base_point[idx2].z);

									int idx3 = triangle[i][2];
									real_color->getRGBColor(color_list[used_color_name][idx3], color);
									glColor4f(color[0], color[1], color[2], transparent_data);
									glVertex3f(base_point[idx3].x, base_point[idx3].y, base_point[idx3].z);
									glEnd();


									glDisable(GL_BLEND);
									glEnable(GL_LIGHTING);
									glDepthMask(GL_TRUE);
								}

							}
							else if constexpr (d == 2) {
								/// TODO
							}
						}
					}
				}
				void setRGB(float color[3], int rgb) {
					color[2] = (rgb & 0xff)/255.0;
					color[1] = ((rgb>>8) & 0xff) / 255.0;
					color[0] = ((rgb >> 16) & 0xff) / 255.0;
				}
		};
	}
}	


#endif	