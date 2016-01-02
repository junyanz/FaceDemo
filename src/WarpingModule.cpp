#include "WarpingModule.h"
#include "WarpUtils.h"

CWarpingModule::CWarpingModule( ) {
    m_pnts3d = CWarpUtils::LoadMeanPnts3D("./model/3d.txt"); // load 3D points
	m_size = Size(174, 224);  // load depth information of 3D face
	m_Z = Mat(m_size, CV_64FC1);

    ifstream fin("./model/Z.img");
	FOR (h, m_size.height) {
		FOR (w, m_size.width) {
			double z; 
			fin >> z; 
			m_Z.at<double>(h,w) = z; 
		}
	}
	fin.close();

}

CWarpingModule::~CWarpingModule() {}

void CWarpingModule::Clear() {}


cv::Mat CWarpingModule::FrontalFace() {
	return m_frontalFace; 
}

inline double bilinear_interp(double x, double y,
	double v11, double v12, double v21, double v22){
	return (v11*(1-y) + v12*y) * (1-x) + (v21*(1-y) + v22*y) * x;
}

void CWarpingModule::WarpToFrontalFace3D( const Mat& _img, const PointSetd& _pnts, bool _isWarp ) {
	//int idx[NUM_POINTS] = {20, 23, 26, 29, 15, 17, 19, 32, 38}; // Matlab idx
	//PointSetd pntsall = CWarpUtils::Mat2Pnts(_pnts);
	PointSetd srcPnts = _pnts;
	PointSetd tgtPnts(9); 
	vector<Point3d> tgtPnts3d(9);
	FOR (i, NUM_POINTS) {
		//srcPnts[i] = pntsall[idx[i]-1];
		tgtPnts[i] = Point2d(m_pnts3d.at<double>(i,0), m_pnts3d.at<double>(i,1));
		tgtPnts3d[i] = Point3d(tgtPnts[i].x, tgtPnts[i].y, m_pnts3d.at<double>(i, 2));
	}

	// 2D alignment 
	double m[6]={0};
	CvMat M = cvMat( 2, 3, CV_64F, m);
	vectord weights(9, 1);
	CWarpUtils::EstimateHWeighted(srcPnts, tgtPnts, weights, &M, true);
	m_H = cvarrToMat(&M).clone();
	
	Mat warp2d = Mat::zeros(m_size, _img.type()); 
	warpAffine(_img, warp2d, m_H, m_size); 

	PointSetd pnts_proj = CWarpUtils::ProjPnts(srcPnts, m_H);
	// 3D alignment
	// remove translation 
	Point2d mean2d = Point2d(0, 0);
	FOR_u (i, srcPnts.size())
		mean2d = mean2d + pnts_proj[i];
	mean2d = Point2d(mean2d.x/srcPnts.size(), mean2d.y/srcPnts.size());
	FOR_u (i, srcPnts.size())
		pnts_proj[i] = pnts_proj[i] - mean2d;
	Point3d mean3d = Point3d(0, 0, 0); //mean(m_pnts3d);
	FOR_u (i, tgtPnts3d.size())
		mean3d = mean3d + tgtPnts3d[i];
	mean3d = Point3d(mean3d.x/tgtPnts3d.size(), mean3d.y/tgtPnts3d.size(), mean3d.z/tgtPnts3d.size());
	FOR_u (i, tgtPnts3d.size())
		tgtPnts3d[i] = tgtPnts3d[i] - mean3d;
	// find 2d to 3d affine
	Mat mat3d = Mat(Size(tgtPnts3d.size(), 3), CV_64FC1);
	Mat mat2d = Mat(Size(pnts_proj.size(), 2), CV_64FC1);
	FOR_u (i, tgtPnts3d.size()) {
		mat3d.at<double>(0, i) = tgtPnts3d[i].x; 
		mat3d.at<double>(1, i) = tgtPnts3d[i].y; 
		mat3d.at<double>(2, i) = tgtPnts3d[i].z; 
	}

	FOR_u (i, srcPnts.size()) {
		mat2d.at<double>(0, i) = pnts_proj[i].x; 
		mat2d.at<double>(1, i) = pnts_proj[i].y; 
	}

	Mat mat33 = mat3d * mat3d.t();
	Mat mat22 = mat2d * mat3d.t();
	Mat ar12 = mat22 * mat33.inv();
	Mat ar3 = ar12.row(0).cross(ar12.row(1));
	vconcat(ar12, ar3, m_ar);
	Mat mean2dmat = Mat(Size(1, 2), CV_64FC1);
	mean2dmat.at<double>(0, 0) = mean2d.x; 
	mean2dmat.at<double>(1, 0) = mean2d.y; 
	Mat mean3dmat = Mat(Size(1, 3), CV_64FC1);
	mean3dmat.at<double>(0, 0) = mean3d.x; 
	mean3dmat.at<double>(1, 0) = mean3d.y; 
	mean3dmat.at<double>(2, 0) = mean3d.z; 
	Mat t2 = mean2dmat - ar12 * mean3dmat;
	m_t = Point2d(t2.at<double>(0, 0), t2.at<double>(1, 0));
	m_warp2d = warp2d;
	if (_isWarp)
		m_frontalFace = WarpFace(warp2d);
	else
		m_frontalFace = Mat();
}

Mat CWarpingModule::WarpFace( Mat warp2d ) {
	//CTimer timer("warp face");
	int width = m_Z.cols; 
	int height = m_Z.rows; 
	Mat warp3d = Mat::zeros(Size(width, height), CV_8UC3);
	int warpWidth = warp2d.cols; 
	int warpHeight = warp2d.rows;
	//#pragma omp parallel for 
	FOR (w, width) {
		FOR (h, height) {
			if (m_Z.at<double>(Point(w, h)) > 0) {
				Mat pnt3d =  Mat(Size(1, 3), CV_64FC1);
				pnt3d.at<double>(0, 0) = w; 
				pnt3d.at<double>(1, 0) = h; 
				pnt3d.at<double>(2, 0) = m_Z.at<double>(Point(w, h));
				Mat pnt2d = m_ar * pnt3d; 
				double nx = pnt2d.at<double>(0, 0) + m_t.x;
				double ny = pnt2d.at<double>(1, 0) + m_t.y; 

				if (nx > 0 && ny > 0 && nx < warpWidth-1 && ny < warpHeight-1) {
					int nxi = int(floor(nx));
					int nyi = int(floor(ny));

					int nxi1 = nxi+1;; 
					int nyi1 = nyi+1; 

					for (int ll=0; ll<3; ll++)
						warp3d.at<Vec3b>(Point(w,h))[ll] =
						(uchar)bilinear_interp(ny-nyi, nx-nxi,
						warp2d.at<Vec3b>(nyi, nxi)[ll], warp2d.at<Vec3b>(nyi, nxi1)[ll],
						warp2d.at<Vec3b>(nyi1, nxi)[ll], warp2d.at<Vec3b>(nyi1, nxi1)[ll]);
				}
			}
		}
	}

	m_frontalFace = warp3d; //clone();
	return warp3d; 
}
