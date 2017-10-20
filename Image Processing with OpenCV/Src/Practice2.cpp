#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2\opencv.hpp>

using namespace cv;

using namespace std;

int main(int argc, char ** argv) {

	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
		return -1;
	Mat g_RGB;
	Mat g_YCrCb;
	Mat channels[3];
	Mat g_Gray;
	Mat g_Bgr;
	Mat result;

	int thresholdVal = 128;
	cvNamedWindow("src",0);
	cvNamedWindow("Y", 0);
	cvNamedWindow("Cr", 0);
	cvNamedWindow("Cb", 0);
	cvCreateTrackbar("T", "src", &thresholdVal, 255, NULL);
	
	cv::Mat temp;

	for (;;)
	{
		cv::Mat frame;
		cap >> frame; // get a new frame from camera
		cv::cvtColor(frame, g_YCrCb, CV_RGB2YCrCb);
		cv::split(g_YCrCb, channels);
		
		imshow("src", frame );
		//imshow("RGB", g_RGB);

		/*threshold(channels[1], channels[1], 77, 150, 3);
		threshold(channels[2], channels[2], 133, 173, 3);*/
		imshow("Y", channels[0]);
		cv::inRange(channels[1], cv::Scalar(77), cv::Scalar(150), channels[1]);
		cv::inRange(channels[2], cv::Scalar(133), cv::Scalar(173), channels[2]);
		imshow("Cr", channels[1]);
		imshow("Cb", channels[2]);
		cv::bitwise_and(channels[1], channels[2], g_Gray);
		imshow("Gray", g_Gray);
		cv::cvtColor(g_Gray, g_Bgr, CV_GRAY2BGR); //COLOR_GRAY2BGRA·Î ÇÏ¸é ¾ÈµÊ
		cv::bitwise_and(g_Bgr, frame, result);
		imshow("result", result);
		if (waitKey(30) >= 0) break;
	}
	
	return 0;
}

