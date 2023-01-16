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
#include<zqVisualization/visualization_point_cloud.h>
#include<zqBasicUtils/utils_timer.h>
#include<zqBasicMath/math_utils.h>
#include<zqBasicMath/math_io.h>
#include<zqBasicUtils/utils_aux_func.h>
#include<visualization_data_path.h>

using namespace std;

//	Two windows and a manager, ID of each window must be unique
zq::opengl::DemoWindowManager		manager;
zq::opengl::GLUTWindow3D<0>			win0(0, 0, 800, 600);

std::vector<std::string> output_root_dir{
	"simple_bubble_capillary",
	"simple_bubble_marangoni",
	"simple_bubble_vorticity",
	"simple_bubble_pressure"
};
std::string name;
int example=0;
std::string output_format;
zq::json j;
zq::vis::PointCloudDraw<3> pcd;
std::vector<std::string> status_info;
//	other variables
float				fps;
float* tex_ptr;
int frame_now=0;
int step = 1;
//	over-ridden functions
void calculateFps() {
	static zq::utils::FPSCalculator fps_calc;
	fps = fps_calc.GetFPS();
}

void win0_showFps() {
	glColor3f(1, 0, 1);
	zq::opengl::printInfo(0, 0, "name:%s, frame: %d", name.c_str(),frame_now);
	for (int i = 0; i < status_info.size(); i++) {
		zq::opengl::printInfo(0, (i+1) * 20, "%s", status_info[i].c_str());
	}
}

void win0_draw() {
	pcd.draw();
}

void read_data() {
	char frame_str[10] = { 0 };
	itoa(frame_now, frame_str, 10);
	pcd.updateData(output_root_dir[example] + std::string("/") + std::string(frame_str));
}

void win3d_keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 27:
			exit(0);
			break;
		case 'i':
			frame_now = (frame_now - step >= 0) ? frame_now - step :frame_now;
			read_data();
			break;
		case 'o':
			frame_now+= step;
			read_data();
			break;
	}
	pcd.control(key);
	pcd.outputStatus(status_info);
	std::cout << pcd.use_this << std::endl;
}

int main() {
	std::string vis_config = std::string(visualization_data_path) + "/bubble_draw.json";
	std::ifstream conf_file(vis_config.c_str());
	zq::json conf_j;
	conf_file >> conf_j;
	step = conf_j["step"];
	example = conf_j["example"];
	name = output_root_dir[example];
	for (int i = 0; i < output_root_dir.size(); i++) {
		output_root_dir[i] = std::string(conf_j["data"]) + std::string("/") + output_root_dir[i];
	}
	output_format = output_root_dir[example] + std::string("/0/format.json");
	std::ifstream jfile(output_format.c_str());
	jfile >> j;
	pcd.readJson(j);
	read_data();
	
	//	create window 0
	win0.SetDrawAppend(win0_showFps);				//	set draw append callback
	win0.SetDraw(win0_draw);						//	set draw callback
	win0.SetBackgroundColor(0, 0.4, 0.5, 0);		//	set background color
	win0.keyboardFunc = win3d_keyboard;
	win0.CreateGLUTWindow();						//	create window 0
	//!	[Window Manager Setup]
	manager.AddIdleFunc(calculateFps);	//	add each idle function in sequence
	manager.AddIdleFunc(win0.idleFunc);

	manager.EnterMainLoop();			//	then enter main loop
	//!	[Window Manager Setup]
}