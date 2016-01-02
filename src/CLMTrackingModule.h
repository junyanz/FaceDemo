#ifndef CLM_TRACKING_MODULE_H_
#define CLM_TRACKING_MODULE_H_
#include "common.h"
#include "../clm/Tracker.h"
class CCLMTrackingModule {
public: 
	CCLMTrackingModule(); 
	~CCLMTrackingModule();
	void Init();
	bool TrackFrame(const Mat& _frame, bool _isVis = false);
	void StartTracking() { m_isStart = true; } 
	bool IsStart() { return m_isStart; }
	PointSetd GetLandmarks(); 
	void Restart(); 
private: 
	void Clear();

private: 
	bool m_isStart; 
	FACETRACKER::Tracker* m_model; 
	Mat m_tri; 
	Mat m_con; 
	bool failed;// = true;
	int fpd; //= -1; 
	Mat m_frame; 
};

#endif //CLM_TRACKING_MODULE_H_