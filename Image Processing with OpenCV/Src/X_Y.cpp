#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2\opencv.hpp>
#include <fstream>

using namespace cv;

using namespace std;

cv::Mat gMatImage;
cv::Mat gMatFlipImage;
cv::Mat tempImage; //�簢���� �׷��� ������ mattrix

typedef struct Coordinate {
	int x, y;
} Coor;

Coor left_upper, right_down;
bool drawing_box;

void reflect_X(int event, int x, int y, int flag, void * param) //X������ ������Ű��
{
	if (event == CV_EVENT_MOUSEMOVE)
	{
		if (drawing_box) {
			right_down.x = x;
			right_down.y = y;
			gMatImage.copyTo(tempImage); //tempImage �ٽ��ʱ�ȭ
			cv::rectangle(tempImage, cvPoint(left_upper.x, left_upper.y), cvPoint(right_down.x, right_down.y), cv::Scalar(0, 0, 0));
			imshow("Image", tempImage);
		}
	}
	else if (event == CV_EVENT_LBUTTONDOWN)
	{
		drawing_box = true;
		//�� ��� ���� ����
		left_upper.x = x;
		left_upper.y = y;
		cout << x << " " << y << endl;
	}
	else if (event == CV_EVENT_LBUTTONUP)
	{
		drawing_box = false;
		//�������� �簢�� �ٿ���� ������ �ٲٱ�
		gMatImage.copyTo(gMatFlipImage);
		for (int i = left_upper.y; i < right_down.y; i++)
		{
			for (int j = left_upper.x; j < right_down.x; j++)
			{
				gMatFlipImage.at<cv::Vec3b>(i, j) = gMatImage.at<cv::Vec3b>((right_down.y - 1) - i+left_upper.y, j);
			}
		}
		cv::imshow("FlipImage", gMatFlipImage);
		gMatFlipImage.copyTo(gMatImage);
		//right_down.x = x;
		//right_down.y = y;

		//cv::rectangle(tempImage, cvPoint(left_upper.x, left_upper.y), cvPoint(right_down.x, right_down.y), cv::Scalar(0, 0, 0));
		//imshow("Image", tempImage);
		////�������� �簢�� �ٿ���� ������ �ٲٱ�
		//gMatImage.copyTo(gMatFlipImage);
		//for (int i = left_upper.y; i < right_down.y; i++)
		//{
		//	for (int j = left_upper.x; j < right_down.x; j++)
		//	{
		//		gMatFlipImage.at<cv::Vec3b>(j, i) = gMatImage.at<cv::Vec3b>(j, (right_down.y - 1) - i);
		//	}
		//}

		//cv::imshow("FlipImage", gMatFlipImage);
		//gMatFlipImage.copyTo(gMatImage);
	}
}

void reflect_Y(int event, int x, int y, int flag, void * param)//Y������ ������Ű��
{
	if (event == CV_EVENT_MOUSEMOVE)
	{
		if (drawing_box) {
			right_down.x = x;
			right_down.y = y;
			gMatImage.copyTo(tempImage); //tempImage �ٽ��ʱ�ȭ
			cv::rectangle(tempImage, cvPoint(left_upper.x, left_upper.y), cvPoint(right_down.x, right_down.y), cv::Scalar(0, 0, 0));
			imshow("Image", tempImage);
		}
	}
	else if (event == CV_EVENT_LBUTTONDOWN)
	{
		drawing_box = true;
		//�� ��� ���� ����
		left_upper.x = x;
		left_upper.y = y;
		cout << x << " " << y << endl;
	}
	else if (event == CV_EVENT_LBUTTONUP)
	{
		drawing_box = false;
		//cv::rectangle(tempImage, cvPoint(left_upper.x, left_upper.y), cvPoint(right_down.x, right_down.y), cv::Scalar(0, 0, 0));
		//imshow("Image", tempImage);
		//�������� �簢�� �ٿ���� ������ �ٲٱ�
		gMatImage.copyTo(gMatFlipImage);
		for (int i = left_upper.x; i < right_down.x; i++)
		{
			for (int j = left_upper.y; j < right_down.y; j++)
			{
				gMatFlipImage.at<cv::Vec3b>(j, i) = gMatImage.at<cv::Vec3b>(j, (right_down.x - 1) - i+left_upper.x);
			}
		}

		cv::imshow("FlipImage", gMatFlipImage);
		gMatFlipImage.copyTo(gMatImage);
	}
}

int main(int argc, char ** argv) {
	string command; // menu ���� 
	gMatImage = imread("usecase diagram.png", 1); //0�� gray scale,1�� color scale
	gMatFlipImage = cv::Mat::zeros(gMatImage.size(), gMatImage.type());

	//for (int i = 0; i < gMatImage.cols; i++)
	//{
	//	for (int j = 0; j < gMatImage.rows; j++)
	//		gMatFlipImage.at<cv::Vec3b>(j, i) = gMatImage.at<cv::Vec3b>(j, (gMatImage.cols -1) - i);
	//}

	cv::namedWindow("Image");
	cv::imshow("Image", gMatImage);

	cv::namedWindow("FlipImage");

	//cv::createTrackbar("aaa", "Image",gnMinCr,gnMaxCr,on_trackbar1); 
	while (1)
	{
		cin >> command;
		if (command == "x")
		{
			cv::setMouseCallback("Image", reflect_X, 0);
			cv::waitKey(0); //�̹����� ���ð��� ������
		}
		else if (command == "y")
		{
			cv::setMouseCallback("Image", reflect_Y, 0);
			cv::waitKey(0); //�̹����� ���ð��� ������
		}
	}
	
	return 0;
}

