/***********************************************************/
/**	\file
	\brief		Example of persistent homology
	\details	Example of persistent homology
	\author		Zhiqi Li
	\date		12/24/2022
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
#include<zqBasicUtils/utils_lookup_table.h>
#include<math_data_path.h>

using namespace std;

//	Two windows and a manager, ID of each window must be unique
zq::opengl::DemoWindowManager		manager;
zq::opengl::GLUTWindow3D<0>			win0(0, 0, 800, 600);  // vis for 3D cluster
zq::opengl::GLUTWindow2D<1>			win1(820, 0, 400, 400); // vis for 2D cluster

// homology
int example = 0;
//https://blog.csdn.net/playgoon2/article/details/77579757
zq::real data2D_raw[] = { -1.26, 0.46, -1.15, 0.49, -1.19, 0.36, -1.33, 0.28, -1.06, 0.22, -1.27, 0.03, -1.28, 0.15, -1.06, 0.08, -1.00, 0.38, -0.44, 0.29, -0.37, 0.45, -0.22, 0.36, -0.34, 0.18, -0.42, 0.06, -0.11, 0.12, -0.17, 0.32,
 -0.27, 0.08, -0.49, -0.34, -0.39, -0.28, -0.40, -0.45, -0.15, -0.33, -0.15, -0.21, -0.33, -0.30, -0.23, -0.45, -0.27, -0.59, -0.61, -0.65, -0.61, -0.53, -0.52, -0.53, -0.42, -0.56, -1.39, -0.26 };
zq::Array<zq::Vec<zq::real, 2>> data2D;
zq::Array<zq::Vec<zq::real, 2>, HOST> center2D;

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
}

void win0_draw() {
	if (example == 0 || example == 1) {
		zq::real win_x = 2;
		zq::real win_y = 2;
		zq::real win_ox = -1;
		zq::real win_oy = -1;
		/// draw example of 2D
		glColor3f(1, 0, 0);
		float color[3];
		zq::opengl::getSequentialDisplayColor(color, 0);
		glColor3f(color[0], color[1], color[2]);
		for (int i = 0; i < center2D.size(); i++) {
			zq::opengl::drawCircleFilled(zq::opengl::Window2DCoordinate<zq::real>(center2D[i], win_x, win_y, win_ox, win_oy), 0.05);
		}
		for (int i = 0; i < data2D.size(); i++) {
			float color[3];
			zq::opengl::getSequentialDisplayColor(color, clusters[i] + 1);
			glColor3f(color[0], color[1], color[2]);
			zq::opengl::drawCircleFilled(zq::opengl::Window2DCoordinate<zq::real>(data2D[i], win_x, win_y, win_ox, win_oy), 0.02);
			//printf("%f %f\n", zq::opengl::Window2DCoordinate<zq::real>(data2D[i], win_x, win_y, win_ox, win_oy)[0], zq::opengl::Window2DCoordinate<zq::real>(data2D[i], win_x, win_y, win_ox, win_oy)[1]);
		}
	}
}




void win3d_keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
		exit(0);
	}
}

void win1_showInfo() {
	glColor3f(0, 0, 1);
	zq::opengl::printInfo(0, 30, "2D cluster draw");
}
void win1_draw() {
	//glEnable(GL_TEXTURE_2D);
	if (example == 0 || example == 1) {
		zq::real win_x = 2;
		zq::real win_y = 2;
		zq::real win_ox = -1;
		zq::real win_oy = -1;
		/// draw example of 2D
		glColor3f(1, 0, 0);
		float color[3];
		zq::opengl::getSequentialDisplayColor(color, 0);
		glColor3f(color[0], color[1], color[2]);
		for (int i = 0; i < center2D.size(); i++) {
			zq::opengl::drawCircleFilled(zq::opengl::Window2DCoordinate<zq::real>(center2D[i], win_x, win_y, win_ox, win_oy),0.05);
		}
		for (int i = 0; i < data2D.size(); i++) {
			float color[3];
			zq::opengl::getSequentialDisplayColor(color, clusters[i]+1);
			glColor3f(color[0], color[1], color[2]);
			zq::opengl::drawCircleFilled(zq::opengl::Window2DCoordinate<zq::real>(data2D[i], win_x, win_y, win_ox, win_oy),0.02);
			//printf("%f %f\n", zq::opengl::Window2DCoordinate<zq::real>(data2D[i], win_x, win_y, win_ox, win_oy)[0], zq::opengl::Window2DCoordinate<zq::real>(data2D[i], win_x, win_y, win_ox, win_oy)[1]);
		}
	}
}

int main() {

	// assemble data
	for (int i = 0; i < sizeof(data2D_raw) / sizeof(zq::real); i += 2) {
		data2D.push_back(zq::Vec<zq::real, 2>(data2D_raw[i]+1.5, data2D_raw[i + 1] + 1.5)/2);
	}
	//scanf("%d", &example);
	example = 0;
	if (example == 0) {
		zq::math::ClusterPara para;
		para.cluster_num = 4;
		zq::math::kMeansCluster<zq::real, 2, HOST>(
			data2D,
			para,
			clusters,
			center2D
		);
	}
	if(example==1){
		zq::math::ClusterPara para;
		para.cluster_num = 8;
		zq::math::kMeansClusterGap<zq::real, 2, HOST>(
			data2D,
			para,
			clusters,
			center2D
		);
	}
	//	create window 0
	win0.SetDrawAppend(win0_showFps);				//	set draw append callback
	win0.SetDraw(win0_draw);						//	set draw callback
	win0.SetBackgroundColor(1,1,1,1);		//	set background color
	win0.keyboardFunc = win3d_keyboard;
	win0.CreateGLUTWindow();						//	create window 0

	//	create window 1, then setup barcode
	win1.SetDraw(win1_draw);							//	set draw callback
	win1.SetDrawAppend(win1_showInfo);					//	set draw append callback
	win1.SetBackgroundColor(1, 1, 1, 1);				//	set background color
	win1.CreateGLUTWindow();							//	create window 1

	//!	[Window Manager Setup]
	manager.AddIdleFunc(calculateFps);	//	add each idle function in sequence
	manager.AddIdleFunc(win0.idleFunc);
	manager.AddIdleFunc(win1.idleFunc);

	manager.EnterMainLoop();			//	then enter main loop
	//!	[Window Manager Setup]
}