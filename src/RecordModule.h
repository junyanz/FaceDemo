/*******************************************************************************
* Simple 3D Face Modeling 
* Copyright 2014 Jun-Yan Zhu (junyanz at eecs dot berkeley dot edu)
* Please email me if you find bugs, or have suggestions or questions!
* Licensed under the MIT License [see README]
*******************************************************************************/

#ifndef RECORD_MODULE_H_
#define RECORD_MODULE_H_
#include "common.h"

enum InputMode {
    IM_IMAGE,
    IM_VIDEO,
};

class CRecordModule {
public: 
	CRecordModule(); 
	CRecordModule(string _videoFile); 
	~CRecordModule();
	bool IsOpened(); 
	Mat GetFrame(); 
	Size GetFrameSize(); 

private: 
	void Clear(); 
private: 
	VideoCapture m_cap; 
	Size m_size; 
	Mat m_lastFrame;
}; 

#endif //RECORD_MODULE_H_
