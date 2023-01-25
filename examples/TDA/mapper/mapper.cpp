/***********************************************************/
/**	\file
	\brief		Example of Mapper
	\details	Example of Mapper
	\author		Zhiqi Li
	\date		1/23/2022
*/
/***********************************************************/

//	This example shows how to use GLUTWindow and FBO

#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include<zqVisualization/visualization_opengl.h>
#include<zqVisualization/visualization_persistent_homology.h>
#include<zqBasicUtils/utils_timer.h>
#include<zqBasicMath/math_utils.h>
#include<zqBasicMath/math_io.h>
#include<zqBasicMath/math_cluster.h>
#include<zqBasicMath/math_mapper.h>
#include<zqBasicUtils/utils_lookup_table.h>
#include<math_data_path.h>

using namespace std;

//	Two windows and a manager, ID of each window must be unique
zq::opengl::DemoWindowManager		manager;
zq::opengl::GLUTWindow3D<0>			win0(0, 0, 800, 600);  // vis for 3D cluster
zq::opengl::GLUTWindow2D<1>			win1(820, 0, 400, 400); // vis for 2D cluster

// control
bool draw_raw_points=true;
bool draw_filter_split = false;
bool draw_cluster = false;
bool draw_topology = false;


// data
int example = 0;

zq::Array<zq::Vec<zq::real, 2>> data2D; /// 2D hand data
zq::Vec<zq::real, 1> ub2D;
zq::Vec<zq::real, 1> lb2D;
zq::Vec<int, 1> slices2D(20);
zq::real overlap2D = 0.5;
zq::Array<zq::Vec<zq::real, 1>, HOST> interval_l2D;
zq::Array<zq::Vec<zq::real, 1>, HOST> interval_u2D;
zq::Array<zq::Array<int, HOST>> clusters_map2D;
zq::Simplical_Complex<zq::Vec<zq::real, 2>> complex2D;
zq::Array<zq::Array<zq::Vec<zq::real, 2>, HOST>> ori_points_class2D;
zq::Array<zq::Vec<zq::real, 1>, HOST> filter_res2D;

zq::Array<zq::Vec<zq::real, 3>> data3D; /// 3D sphere data
zq::Vec<zq::real, 2> ub3D;
zq::Vec<zq::real, 2> lb3D;
zq::Vec<int, 2> slices3D(20,20);
zq::real overlap3D = 0.5;
zq::Array<zq::Vec<zq::real, 2>, HOST> interval_l3D;
zq::Array<zq::Vec<zq::real, 2>, HOST> interval_u3D;
zq::Array<zq::Array<int, HOST>> clusters_map3D;
zq::Simplical_Complex<zq::Vec<zq::real, 3>> complex3D;
zq::Array<zq::Array<zq::Vec<zq::real, 3>, HOST>> ori_points_class3D;
zq::Array<zq::Vec<zq::real, 2>, HOST> filter_res3D;

zq::Array<int, HOST> clusters;
//	other variables
float				fps;
float* tex_ptr;

//	over-ridden functions
void calculateFps() {
	static zq::utils::FPSCalculator fps_calc;
	fps = fps_calc.GetFPS();
}

void win0_showFps() {
	glColor3f(1, 0, 1);
	zq::opengl::printInfo(0, 0, "fps: %f", fps);
	zq::opengl::printInfo(0, 20, "draw_raw_points(q):%s", draw_raw_points ? "True" : "False");
	zq::opengl::printInfo(0, 40, "draw_filter_split(w):%s", draw_filter_split ? "True" : "False");
	zq::opengl::printInfo(0, 60, "draw_cluster(e):%s", draw_cluster ? "True" : "False");
	zq::opengl::printInfo(0, 80, "draw_topology(r):%s", draw_topology ? "True" : "False");
}

void win0_draw() {
	if (true || example == 1) {
		if (draw_raw_points) {
			float color[3];
			zq::opengl::getSequentialDisplayColor(color, 0);
			glColor3f(color[0], color[1], color[2]);
			for (int i = 0; i < data3D.size(); i++) {
				zq::opengl::drawPointAsSphere(data3D[i], 0.02);
			}
		}
		if (draw_topology) {
			zq::homology::DrawSimplicalComplex<zq::real, 3>(
				complex3D,
				std::vector<std::vector<float>>{
					{1, 0, 0, 1},
					{ 0,1.0,0,1 },
					{ 0,0,1,1 },
					{ 1,0,1,0.5 },
				},
				0.03f
				);
		}
		if (draw_filter_split) {
			for (int j = 0; j < ori_points_class3D.size(); j++) {
				float color[3];
				zq::opengl::getSequentialDisplayColor(color, j + complex3D.points.size() + 2);
				glColor3f(color[0], color[1], color[2]);
				for (int i = 0; i < ori_points_class3D[j].size(); i++) {
					zq::opengl::drawPointAsSphere(ori_points_class3D[j][i], 0.03);
				}

			}
		}
		if (draw_cluster) {
			for (int j = 0; j < complex3D.points.size(); j++) {
				float color[3];
				zq::opengl::getSequentialDisplayColor(color, j + complex3D.points.size() + 2 + ori_points_class3D.size());
				glColor3f(color[0], color[1], color[2]);
				for (int i = 0; i < clusters_map3D.size(); i++) {
					for (int k = 0; k < clusters_map3D[i].size(); k++) {
						if (clusters_map3D[i][k] == j) {
							zq::opengl::drawPointAsSphere(data3D[k], 0.03);
						}
					}
				}
			}
		}
	}
}




void win3d_keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
		exit(0);
		break;
	case 'q':
		draw_raw_points = !draw_raw_points;
		break;
	case 'w':
		draw_filter_split = !draw_filter_split;
		break;
	case 'e':
		draw_cluster = !draw_cluster;
		break;
	case 'r':
		draw_topology = !draw_topology;
		break;
	}
}

void win1_showInfo() {
	glColor3f(0, 0, 1);
	zq::opengl::printInfo(0, 0, "2D cluster draw");
}
void win1_draw() {
	//glEnable(GL_TEXTURE_2D);
	if (true || example == 0) {
		zq::real win_x = 2;
		zq::real win_y = 2;
		zq::real win_ox = -1;
		zq::real win_oy = -1;
		// raw: color 0, connection 1
		// 2,..., center
		// 2+center,...,  filter_split
		// 2+center+filter_split,... cluster
		if (draw_raw_points) {
			float color[3];
			zq::opengl::getSequentialDisplayColor(color, 0);
			glColor3f(color[0], color[1], color[2]);
			for (int i = 0; i < data2D.size(); i++) {
				zq::opengl::drawCircleFilled(zq::opengl::Window2DCoordinate<zq::real>(data2D[i], win_x, win_y, win_ox, win_oy), 0.02);
			}
		}
		if (draw_topology) {
			std::function<zq::Vec<zq::real, 2>(const zq::Vec<zq::real, 2>&)> convert2D = [&](const zq::Vec<zq::real, 2>& p)->zq::Vec<zq::real, 2> {
				return zq::opengl::Window2DCoordinate<zq::real>(p, win_x, win_y, win_ox, win_oy);
			};
			zq::homology::DrawSimplicalComplex<zq::real,2>(
				complex2D,
				std::vector<std::vector<float>>{
					{1, 0, 0, 1},
					{ 0,1.0,0,1 },
					{ 0,0,1,1 },
					{ 1,0,1,0.5 },
				},
				0.02,
				&convert2D
			);
		}
		if (draw_filter_split) {
			for (int j = 0; j < ori_points_class2D.size(); j++) {
				float color[3];
				zq::opengl::getSequentialDisplayColor(color, j + complex2D.points.size()+2);
				glColor3f(color[0], color[1], color[2]);
				for (int i = 0; i < ori_points_class2D[j].size(); i++) {
					zq::opengl::drawCircleFilled(zq::opengl::Window2DCoordinate<zq::real>(ori_points_class2D[j][i], win_x, win_y, win_ox, win_oy), 0.05-j*0.05/ ori_points_class2D.size());
				}

			}
		}
		if (draw_cluster) {
			for (int j = 0; j < complex2D.points.size(); j++) {
				float color[3];
				zq::opengl::getSequentialDisplayColor(color, j + complex2D.points.size() + 2+ ori_points_class2D.size());
				glColor3f(color[0], color[1], color[2]);
				for (int i = 0; i < clusters_map2D.size(); i++) {
					for (int k = 0; k < clusters_map2D[i].size(); k++) {
						if (clusters_map2D[i][k] == j) {
							zq::opengl::drawCircleFilled(zq::opengl::Window2DCoordinate<zq::real>(data2D[k], win_x, win_y, win_ox, win_oy), 0.05 - j * 0.05 / complex2D.points.size());
						}
					}
				}
			}
		}

	}
	
}

int main() {

	// assemble data
	float reso = 1000, r = 0.8, ran = 0;
	for (int i = 0; i < reso; i++) {
		data2D.push_back(
			zq::Vec<zq::real,2>(
				(r + zq::randFloatingPointNumber(-ran / 2, ran / 2)) * cos(2 * ZQ_PI * i / reso),
				(r + zq::randFloatingPointNumber(-ran / 2, ran / 2)) * sin(2 * ZQ_PI * i / reso)
				));
	}
	ub2D= zq::Vec<zq::real, 1>(0.9);
	lb2D= zq::Vec<zq::real, 1>(-0.9);
	
	float reso3D = 10000, r3D = 0.6, ran3D = 0;
	float sqrt_reso3D = sqrt(reso3D);
	for (int j = 0; j < sqrt_reso3D; j++) {
		for (int i = 0; i < sqrt_reso3D * sin(ZQ_PI * j / sqrt_reso3D); i++) {
			data3D.push_back(
				zq::Vec<zq::real,3>(
					(r3D + zq::randFloatingPointNumber(0.0f, ran3D)) * cos(2 * ZQ_PI * i / sqrt_reso3D / sin(ZQ_PI * j / sqrt_reso3D)) * sin(ZQ_PI * j / sqrt_reso3D),
					(r3D + zq::randFloatingPointNumber(0.0f, ran3D)) * sin(2 * ZQ_PI * i / sqrt_reso3D / sin(ZQ_PI * j / sqrt_reso3D)) * sin(ZQ_PI * j / sqrt_reso3D),
					(r3D + zq::randFloatingPointNumber(0.0f, ran3D)) * cos(ZQ_PI * j / sqrt_reso3D)
				));
		}
	}
	lb3D = zq::Vec<zq::real, 2>(-r3D-ran3D, -r3D - ran3D);
	ub3D = zq::Vec<zq::real, 2>(r3D + ran3D, r3D + ran3D);
	//scanf("%d", &example);
	example = 1;
	if (true || example == 0) {
		zq::math::ClusterPara para;
		para.cluster_num = 2;
		zq::math::Mapper<zq::math::XCoordFilter<zq::real, 2>, zq::math::ClusterType::KMEANS> mapper;

		mapper.Interval<zq::real, 1, HOST>(
			ub2D,
			lb2D,
			slices2D,
			overlap2D,
			interval_u2D,
			interval_l2D
		);
		mapper.DoMapper<zq::real, 2, HOST>(
			data2D,
			interval_l2D,
			interval_u2D,
			para,
			clusters_map2D,
			filter_res2D,
			complex2D,
			ori_points_class2D
			);
	}
   if (true || example == 1) {
		zq::math::ClusterPara para;
		para.cluster_num = 2;
		zq::math::Mapper<zq::math::XYCoordFilter<zq::real, 3>, zq::math::ClusterType::KMEANS> mapper;
		
		mapper.Interval<zq::real, 2, HOST>(
			ub3D,
			lb3D,
			slices3D,
			overlap3D,
			interval_u3D,
			interval_l3D
			);
		mapper.DoMapper<zq::real, 3, HOST>(
			data3D,
			interval_l3D,
			interval_u3D,
			para,
			clusters_map3D,
			filter_res3D,
			complex3D,
			ori_points_class3D
			);
	}
	//	create window 0
	complex3D;
	win0.SetDrawAppend(win0_showFps);				//	set draw append callback
	win0.SetDraw(win0_draw);						//	set draw callback
	win0.SetBackgroundColor(0.5, 0.5, 0.5, 1);		//	set background color
	win0.keyboardFunc = win3d_keyboard;
	win0.CreateGLUTWindow();						//	create window 0

	//	create window 1, then setup barcode
	win1.SetDraw(win1_draw);							//	set draw callback
	win1.SetDrawAppend(win1_showInfo);					//	set draw append callback
	win1.SetBackgroundColor(0.5, 0.5, 0.5, 1);				//	set background color
	win1.CreateGLUTWindow();							//	create window 1

	//!	[Window Manager Setup]
	manager.AddIdleFunc(calculateFps);	//	add each idle function in sequence
	manager.AddIdleFunc(win0.idleFunc);
	manager.AddIdleFunc(win1.idleFunc);

	manager.EnterMainLoop();			//	then enter main loop
	//!	[Window Manager Setup]
}