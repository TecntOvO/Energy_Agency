#pragma once

#ifndef SETTING_HPP_
#define SETTING_HPP_

#include <opencv2\opencv.hpp>

class Setting {
public:
    Setting() = default;
    explicit Setting(cv::String path);

	int Blue_Binary_Value;   //二值化阈值  
    int Red_Binary_Value;

    //灯条筛选相关数据
    float CL_Maxratio;  //圆形灯条最大长宽比
    float CircleLight_area; //圆形灯条最小面积
    float BL_Maxratio, BL_Minratio; //大灯条最大以及最小长宽比
    float SL_Maxratio, SL_Minratio;
    float HL_Maxratio, HL_Minratio;
    float HL_Maxarea, HL_Minarea;
    float AttackArea_ratio;
    float R_minratio;
    int R_minarea, R_maxarea;

    int blue_st;    //选择处理图像来源 1 提取红色图 2 提取蓝色图 0 灰度图 3 HSV图
    int blue_Sin;   //色阶调整
    int blue_erode; //腐蚀次数
    int blue_dilate;    //膨胀次数
    int blue_ks;    //形态学处理结构核大小

    int red_st;     //选择处理图像来源 1 提取红色图 2 提取蓝色图 0 灰度图 3 HSV图
    int red_Sin;    //色阶调整
    int red_erode;  //腐蚀次数
    int red_dilate; //膨胀次数
    int red_ks; //形态学处理结构核大小

    //HSV色彩提取
    int blue_iLowH;
    int blue_iHighH;
    int blue_iLowS;
    int blue_iHighS;
    int blue_iLowV;
    int blue_iHighV;

    int red_iLowH;
    int red_iHighH;
    int red_iLowS;
    int red_iHighS;
    int red_iLowV;
    int red_iHighV;


private:
    cv::String path;

};


#endif
