#pragma once

#ifndef SETTING_HPP_
#define SETTING_HPP_

#include <opencv2\opencv.hpp>

class Setting {
public:
    Setting() = default;
    explicit Setting(cv::String path);

	int Blue_Binary_Value;   //��ֵ����ֵ  
    int Red_Binary_Value;

    //����ɸѡ�������
    float CL_Maxratio;  //Բ�ε�����󳤿��
    float CircleLight_area; //Բ�ε�����С���
    float BL_Maxratio, BL_Minratio; //���������Լ���С�����
    float SL_Maxratio, SL_Minratio;
    float HL_Maxratio, HL_Minratio;
    float HL_Maxarea, HL_Minarea;
    float AttackArea_ratio;
    float R_minratio;
    int R_minarea, R_maxarea;

    int blue_st;    //ѡ����ͼ����Դ 1 ��ȡ��ɫͼ 2 ��ȡ��ɫͼ 0 �Ҷ�ͼ 3 HSVͼ
    int blue_Sin;   //ɫ�׵���
    int blue_erode; //��ʴ����
    int blue_dilate;    //���ʹ���
    int blue_ks;    //��̬ѧ����ṹ�˴�С

    int red_st;     //ѡ����ͼ����Դ 1 ��ȡ��ɫͼ 2 ��ȡ��ɫͼ 0 �Ҷ�ͼ 3 HSVͼ
    int red_Sin;    //ɫ�׵���
    int red_erode;  //��ʴ����
    int red_dilate; //���ʹ���
    int red_ks; //��̬ѧ����ṹ�˴�С

    //HSVɫ����ȡ
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
