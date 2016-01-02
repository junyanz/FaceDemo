/*******************************************************************************
* Simple 3D Face Modeling 
* Copyright 2014 Jun-Yan Zhu (junyanz at eecs dot berkeley dot edu)
* Please email me if you find bugs, or have suggestions or questions!
* Licensed under the MIT License [see README]
* Description: This is the main entry class. 
*******************************************************************************/
#ifndef FACE_DEMO_H_
#define  FACE_DEMO_H_
#include "common.h"
#include "RecordModule.h"
//#include "module/SDMDetectModule.h"
//#include "module/SDMTrackModule.h"
#include "CLMTrackingModule.h"
#include "WarpingModule.h"

class CFaceDemo {
public: 
	CFaceDemo();
    CFaceDemo(InputMode _mode, string _fileName);
//    CFaceDemo(string _fName);
	~CFaceDemo();
	Mat Texture();
	Mat Depth() { return m_warpModule->Z(); }
	void Transform(Mat& _ar, Mat& _H, Point2d& _t) { m_warpModule->Transform(_ar, _H, _t); }
	Size ImageSize();

private: 
	void Clear();
	void ReadConfig(string _fName);
	void Init();

private:
	InputMode m_inputMode;
	string m_fileName; 
	Mat m_inputImg; 
	Mat m_imgTexture; 
	CRecordModule* m_recorder;
	CWarpingModule* m_warpModule;
	//CSDMDetectModule* m_detector;
	//CSDMTrackModule* m_tracker;
	CCLMTrackingModule* m_tracker; 
	vector<PointSetd> m_recentPnts; 
};


#endif //FACE_DEMO_H_
