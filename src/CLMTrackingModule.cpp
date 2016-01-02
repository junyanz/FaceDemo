#include "CLMTrackingModule.h"

CCLMTrackingModule::CCLMTrackingModule() {
	Clear(); 
	failed = false; 
	fpd = -1; 
    m_model = new FACETRACKER::Tracker ("./model/face2.tracker");
    m_tri=FACETRACKER::IO::LoadTri("./model/face.tri");
    m_con=FACETRACKER::IO::LoadCon("./model/face.con");
}

void Draw(cv::Mat &image,cv::Mat &shape,cv::Mat &con,cv::Mat &tri,cv::Mat &visi)
{
	int i,n = shape.rows/2; cv::Point p1,p2; cv::Scalar c;

	//draw points
	for(i = 0; i < n; i++){    
		if(visi.at<int>(i,0) == 0)continue;
		p1 = cv::Point(shape.at<double>(i,0),shape.at<double>(i+n,0));
		c = CV_RGB(255,0,0); cv::circle(image,p1,2,c);
		stringstream ss; 
		ss << i; 
		cv::putText(image, ss.str(), Point(p1.x+1,p1.y+1),  CV_FONT_HERSHEY_SIMPLEX,0.4,CV_RGB(255,255,255));
	}return;
}


bool CCLMTrackingModule::TrackFrame( const Mat& _frame, bool _isVis /*= false*/ ) {
	int nIter = 5; 
	double clamp=3; 
	double fTol=0.01; 
	bool fcheck = false; 
	double scale = 1; 
	
	//bool show = true;
	//set other tracking parameters
	std::vector<int> wSize1(1); wSize1[0] = 7;
	std::vector<int> wSize2(3); wSize2[0] = 11; wSize2[1] = 9; wSize2[2] = 7;
	//int nIter = 5; 
	Mat im = _frame.clone();
	m_frame = im; 
	Mat gray; 
	//else cv::resize(frame,im,cv::Size(scale*frame.cols,scale*frame.rows));
	//Mat im; 
	//imshow("before", im

	//cv::flip(im,im,1);
	 cv::cvtColor(im,gray,CV_BGR2GRAY);

	//track this image
	std::vector<int> wSize; if(failed)wSize = wSize2; else wSize = wSize1; 
	if(m_model->Track(gray,wSize,fpd,nIter,clamp,fTol,fcheck) == 0){
		int idx = m_model->_clm.GetViewIdx(); failed = false;
		Draw(im,m_model->_shape,m_con,m_tri,m_model->_clm._visi[idx]); 
	} else{
		if(_isVis){cv::Mat R(im,cvRect(0,0,150,50)); R = cv::Scalar(0,0,255);}
		m_model->FrameReset(); failed = true;
	}     

	//show image and check for user input
	if (_isVis) {
		//imshow("Face Tracker",im);
		//waitKey();
	}
	
	return !failed; 
}

void CCLMTrackingModule::Restart() {
}

PointSetd CCLMTrackingModule::GetLandmarks() {
	PointSetd pnts(NUM_POINTS); 
	int idx[NUM_POINTS] = {36,39,42,45,31,33,35,48,54}; // index
	int n = m_model->_shape.rows/2;
	FOR (ii, NUM_POINTS) {
		int i = idx[ii];
		pnts[ii] = cv::Point(m_model->_shape.at<double>(i,0),
			m_model->_shape.at<double>(i+n,0));
	}

	Mat disp = m_frame.clone();
	FOR (i, NUM_POINTS)
		circle(disp, pnts[i], 2, Scalar(0, 255, 0), 2);
//	imshow("Detected Points", disp);
//	waitKey();
	return pnts; 
}

void CCLMTrackingModule::Init()
{

}

CCLMTrackingModule::~CCLMTrackingModule() {
	Clear(); 
}

void CCLMTrackingModule::Clear() {
}
