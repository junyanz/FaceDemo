#ifndef WARP_UTILS_H_
#define WARP_UTILS_H_
#include "common.h"

class CWarpUtils {

public:
	static PointSetd Mat2Pnts(Mat _matPnts);
	static PointSetd MedianPnts(const vector<PointSetd>& _allPnts );
	static Mat LoadMeanPnts3D(string _path);
	static PointSetd ProjPnts(const PointSetd _p, const Mat& _H);
	static void EstimateHWeighted(const PointSetd a, const PointSetd b, const vectord _w, CvMat* M, int full_affine);
	static Mat EstimateHWeighted( PointSetd _pnt1, PointSetd _pnt2, bool _flag);
};
#endif //WARP_UTILS_H_