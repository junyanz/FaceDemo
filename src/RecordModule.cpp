#include "RecordModule.h"

void CRecordModule::Clear() {
	m_size = Size(-1, -1); 
}

CRecordModule::CRecordModule() {
	Clear(); 
	m_cap.open(0); 
}

CRecordModule::CRecordModule( string _videoFile ) {
	Clear(); 
	m_cap.open(_videoFile); 
}

CRecordModule::~CRecordModule() { 
	Clear(); 
}

bool CRecordModule::IsOpened() {
	if (m_cap.isOpened()) 
		DEBUG_INFO("Camera is open"); 
	else 
		DEBUG_ERROR("Camera is disabled"); 
	return m_cap.isOpened();  
}

cv::Mat CRecordModule::GetFrame() {
	Mat frame; 
	m_cap >> frame; 
	return frame; 
}

Size CRecordModule::GetFrameSize() {
	if (m_size == Size(-1, -1))
		m_size = GetFrame().size(); 
	return m_size; 
}

