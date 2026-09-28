#include "Camera.h"
#include <iostream>

// ================== 构造函数 ==================
Camera::Camera() {
    int ret = 0;

    ret = MV_CC_EnumDevices(MV_USB_DEVICE, &device_list_);
    if (ret != MV_OK || device_list_.nDeviceNum == 0) {
        std::cout << "[Error] 未发现任何设备!" << std::endl;
        return;
    }
    std::cout << "[Info] 发现设备数量: " << device_list_.nDeviceNum << std::endl;

    ret = MV_CC_CreateHandle(&handle_, device_list_.pDeviceInfo[0]);
    if (ret != MV_OK) {
        std::cout << "[Error] 创建句柄失败!" << std::endl;
        return;
    }

    ret = MV_CC_OpenDevice(handle_);
    if (ret != MV_OK) {
        std::cout << "[Error] 打开设备失败!" << std::endl;
        return;
    }

    MV_CC_SetEnumValue(handle_, "BalanceWhiteAuto", MV_BALANCEWHITE_AUTO_CONTINUOUS);
    MV_CC_SetEnumValue(handle_, "ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
    MV_CC_SetEnumValue(handle_, "GainAuto", MV_GAIN_MODE_OFF);
    MV_CC_SetFloatValue(handle_, "ExposureTime", 10000);
    MV_CC_SetFloatValue(handle_, "Gain", 20);
    MV_CC_SetFrameRate(handle_, 60);

    ret = MV_CC_StartGrabbing(handle_);
    if (ret != MV_OK) {
        std::cout << "[Error] 开始采集失败!" << std::endl;
    } else {
        std::cout << "[Info] 相机初始化完成，开始采集..." << std::endl;
    }

    nMsec_ = 1000;
}

// ================== 析构函数 ==================
Camera::~Camera() {
    int ret = 0;

    ret = MV_CC_StopGrabbing(handle_);
    if (ret != MV_OK) std::cout << "[Warning] 停止采集失败" << std::endl;

    ret = MV_CC_CloseDevice(handle_);
    if (ret != MV_OK) std::cout << "[Warning] 关闭设备失败" << std::endl;

    ret = MV_CC_DestroyHandle(handle_);
    if (ret != MV_OK) std::cout << "[Warning] 销毁句柄失败" << std::endl;

    std::cout << "[Info] 相机资源已释放。" << std::endl;
}

// ================== read 函数 ==================
cv::Mat Camera::read() {
    int ret = 0;

    ret = MV_CC_GetImageBuffer(handle_, &raw_frame_, nMsec_);
    if (ret != MV_OK) {
        std::cout << " 获取图像失败 " << std::endl;
        return cv::Mat(); 
    }

    cv::Mat img = transfer(raw_frame_);

    ret = MV_CC_FreeImageBuffer(handle_, &raw_frame_);
    if (ret != MV_OK) {
        std::cout << "[Error] 释放图像缓冲区失败" << std::endl;
    }

    return img; 
}

// ================== transfer 辅助函数 ==================
cv::Mat Camera::transfer(MV_FRAME_OUT& raw_frame_) {
    MV_CC_PIXEL_CONVERT_PARAM convert_param_ = {};
    cv::Mat img;

    convert_param_.nWidth = raw_frame_.stFrameInfo.nWidth;     
    convert_param_.nHeight = raw_frame_.stFrameInfo.nHeight;  
    convert_param_.pSrcData = raw_frame_.pBufAddr;              
    convert_param_.nSrcDataLen = raw_frame_.stFrameInfo.nFrameLen;
    convert_param_.enSrcPixelType = raw_frame_.stFrameInfo.enPixelType; 
    convert_param_.enDstPixelType = PixelType_Gvsp_BGR8_Packed; 

    unsigned int nDstBufSize = raw_frame_.stFrameInfo.nWidth * raw_frame_.stFrameInfo.nHeight * 3;
    char* pDstBuffer = new char[nDstBufSize]; 
    convert_param_.pDstBuffer = pDstBuffer;
    convert_param_.nDstBufferSize = nDstBufSize;

    int ret = MV_CC_ConvertPixelType(handle_, &convert_param_);

    if (ret == MV_OK) {

        img = cv::Mat(raw_frame_.stFrameInfo.nHeight, raw_frame_.stFrameInfo.nWidth, CV_8UC3, pDstBuffer).clone();
    } else {
        std::cout << "像素格式转换失败" << std::endl;
    }

    delete[] pDstBuffer; 
    return img;
}