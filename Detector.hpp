#pragma once
#ifndef ENERGY_AGENCY_DETECTOR_HPP_
#define ENERGY_AGENCY_DETECTOR_HPP_

#include <vector>
#include <opencv2\opencv.hpp>

#include "Setting.hpp"

//大能量机关灯条颜色
#define RED  0
#define BLUE 1

//直线类
class Line {
public:
	Line() = default;
	explicit Line(const float& a, const float& b, const float& c);	
	explicit Line(const cv::Point &p1,const cv::Point &p2);	
	Line GetNormalLine(const cv::Point& p);	
	float GetY(const float& x);	
	bool IfSameSide(const cv::Point& p1, const cv::Point& p2); 
	bool IfBetweenLines(const cv::Point& p, const Line& line);	
	void DrawLine(cv::Mat InputArray, const cv::Scalar& color, const int& thickness);

private:
	float a, b, c; //AX+BY+C=0
};

class Light :public cv::RotatedRect {
public:
	Light() = default;
	explicit Light(const cv::RotatedRect &box,const std::vector<cv::Point> &contours) : cv::RotatedRect(box),contour(contours)
	{
		cv::Point2f p[4];
		box.points(p);  //旋转矩形四个角点坐标

		double value1 = cv::norm(p[0] - p[1]); 
		double value2 = cv::norm(p[1] - p[2]); 
		if (value1 < value2|| value1 == value2) {
			point[0] = p[0];point[1] = p[1];point[2] = p[2];point[3] = p[3];
			length = value2;width = value1;
		}	
		else {
			point[0] = p[1];point[1] = p[2];point[2] = p[3];point[3] = p[0];
			length = value1;width = value2;
		} 
		ratio = length / width;
	}
	cv::Point2f point[4];
	cv::Point2f top, bottom;
	double length;
	double width;
	double ratio;
	std::vector<cv::Point> contour;

private:

};

//检测类
class Detector {
public:
	//Detector(const int& bin_thres, const int& color, const LightParams& R);
	Detector(const cv::String path);
	//void UpdateParams(const int& bin_thres, const int& color);
	void detect(const cv::Mat& input,const int& color);
	cv::Mat preprocessImage(const cv::Mat& input);
	cv::Mat levelAdjust(const cv::Mat& img, int Sin, int Hin = 255, double Mt = 1.0, int Sout = 0, int Hout = 255);
	cv::Mat useHSV(const cv::Mat &src, const int& iLowH, const int& iHighH, const int& iLowS, const int& iHighS, const int& iLowV, const int& iHighV);
	void findLights();
	void ClassifyLights();
	void GetAttackTarget();
	const cv::Mat drawResult(const int& Type);
	cv::Mat GetPreprocess_Img();

	Setting settings;
	
	cv::Mat target_img;
	cv::Mat adjust_img;
	cv::Mat preprocess_src_img;
	
private:
	int Detector_color;
	std::vector<Light> Circle_L;
	std::vector<Light> Big_L;
	std::vector<Light> Small_L;
	std::vector<Light> Huge_L;
	Light* Attack_Target;
	Light* R_Target;
	cv::Mat preprocess_img;
	cv::Mat src_img;
};

#endif