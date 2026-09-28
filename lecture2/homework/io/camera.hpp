#ifndef CAMERA_H
#define CAMERA_H

#include "hikrobot/include/MvCameraControl.h" 
#include <opencv2/opencv.hpp>    

class Camera {
public:
    // 【构造函数】：负责打开相机、设置参数、开始采集
    Camera();

    // 【析构函数】：负责停止采集、关闭相机、释放资源
    ~Camera();

    // 【读取函数】：负责获取一帧图像并返回 OpenCV 格式
    cv::Mat read();

private:

    cv::Mat transfer(MV_FRAME_OUT& raw_);

private:
    void* handle_;      
    MV_CC_DEVICE_INFO_LIST device_list_;
    unsigned int nMsec_;            
    MV_FRAME_OUT raw_frame_;           
};

#endif // CAMERA_H