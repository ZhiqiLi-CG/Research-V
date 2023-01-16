#ifndef __VISUALIZATION_POINT_COLOR_H__
#define __VISUALIZATION_POINT_COLOR_H__
#include<zqBasicMath/math_vector.h>
#include<zqVisualization/visualization_color_data.h>
namespace zq {
	namespace vis {
		class RealColor {
		public:
			RealColor() {}
			virtual void getRGBColor(real data,float color[3]) {
				Assert(false, "getRGBColor of RealColor have not bee implemented");
			}
			virtual void getRGBAColor(real data, float  color[4]) {
				Assert(false, "getRGBAColor of RealColor have not bee implemented");
			}
		};
		class FilmColor :public RealColor{
		public:
			real gamma;
			FilmColor(real gamma = 1):gamma(gamma) {}
			virtual void getRGBColor(real data, float color[3]) {
				real thick = data * 1e9;
				if (thick > film_color_data::max_thick) thick = film_color_data::max_thick;
				int d1 = thick / film_color_data::color_dx;
				real lb = d1 * film_color_data::color_dx;
				real ratio = (thick - lb) / film_color_data::color_dx;
				for (int i = 0; i < 3; i++) {
					color[i] = (ratio * ((real)film_color_data::color_film[d1][i]) + (1 - ratio) * ((real)film_color_data::color_film[d1 + 1][i])) * 1.0 / 255;
				}
			}
		};
	}
}
#endif
