#include "Setting.hpp"


Setting::Setting(cv::String path):path(path) {
	cv::FileStorage fs(path, cv::FileStorage::READ);

    fs["circlelight_Highratio"] >> this->CL_Maxratio;
    fs["circlelight_Area"] >> this->CircleLight_area;
    fs["biglight_Highratio"] >> this->BL_Maxratio;
    fs["biglight_Lowratio"] >> this->BL_Minratio;
    fs["smalllight_Highratio"] >> this->SL_Maxratio;
    fs["smalllight_Lowratio"] >> this->SL_Minratio;
    fs["hugelight_Highratio"] >> this->HL_Maxratio;
    fs["hugelight_Lowratio"] >> this->HL_Minratio;
    fs["hugelight_Maxarea"] >> this->HL_Maxarea;
    fs["hugelight_Minarea"] >> this->HL_Minarea;
    fs["attackarea_Lowratio"] >> this->AttackArea_ratio;

    fs["blue_binary_value"] >> this->Blue_Binary_Value;
    fs["blue_st"] >> this->blue_st; 
    fs["blue_Sin"] >> this->blue_Sin;
    fs["blue_erode"] >> this->blue_erode;
    fs["blue_dilate"] >> this->blue_dilate;
    fs["blue_ks"] >> this->blue_ks;

    fs["red_binary_value"] >> this->Red_Binary_Value;
    fs["red_st"] >> this->red_st;
    fs["red_Sin"] >> this->red_Sin;
    fs["red_erode"] >> this->red_erode;
    fs["red_dilate"] >> this->red_dilate;
    fs["red_ks"] >> this->red_ks;

    fs["blue_iLowH"] >> this->blue_iLowH;
    fs["blue_iHighH"] >> this->blue_iHighH;
    fs["blue_iLowS"] >> this->blue_iLowS;
    fs["blue_iHighS"] >> this->blue_iHighS;
    fs["blue_iLowV"] >> this->blue_iLowV;
    fs["blue_iHighV"] >> this->blue_iHighV;

    fs["red_iLowH"] >> this->red_iLowH;
    fs["red_iHighH"] >> this->red_iHighH;
    fs["red_iLowS"] >> this->red_iLowS;
    fs["red_iHighS"] >> this->red_iHighS;
    fs["red_iLowV"] >> this->red_iLowV;
    fs["red_iHighV"] >> this->red_iHighV;

    fs.release();
}