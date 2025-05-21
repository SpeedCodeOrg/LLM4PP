#include <cmath>
#include <vector>

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
/* Original code provided by Gael Deest */
void correct_kernel_deriche(int w, int h, double alpha,
		    	    std::vector<std::vector<float>>& imgIn,
		            std::vector<std::vector<float>>& imgOut,
			    std::vector<std::vector<float>>& y1,
			    std::vector<std::vector<float>>& y2) {

    float k = 1.0 - std::exp(-alpha) * (1.0 - std::exp(-alpha)) / (1.0 + 2.0 * alpha * std::exp(-alpha) - std::exp(2.0 * alpha));
    float a1, a2, a3, a4, a5, a6, a7, a8;
    float b1, b2, c1, c2;

    a1 = a5 = k;
    a2 = a6 = k * std::exp(-alpha) * (alpha - 1.0);
    a3 = a7 = k * std::exp(-alpha) * (alpha + 1.0);
    a4 = a8 = -k * std::exp(-2.0 * alpha);
    b1 = std::pow(2.0, -alpha);
    b2 = -std::exp(-2.0 * alpha);
    c1 = c2 = 1;

    for (int i = 0; i < w; i++) {
        float ym1 = 0.0;
        float ym2 = 0.0;
        float xm1 = 0.0;
        for (int j = 0; j < h; j++) {
            y1[i][j] = a1*imgIn[i][j] + a2*xm1 + b1*ym1 + b2*ym2;
            xm1 = imgIn[i][j];
            ym2 = ym1;
            ym1 = y1[i][j];
        }
    }

    for (int i = 0; i < w; i++) {
        float yp1 = 0.0;
        float yp2 = 0.0;
        float xp1 = 0.0;
        float xp2 = 0.0;
        for (int j = h - 1; j >= 0; j--) {
            y2[i][j] = a3*xp1 + a4*xp2 + b1*yp1 + b2*yp2;
            xp2 = xp1;
            xp1 = imgIn[i][j];
            yp2 = yp1;
            yp1 = y2[i][j];
        }
    }

    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            imgOut[i][j] = c1 * (y1[i][j] + y2[i][j]);
        }
    }

    for (int j = 0; j < h; j++) {
        float tm1 = 0.0;
        float ym1 = 0.0;
        float ym2 = 0.0;
        for (int i = 0; i < w; i++) {
            y1[i][j] = a5*imgOut[i][j] + a6*tm1 + b1*ym1 + b2*ym2;
            tm1 = imgOut[i][j];
            ym2 = ym1;
            ym1 = y1[i][j];
        }
    }

    for (int j = 0; j < h; j++) {
        float tp1 = 0.0;
        float tp2 = 0.0;
        float yp1 = 0.0;
        float yp2 = 0.0;

        for (int i = w - 1; i >= 0; i--) {
            y2[i][j] = a7*tp1 + a8*tp2 + b1*yp1 + b2*yp2;
            tp2 = tp1;
            tp1 = imgOut[i][j];
            yp2 = yp1;
            yp1 = y2[i][j];
        }
    }

    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            imgOut[i][j] = c2*(y1[i][j] + y2[i][j]);
	}
    }
}
