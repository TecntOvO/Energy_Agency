#include "Detector.hpp"

void main() {
	Detector det("Energy_params.xml");

	cv::namedWindow("OutPut", cv::WINDOW_NORMAL);
	cv::namedWindow("OutPut_Aft", cv::WINDOW_NORMAL);
	//cv::namedWindow("Adjust OutPut", cv::WINDOW_NORMAL);
	cv::namedWindow("Preprocess OutPut", cv::WINDOW_NORMAL);
	//cv::namedWindow("Preprocess src OutPut", cv::WINDOW_NORMAL);
	cv::namedWindow("Control", cv::WINDOW_NORMAL);
	cv::namedWindow("Target", cv::WINDOW_NORMAL);

	cv::createTrackbar("bin_thres", "Control", &det.settings.Binary_Value, 255);
	cv::createTrackbar("Sin", "Control", &det.settings.red_Sin, 255);

	cv::createTrackbar("LowH", "Control", &det.settings.red_iLowH, 255); // Hue (0 - 179)
	cv::createTrackbar("HighH", "Control", &det.settings.red_iHighH, 255);

	cv::createTrackbar("LowS", "Control", &det.settings.red_iLowS, 255); // Saturation (0 - 255)
	cv::createTrackbar("HighS", "Control", &det.settings.red_iHighS, 255);

	cv::createTrackbar("LowV", "Control", &det.settings.red_iLowV, 255); // Value (0 - 255)
	cv::createTrackbar("HighV", "Control", &det.settings.red_iHighV, 255);
	

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
		det.detect(frame,RED);
		cv::imshow("Preprocess OutPut", det.GetPreprocess_Img());
		cv::imshow("OutPut",det.drawResult(4, cv::Scalar(255, 255, 255), 7));
		cv::imshow("OutPut_Aft", det.drawResult(5, cv::Scalar(255, 255, 255), 7));
		//cv::imshow("Adjust OutPut", det.adjust_img);
		//cv::imshow("Preprocess src OutPut", det.preprocess_src_img);
		if(!det.target_img.empty())	cv::imshow("Target", det.target_img);
		cv::waitKey(1000 / (int)video.get(cv::CAP_PROP_FPS));
	}
}