#include "Detector.hpp"
#include <fstream>

//直接构造直线
Line::Line(const float& a, const float& b, const float& c):a(a),b(b),c(c){}

//根据两点构造直线
Line::Line(const cv::Point& p1, const cv::Point& p2) {
	a = p1.y - p2.y;
	b = p2.x - p1.x;
	c = -a * p1.x - b * p1.y;
}

//过一点求该直线法线
Line Line::GetNormalLine(const cv::Point& p) {
	float a_ = -b;
	float b_ = a;
	float c_ = -a_ * p.x - b_ * p.y;
	return Line(a_, b_, c_);
}

//根据x求y
float Line::GetY(const float& x) {
	if (b != 0)return (-c - a * x) / b;
	else return 0;
}

//判断两点是否在直线同侧
bool Line::IfSameSide(const cv::Point& p1, const cv::Point& p2) {
	float value1 = a * p1.x + b * p1.y + c;
	float value2 = a * p2.x + b * p2.y + c;
	if (value1 * value2 > 0 || (value1 == 0 && value2 == 0))return true;
	return false;
}

//判断一点是否在两线之间
bool Line::IfBetweenLines(const cv::Point& p, const Line& line) {
	float value1 = a * p.x + b * p.y + c;
	float value2 = line.a * p.x + line.b * p.y + line.c;
	if (value1 * value2 < 0)return true;
	else return false;
}

//绘制直线
void Line::DrawLine(cv::Mat InputArray,const cv::Scalar & color,const int & thickness) {
	using namespace cv;
	line(InputArray, Point(0, GetY(0)), Point(InputArray.size().width, GetY(InputArray.size().width)),color, thickness);
}



Detector::Detector(const std::string& pharams_path, const int& color) {
	settings = Setting(pharams_path);
	//net_ = cv::dnn::readNetFromONNX(settings.model_path);

	std::ifstream lable_file(settings.label_path);
	cv::String line;
	while (std::getline(lable_file, line)) {
		class_names_.push_back(line);
	}

	if (color == ENERGY_COLOR::RED) {
		pharams.st = settings.red_st;
		pharams.sin = settings.red_Sin;
		pharams.iLowH = settings.red_iLowH;
		pharams.iHighH = settings.red_iHighH;
		pharams.iLowS = settings.red_iLowS;
		pharams.iHighS = settings.red_iHighS;
		pharams.iLowV = settings.red_iLowV;
		pharams.iHighV = settings.red_iHighV;
		pharams.Binary_Value = settings.Red_Binary_Value;
		pharams.ks = settings.red_ks;
		pharams.erode = settings.red_erode;
		pharams.dilate = settings.red_dilate;
	}
	else if (color == ENERGY_COLOR::BLUE) {
		pharams.st = settings.blue_st;
		pharams.sin = settings.blue_Sin;
		pharams.iLowH = settings.blue_iLowH;
		pharams.iHighH = settings.blue_iHighH;
		pharams.iLowS = settings.blue_iLowS;
		pharams.iHighS = settings.blue_iHighS;
		pharams.iLowV = settings.blue_iLowV;
		pharams.iHighV = settings.blue_iHighV;
		pharams.Binary_Value = settings.Blue_Binary_Value;
		pharams.ks = settings.blue_ks;
		pharams.erode = settings.blue_erode;
		pharams.dilate = settings.blue_dilate;
	}
}

//void Detector::UpdateParams(const int& bin_thres_, const int& color_) {
//	settings.Binary_Value = bin_thres_;
//	settings.Detect_Color = color_;
//}

void Detector::detect(const cv::Mat& input){
	findAT = false;
	findR = false;
	std::vector<cv::Mat> src_channels;
	input.copyTo(src_img);
	adjust_img = levelAdjust(input, pharams.sin);
	split(adjust_img, src_channels);

	if (pharams.st == 0)cvtColor(adjust_img, preprocess_src_img, cv::COLOR_BGR2GRAY);	//转化为灰度图
	else if (pharams.st == 1)subtract(src_channels[2], src_channels[0], preprocess_src_img);	//红色通道 - 蓝色通道（提取红色）
	else if (pharams.st == 2)subtract(src_channels[0], src_channels[2], preprocess_src_img);	//蓝色通道 - 红色通道（提取蓝色）
	else if (pharams.st == 3)preprocess_src_img = useHSV(adjust_img, pharams.iLowH, pharams.iHighH, pharams.iLowS, pharams.iHighS, pharams.iLowV, pharams.iHighV);

	preprocess_img = preprocessImage(preprocess_src_img);
	findLights();
	ClassifyLights();
	GetAttackTarget();
}

cv::Mat Detector::preprocessImage(const cv::Mat& input) {
	using namespace cv;
	
	Mat process_img;
	Mat kernel = getStructuringElement(MORPH_RECT, Size(pharams.ks, pharams.ks));

	threshold(input, process_img, pharams.Binary_Value, 255, THRESH_BINARY);	//二值化
	
	morphologyEx(process_img, process_img, MORPH_ERODE, kernel, Point(-1, -1), pharams.erode);
	morphologyEx(process_img, process_img, MORPH_DILATE, kernel, Point(-1, -1), pharams.dilate);
	return process_img;
}

//色阶调整
cv::Mat Detector::levelAdjust(const cv::Mat & img, int Sin, int Hin, double Mt, int Sout, int Hout)
{
	using namespace cv;
	Sin = min(max(Sin, 0), Hin - 2);               // Sin, 黑场阈值, 0 <= Sin < Hin
	Hin = min(Hin, 255);                           // Hin, 白场阈值, Sin < Hin <= 255
	Mt = min(max(Mt, (double)0.01), (double)9.99); // Mt, 灰场调节值, 0.01~9.99
	Sout = min(max(Sout, 0), Hout - 2);            // Sout, 输出黑场阈值, 0 <= Sout < Hout
	Hout = min(Hout, 255);                         // Hout, 输出白场阈值, Sout < Hout <= 255

	double difIn = Hin - Sin;
	double difOut = Hout - Sout;
	uchar tableData[256];
	for (int i = 0; i < 256; i++)
	{
		double V1 = min(max(255 * (i - Sin) / difIn, (double)0), (double)255);     // 输入动态线性拉伸
		double V2 = 255 * std::pow(V1 / 255, 1 / Mt);                              // 灰场伽马调节
		tableData[i] = min(max(Sout + difOut * V2 / 255, (double)0), (double)255); // 输出线性拉伸
	}
	Mat table(1, 256, CV_8UC1, tableData);
	Mat imgTone;
	LUT(img, table, imgTone);
	return imgTone;
}


//HSV图像提取
cv::Mat Detector::useHSV(const cv::Mat& src, const int& iLowH, const int& iHighH, const int& iLowS, const int& iHighS, const int& iLowV, const int& iHighV)
{
	using namespace cv;
	Mat HSVMat, mergeMat;
	std::vector<Mat> planes;
	cvtColor(src, HSVMat, COLOR_BGR2HSV);
	split(HSVMat, planes);
	equalizeHist(planes[2], planes[2]);
	merge(planes, mergeMat);
	Mat imgThresholded;
	inRange(mergeMat, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);
	return imgThresholded;
}

/*
首先要找出等待打击目标
随后找出符合中心R的轮廓
根据直线关系限制R的轮廓
*/
void Detector::findLights() {
	using std::vector;
	vector<vector<cv::Point>> contours;
	vector<cv::Vec4i> hierarchy;

	Circle_L.clear();
	Big_L.clear();
	Small_L.clear();
	Huge_L.clear();

	cv::findContours(preprocess_img, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
	//遍历所有轮廓
	auto it_contour = contours.begin();
	auto it_hierarchy = hierarchy.begin();

	while (it_contour != contours.end() && it_hierarchy != hierarchy.end()) {
		//排除点少于5个的轮廓（此时不能用矩形拟合）
		if (it_contour->size() > 4) {
			//矩形拟合
			auto r_rect = cv::minAreaRect(*it_contour);
			auto light = Light(r_rect, *it_contour);

			//轮廓分类
			double light_area = light.length * light.width;
			if (light.ratio<settings.CL_Maxratio && light_area>settings.CircleLight_area) {
				Circle_L.push_back(light);
			}
			else if (light.ratio < settings.BL_Maxratio && light.ratio>settings.BL_Minratio) {
				Big_L.push_back(light);
			}
			else if (light.ratio < settings.SL_Maxratio && light.ratio>settings.SL_Minratio) {
				Small_L.push_back(light);
			}
			else if (light.ratio < settings.HL_Maxratio && light.ratio > settings.HL_Minratio &&
				light_area > settings.HL_Minarea && light_area < settings.HL_Maxarea) {
				Huge_L.push_back(light);
			}
		}
		it_contour++;it_hierarchy++;
	}
}

void Detector::ClassifyLights() {
	for (auto light = Huge_L.begin();light != Huge_L.end();light++) {
		float Area_raito = light->length * light->width / cv::contourArea(light->contour);
		 //cv::putText(result_img, std::to_string(Area_raito), light->center, 1, 2, cv::Scalar(255, 255, 255),3);
		if (Area_raito > settings.AttackArea_ratio) {
			AT=Attack_Target(*light);
			findAT = true;
			break;
		}
	}
	//利用打击目标构造直线筛选中心R
	if (findAT) {
		//根据两条短边构造直线
		auto ReferLine_1 = Line(AT.point[0], AT.point[1]);
		auto ReferLine_2 = Line(AT.point[2], AT.point[3]);

		//绘制直线
		//ReferLine_1.DrawLine(src_img, cv::Scalar(255, 255, 255), 2);
		//ReferLine_2.DrawLine(src_img, cv::Scalar(255, 255, 255), 2);


		for (auto light = Circle_L.begin();light != Circle_L.end();light++) {
			//根据其中一条短边做过圆形灯条轮廓中心点的法线
			auto normalLine = ReferLine_1.GetNormalLine(light->center);
			//绘制直线
			//normalLine.DrawLine(src_img, cv::Scalar(255, 255, 255), 2);

			//当法线与另一条直线的两点间有交点（即直线在两点之间时），并且该中心点不在两条短边所做直线之间时判断为R点
			if (!normalLine.IfSameSide(AT.point[2], AT.point[3]) &&
				ReferLine_1.IfBetweenLines(light->center, ReferLine_2)) {
				RT = R_Target(*light);
				findR = true;
				break;
			}
		}
	}

	//无打击目标情况
	else {
		for (auto light = Circle_L.begin();light != Circle_L.end();light++) {
			double area = light->length * light->width;
			if (light->ratio > settings.R_minratio&&area>settings.R_minarea&& area < settings.R_maxarea) {
				RT = R_Target(*light);
				findR = true;
				break;
			}
		}
	}
}

void Detector::GetAttackTarget() {
	if (findAT) {
		using namespace cv;
		Point2f recpoint[4];
		Mat perspectMat;
		recpoint[0] = Point2f(0, 0);
		recpoint[1] = Point2f(AT.width, 0);
		recpoint[2] = Point2f(AT.width, AT.length);
		recpoint[3] = Point2f(0, AT.length);

		//投射变换
		Mat transform = getPerspectiveTransform(AT.point, recpoint);
		warpPerspective(src_img, perspectMat, transform, src_img.size());
		target_img = perspectMat(Rect(0,0, AT.width, AT.length));

		/*Mat img = target_img.clone();
		img = img / 255.0;

		Mat blob;
		cv::dnn::blobFromImage(img, blob);
		net_.setInput(blob);
		Mat outputs = net_.forward();

		float max_prob = *std::max_element(outputs.begin<float>(), outputs.end<float>());
		Mat softmax_prob;
		exp(outputs - max_prob, softmax_prob);
		float sum = static_cast<float>(cv::sum(softmax_prob)[0]);
		softmax_prob /= sum;

		double confidence;
		cv::Point class_id_point;
		minMaxLoc(softmax_prob.reshape(1, 1), nullptr, &confidence, nullptr, &class_id_point);
		int label_id = class_id_point.x;

		AT.confidence = confidence;
		AT.color = class_names_[label_id];*/


		/* 传统定位打击中心方法，现采用深度学习方法评估
		//首先拓宽边界防止打击点运算过程中偏移，再进行强闭运算和开运算消除打击位置以外轮廓
		Mat border_img,Open_img, Close_img;
		int extend = 20;
		copyMakeBorder(target_img, border_img, extend, extend, extend, extend, BORDER_CONSTANT, Scalar(0, 0, 0));
		Mat kernel = getStructuringElement(MORPH_CROSS, Size(15, 15));
		morphologyEx(border_img, Open_img, MORPH_CLOSE, kernel, Point(-1, -1), 2);
		morphologyEx(Open_img, Close_img, MORPH_OPEN, kernel, Point(-1, -1), 2);

		//寻找打击中心
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;
		findContours(Close_img, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		auto it_1 = contours.begin();auto it_2 = hierarchy.begin();
		while (it_1 != contours.end() && it_2 != hierarchy.end()) {
			if ((*(it_2))[2] == -1) {
				auto rect = minAreaRect(*it_1);

				//计算轮廓中心点在进行边界拓宽前的坐标
				Point2f Target_Point_UNcorrected = Point2f(rect.center.x - extend, rect.center.y - extend);
				break;
			}
			it_1++;
			it_2++;

		}
		*/
	}
}


const cv::Mat Detector::drawResult(const int& Type) {
	cv::Mat result_img;
	src_img.copyTo(result_img);
	cv::Point2f center;
	float radius;
	std::vector<Light> L;
	if (Type == 0) L = Circle_L;
	else if (Type == 1)L = Big_L;
	else if (Type == 2)L = Small_L;
	else if (Type == 3)L = Huge_L;
	else if (Type == 4 && findAT) {
		std::vector<cv::Point2f> rect_points;
		auto rect = cv::minAreaRect(AT.contour);
		rect.points(rect_points);
		for (int i = 0;i < rect_points.size();i++) {
			cv::line(result_img, rect_points[i], rect_points[(i + 1) % rect_points.size()], cv::Scalar(255, 255, 255),3);
		}

		//cv::ellipse(result_img, *Attack_Target, color, thick);

		//以下代码用于确定拟合矩形的边点
		/*for (int i = 0;i < 4;i++) {
			cv::circle(result_img, Attack_Target->point[i], 2, cv::Scalar(255, 255, 255), 2);
			cv::putText(result_img, std::to_string(i), Attack_Target->point[i], 1, 2, cv::Scalar(255, 255, 255), 3);
		}
		if (Attack_Target->length>Attack_Target->width) {
			cv::putText(result_img, cv::format("yes"), cv::Point(100, 100), 1, 2, cv::Scalar(255, 0, 0), 3);
		}
		else {
			cv::putText(result_img, cv::format("no"), cv::Point(100, 100), 1, 2, cv::Scalar(255, 0, 0), 3);
		}*/
		return result_img;
	}
	else if (Type == 5 && findR) {
		cv::minEnclosingCircle(RT.contour, center, radius);
		
	}
	else if (Type == 6 ) {
		if (findR) {
			cv::minEnclosingCircle(RT.contour, center, radius);
			cv::circle(result_img, center, radius * 1.5, cv::Scalar(0, 255, 0), 3);
		}
		if (findAT) {
			std::vector<cv::Point2f> rect_points;
			auto rect = cv::minAreaRect(AT.contour);
			rect.points(rect_points);
			for (int i = 0;i < rect_points.size();i++) {
				cv::line(result_img, rect_points[i], rect_points[(i + 1) % rect_points.size()], cv::Scalar(255, 255, 255), 3);
			}
			
		}
		return result_img;
	}
	else return result_img;
	for (auto light = L.begin();light != L.end();light++) {
		//cv::minEnclosingCircle(light->contour, center, radius);
		//cv::circle(result_img, center, static_cast<int>(radius), color, thick);
		std::vector<cv::Point2f> rect_points;
		auto rect = cv::minAreaRect(light->contour);
		rect.points(rect_points);
		for (int i = 0;i < rect_points.size();i++) {
			cv::line(result_img, rect_points[i], rect_points[(i + 1) % rect_points.size()], cv::Scalar(255, 255, 255), 3);
			cv::putText(result_img, cv::format("Angle:%f",light->angle), light->center, 1, 2, cv::Scalar(255, 0, 0), 2);
			cv::putText(result_img, cv::format("Ratio:%f", light->ratio), cv::Point2f(light->center.x, light->center.y+50), 1, 2, cv::Scalar(255, 0, 0), 2);
		}
	}

	return result_img;
}

cv::Mat Detector::GetPreprocess_Img() {
	return preprocess_img;
}