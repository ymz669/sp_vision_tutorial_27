#include "io/camera.hpp"
#include "tasks/yolo.hpp"
#include "opencv2/opencv.hpp"
#include "tools/img_tools.hpp"
#include <iostream>
using namespace auto_aim;
int main()
{
  // 1. 初始化相机、yolo类
    Camera camera; 
    // 按照提示，传入配置文件路径 ./configs/yolo.yaml
    Yolo yolo("./configs/yolo.yaml", true); 

    cv::Mat img;
    int frame_count = 0; 

    while (true) {
        // 2. 调用相机读取图像
        img = camera.read();
        if (img.empty()) {
            std::cerr << "[Warning] 获取图像为空，跳过此帧！" << std::endl;
            continue;
        }

        // 3. 调用yolo识别装甲板
        // 传入图像和当前帧数
        std::list<Armor> results = yolo.detect(img, frame_count++); 

        // 4. 在图片上将装甲板的四个关键点连接成闭合矩形，并使用绿色绘制
        for (const auto& armor : results) {
            draw_points(img, armor.points, cv::Scalar(0, 255, 0), 2);
        }

        // 5. 显示图像
        cv::resize(img, img, cv::Size(640, 480));
        cv::imshow("img", img);
        
        if (cv::waitKey(0) == 'q') {
            break;
        }
    }

    cv::destroyAllWindows();
    return 0;
}