#ifndef COMMON_H_
#define COMMON_H_

//include std
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <iomanip>
#include <time.h>
#include <cstdlib>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/glu.h>

//logging
#include "log.hpp"
//include OpenCV 
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv/cxcore.h"

using namespace std;
using namespace cv;

#define DELETE_OBJECT(x)	if ((x)) { delete (x); (x) = NULL; }
#define FOR(i,n) for( int i=0; i<n; i++ )
#define FOR_u(i, n) for (unsigned i = 0; i < n; i++)
#define PI 3.14159265358979323846 
#define NUM_POINTS 9
// typedefs
typedef vector<int> vectori;
typedef vector<float> vectorf;
typedef vector<double> vectord;
typedef vector<Point2d> PointSetd;

#endif //COMMON_H_
