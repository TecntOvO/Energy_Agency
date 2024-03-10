#include "Detector.hpp"

void main() {
	int value = 70; //100 red 68 blue
	int alpha=100, beta=100; //46 blue
	cv::namedWindow("OutPut", cv::WINDOW_NORMAL);
	cv::namedWindow("OutPut_Aft", cv::WINDOW_NORMAL);
	cv::namedWindow("Binary OutPut", cv::WINDOW_NORMAL);
	//cv::namedWindow("Target", cv::WINDOW_NORMAL);
	cv::createTrackbar("bin_thres", "Binary OutPut", &value, 255);
	cv::createTrackbar("alpha", "Binary OutPut", &alpha, 200);
	cv::createTrackbar("beta", "Binary OutPut", &beta, 200);
	Detector::LightParams myR = {1.1,200,1.35,1.15,2.9,2.5,2.05,1.6,30000,9000,1.8};	//�ƹ�ɸѡ�������
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
		det.detect(frame);
		cv::imshow("Binary OutPut", det.GetBinaryImg());
		cv::imshow("OutPut",det.drawResult(4, cv::Scalar(255, 255, 255), 7));
		cv::imshow("OutPut_Aft", det.drawResult(5, cv::Scalar(255, 255, 255), 7));
		//if(!det.target_img.empty())	cv::imshow("Target", det.target_img);
		cv::waitKey(1000 / (int)video.get(cv::CAP_PROP_FPS));
	}
}