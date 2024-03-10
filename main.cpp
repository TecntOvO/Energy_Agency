#include "Detector.hpp"

void main() {
	int value = 70,sin_value=30; //100 red 68 blue
	int alpha=100, beta=100; //46 blue
	cv::namedWindow("OutPut", cv::WINDOW_NORMAL);
	cv::namedWindow("OutPut_Aft", cv::WINDOW_NORMAL);
	cv::namedWindow("Binary OutPut", cv::WINDOW_NORMAL);
	cv::namedWindow("Adjust OutPut", cv::WINDOW_NORMAL);
	cv::namedWindow("Control", cv::WINDOW_NORMAL);
	//cv::namedWindow("Target", cv::WINDOW_NORMAL);
	cv::createTrackbar("bin_thres", "Control", &value, 255);
	cv::createTrackbar("alpha", "Control", &alpha, 200);
	cv::createTrackbar("beta", "Control", &beta, 200);
	cv::createTrackbar("Sin", "Control", &sin_value, 255);
	cv::createTrackbar("LowH", "Control", &settings.red_iLowH, 255); // Hue (0 - 179)
	cv::createTrackbar("HighH", "Control", &settings.red_iHighH, 255);

	cv::createTrackbar("LowS", "Control", &settings.red_iLowS, 255); // Saturation (0 - 255)
	cv::createTrackbar("HighS", "Control", &settings.red_iHighS, 255);

	cv::createTrackbar("LowV", "Control", &settings.red_iLowV, 255); // Value (0 - 255)
	cv::createTrackbar("HighV", "Control", &settings.red_iHighV, 255);
	Detector::LightParams myR = {1.1,200,1.35,1.15,2.9,2.5,2.05,1.6,30000,9000,1.8};	//灯光筛选相关数据
	Detector det(value, 1, myR);

	/*cv::Mat img = cv::imread("ori.png");
	det.detect(img);
	det.drawResult(4, cv::Scalar(255, 255, 255), 2);
	cv::imshow("Binary OutPut", det.GetBinaryImg());
	cv::imshow("OutPut", det.result_img);
	cv::waitKey();*/
	
	cv::VideoCapture video("../PowerMachine.mp4");
	
	if (!video.isOpened()) exit(0);
	cv::Mat frame;
	for (;;) {
		video >> frame;
		if (frame.empty()) break;
		cv::convertScaleAbs(frame, frame, alpha/100.0, beta/100.0);
		det.UpdateParams(value, 1, myR);
		det.detect(frame, sin_value);
		cv::imshow("Binary OutPut", det.GetBinaryImg());
		cv::imshow("OutPut",det.drawResult(4, cv::Scalar(255, 255, 255), 7));
		cv::imshow("OutPut_Aft", det.drawResult(5, cv::Scalar(255, 255, 255), 7));
		cv::imshow("Adjust OutPut", det.adjust_img);
		//if(!det.target_img.empty())	cv::imshow("Target", det.target_img);
		cv::waitKey(1000 / (int)video.get(cv::CAP_PROP_FPS));
	}
}