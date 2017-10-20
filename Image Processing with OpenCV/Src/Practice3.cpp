#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2\opencv.hpp>

using namespace cv;

using namespace std;

void mouse_handle(int event, int x, int y, int flag, void * param) //X축으로 반전시키기
{
	if (event == CV_EVENT_MOUSEMOVE)
	{
		cout << x << " " << y << endl;
	}
	
}
int main(int argc, char ** argv) {

	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
		return -1;
	Mat g_YCrCb;
	Mat g_RGB;
	Mat channels[3];
	Mat g_Gray;
	Mat g_Bgr;
	Mat temp;
	Mat result;


	int thresholdVal = 128;
	cvNamedWindow("src", 0);
	cvCreateTrackbar("T", "src", &thresholdVal, 255, NULL);
	cv::setMouseCallback("src", mouse_handle, 0);

	
	
	for (;;)
	{
		cv::Mat frame;
		cap >> frame; // get a new frame from camera
		//cvtColor(frame, g_RGB, CV_RGBA2RGB);
		//cvtColor(g_RGB, g_YCrCb, CV_RGB2YCrCb);
		cvtColor(frame, g_YCrCb, CV_RGB2YCrCb);

		split(g_YCrCb, channels);

		imshow("src", frame); //원본영상 띄움

		cv::inRange(channels[1], cv::Scalar(77), cv::Scalar(150), channels[1]);
		cv::inRange(channels[2], cv::Scalar(133), cv::Scalar(173), channels[2]);
		bitwise_and(channels[1], channels[2], g_Gray);
		cvtColor(g_Gray, g_Bgr, CV_GRAY2BGR); //COLOR_GRAY2BGRA로 하면 안됨

		int change_cnt = 0; //색변화횟수를 저장

		g_Bgr.copyTo(temp);
		cv::rectangle(temp, cv::Point(203, 342), cv::Point(406, 462), cv::Scalar(255, 0, 0));
		cv::line(temp, cv::Point(203,(342+462)/2), cv::Point(406,(342 + 462) / 2), cv::Scalar(255, 0, 0));
		
		imshow("result",temp);
		//imshow("result", g_Bgr);

		int line_y = (342 + 462) / 2;

		for (int x = 203; x<406; x++)
		{
			// get pixel
			Vec3b color = g_Bgr.at<Vec3b>(Point(x, line_y));
			Vec3b color2 = g_Bgr.at<Vec3b>(Point(x + 1, line_y));
			if (color.val[0] == color2.val[0] && color.val[1] == color2.val[1] && color.val[2] == color2.val[2]) //색깔변화가 없을경우
			{

			}
			else //색깔변화 있을경우
			{
				change_cnt++;
			}
		}

		//cout << "change_cnt : " << change_cnt << endl;
		if (change_cnt == 2)
			cout << "Rock" << endl;
		else if (change_cnt == 4)
			cout << "Sissors" << endl;
		else if (change_cnt == 10)
			cout << "Paper" << endl;
		else
			cout << "Unknown" << endl;
		if (waitKey(30) >= 0) break;
	
	}

	return 0;
}

