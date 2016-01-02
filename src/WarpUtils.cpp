#include "WarpUtils.h"

PointSetd CWarpUtils::Mat2Pnts( Mat _matPnts ) {
	int nLandmarks = _matPnts.rows * _matPnts.cols/2; 
	PointSetd pnts(nLandmarks);
	FOR (i, nLandmarks) 
		pnts[i] = Point2d(_matPnts.at<float>(0,i), _matPnts.at<float>(1,i));//_matPnts.at<double>(i+nLandmarks));
	return pnts; 
}


PointSetd CWarpUtils::MedianPnts(const vector<PointSetd>& _allPnts ) {
	if (_allPnts.empty())
		return PointSetd();
	int nPnts = _allPnts[0].size();
	int nData = (int)_allPnts.size();
	PointSetd medianPnts(nPnts); 
	
	FOR (i, nPnts) {
		vectord xs(nData, 0.0);
		vectord ys(nData, 0.0);
		FOR (j, nData) {
			xs[j] = _allPnts[j][i].x;
			ys[j] = _allPnts[j][i].y; 
		}

		sort(xs.begin(), xs.end());
		sort(ys.begin(), ys.end());

		if (nData % 2 == 0) {
			medianPnts[i].x = (xs[nData/2-1]+xs[nData/2])/2.0; 
			medianPnts[i].y = (ys[nData/2-1]+ys[nData/2])/2.0; 
		} else {
			medianPnts[i].x = xs[nData/2];
			medianPnts[i].y = ys[nData/2];
		}
	}

	return medianPnts;
}


PointSetd CWarpUtils::ProjPnts( const PointSetd _p, const Mat& _H ) {
	int nLandmarks = (int)_p.size(); 
	Mat pnts_3d = Mat::zeros(Size(nLandmarks, 3), CV_64FC1); 

	FOR (i, nLandmarks) {
		pnts_3d.at<double>(0, i) = _p[i].x; //.at<double>(i); 
		pnts_3d.at<double>(1, i) = _p[i].y; //_srcPnts.at<double>(i+nLandmarks); 
		pnts_3d.at<double>(2, i) = 1.0; 
	}

	Mat pnts_proj = _H * pnts_3d; 
	PointSetd projPnts(nLandmarks);

	FOR (i, nLandmarks) {
		double x = pnts_proj.at<double>(0, i); 
		double y = pnts_proj.at<double>(1, i); 
		projPnts[i] = Point2d(x, y);
	}
	return projPnts; 
}

void CWarpUtils::EstimateHWeighted(const PointSetd a, const PointSetd b, const vectord _w, CvMat* M, int full_affine ) {
	int count = (int)a.size();
	if (full_affine ) {
		double sa[36], sb[6];
		CvMat A = cvMat( 6, 6, CV_64F, sa ), B = cvMat( 6, 1, CV_64F, sb );
		CvMat MM = cvMat( 6, 1, CV_64F, M->data.db );

		int i;

		memset( sa, 0, sizeof(sa) );
		memset( sb, 0, sizeof(sb) );

		for( i = 0; i < count; i++ )
		{
			sa[0] += a[i].x*a[i].x*_w[i];
			sa[1] += a[i].y*a[i].x*_w[i];
			sa[2] += a[i].x*_w[i];

			sa[6] += a[i].x*a[i].y*_w[i];
			sa[7] += a[i].y*a[i].y*_w[i];
			sa[8] += a[i].y*_w[i];

			sa[12] += a[i].x*_w[i];
			sa[13] += a[i].y*_w[i];
			sa[14] += 1*_w[i];

			sb[0] += a[i].x*b[i].x*_w[i];
			sb[1] += a[i].y*b[i].x*_w[i];
			sb[2] += b[i].x*_w[i];
			sb[3] += a[i].x*b[i].y*_w[i];
			sb[4] += a[i].y*b[i].y*_w[i];
			sb[5] += b[i].y*_w[i];
		}

		sa[21] = sa[0];
		sa[22] = sa[1];
		sa[23] = sa[2];
		sa[27] = sa[6];
		sa[28] = sa[7];
		sa[29] = sa[8];
		sa[33] = sa[12];
		sa[34] = sa[13];
		sa[35] = sa[14];

		cvSolve( &A, &B, &MM, CV_SVD );
	}
	else
	{
		double sa[16], sb[4], m[4], *om = M->data.db;
		CvMat A = cvMat( 4, 4, CV_64F, sa ), B = cvMat( 4, 1, CV_64F, sb );
		CvMat MM = cvMat( 4, 1, CV_64F, m );

		int i;

		memset( sa, 0, sizeof(sa) );
		memset( sb, 0, sizeof(sb) );

		for( i = 0; i < count; i++ )
		{
			sa[0] += a[i].x*a[i].x*_w[i] + a[i].y*a[i].y*_w[i];
			sa[1] += 0;
			sa[2] += a[i].x*_w[i];
			sa[3] += a[i].y*_w[i];

			sa[4] += 0;
			sa[5] += a[i].x*a[i].x*_w[i] + a[i].y*a[i].y*_w[i];
			sa[6] += -a[i].y*_w[i];
			sa[7] += a[i].x*_w[i];

			sa[8] += a[i].x*_w[i];
			sa[9] += -a[i].y*_w[i];
			sa[10] += 1*_w[i];
			sa[11] += 0;

			sa[12] += a[i].y*_w[i];
			sa[13] += a[i].x*_w[i];
			sa[14] += 0;
			sa[15] += 1*_w[i];

			sb[0] += a[i].x*b[i].x*_w[i] + a[i].y*b[i].y*_w[i];
			sb[1] += a[i].x*b[i].y*_w[i] - a[i].y*b[i].x*_w[i];
			sb[2] += b[i].x*_w[i];
			sb[3] += b[i].y*_w[i];
		}

		cvSolve( &A, &B, &MM, CV_SVD );

		om[0] = om[4] = m[0];
		om[1] = -m[1];
		om[3] = m[1];
		om[2] = m[2];
		om[5] = m[3];
	}
}

Mat CWarpUtils::EstimateHWeighted( PointSetd _pnt1, PointSetd _pnt2, bool _flag ) {
	double m[6]={0};
	CvMat M = cvMat( 2, 3, CV_64F, m);
	vectord weights(66, 1);

	for (int i = 48; i <= 65; i++)
		weights[i] = 0; 
	weights[39] = 10; weights[42] = 10; weights[27] = 7;

	weights[36] = 7; weights[45] = 7;  weights[30] = 2;  weights[33] = 2; weights[48] = 4; weights[54] = 4; 
	weights[31] = 7; weights[35] = 7; weights[17] = 4; weights[21] = 5; weights[22] = 5; weights[26] = 4; 
	EstimateHWeighted(_pnt1, _pnt2, weights, &M, _flag);
	Mat H = cvarrToMat(&M).clone();
	return H; 

}

Mat CWarpUtils::LoadMeanPnts3D( string _path ) {
	ifstream fin(_path.c_str());
	if (fin != NULL) {
		Mat landmark = Mat::zeros(Size(3, 9), CV_64FC1); 
		FOR (i, 9) {
			double x, y, z; //= 0; 
			fin >> x >> y >> z; 
			landmark.at<double>(i, 0) = x;
			landmark.at<double>(i, 1) = y;
			landmark.at<double>(i, 2) = z;
		}
		fin.close();
		return landmark;
	} else {
		DEBUG_ERROR("cannot load mean pnts from (%s)", _path.c_str());
		return Mat(); 
	}
}




