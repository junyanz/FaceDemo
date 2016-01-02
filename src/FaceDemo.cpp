#include "FaceDemo.h"
#include "WarpUtils.h"
const int NUM_FRAME_ALIGN = 5;  // for temporal smoothing

CFaceDemo::CFaceDemo() {
    Clear();
}

CFaceDemo::CFaceDemo(InputMode _mode, string _fileName) {
    Clear();
    m_inputMode = _mode;
    m_fileName = _fileName;
    Init();
}

CFaceDemo::~CFaceDemo() {
    DELETE_OBJECT(m_recorder);
    DELETE_OBJECT(m_tracker);
    DELETE_OBJECT(m_warpModule);
    Clear();
}

void CFaceDemo::Clear() {
    m_recorder = NULL;
    m_warpModule = NULL;
    m_tracker = NULL;
    m_recentPnts.clear();
}


void CFaceDemo::Init() {
    m_warpModule = new CWarpingModule();

    if (m_inputMode == IM_VIDEO) {
        m_recorder = new CRecordModule(m_fileName);
        m_tracker = new CCLMTrackingModule();
        m_tracker->Init();
        m_tracker->StartTracking();
    } else  {
        m_inputImg = imread(m_fileName);
        if (m_inputImg.empty())
            DEBUG_ERROR("cannot read the image (%s)", m_fileName.c_str());

        m_tracker = new CCLMTrackingModule();
        //Mat landmark;
        m_tracker->TrackFrame(m_inputImg, true); //, landmark);
        PointSetd landmark = m_tracker->GetLandmarks();
        if (!landmark.empty()) {
            Mat disp = m_inputImg.clone();
            FOR (i, NUM_POINTS)
                circle(disp, landmark[i], 2, Scalar(0, 255, 0), 2);
            imshow("Detected Points", disp);

            waitKey(100);

            m_warpModule->WarpToFrontalFace3D(m_inputImg, landmark);
            m_imgTexture = m_warpModule->FrontalFace();
        } else
            DEBUG_ERROR("cannot detect points (%s)", m_fileName.c_str());
    }
}

cv::Mat CFaceDemo::Texture() {
    if (m_inputMode == IM_IMAGE)
        return m_imgTexture;
    else {
        Mat frame = m_recorder->GetFrame();
        bool isTrack = m_tracker->TrackFrame(frame, false);
        if (isTrack) {
            PointSetd landmark = m_tracker->GetLandmarks();
            if ((int)m_recentPnts.size() >= NUM_FRAME_ALIGN)
                m_recentPnts.erase(m_recentPnts.begin());
            m_recentPnts.push_back(landmark);
            PointSetd medianPnts = CWarpUtils::MedianPnts(m_recentPnts);
            m_warpModule->WarpToFrontalFace3D(frame, medianPnts);
            Mat disp = frame.clone();
            FOR (i, NUM_POINTS)
                circle(disp, medianPnts[i], 2, Scalar(0, 255, 0), 2);
            imshow("Detected Points", disp);
            waitKey(1);
            return m_warpModule->FrontalFace();
        } else
            return Mat();
    }
}

Size CFaceDemo::ImageSize() {
    if (m_inputMode == IM_IMAGE)
        return m_inputImg.size();
    else
        return m_recorder->GetFrameSize();
}

