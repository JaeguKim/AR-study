#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2\opencv.hpp>
#include <fstream>

using namespace cv;

using namespace std;

typedef struct Coordinate {
	int x, y;
} Coor;

cv::Mat src;
cv::Mat dst;
cv::Mat tempImage; //사각형을 그려서 보여줄 mattrix
Coor left_upper, right_down, center;
bool drawing_box;

void rotate(int event, int x, int y, int flag, void * param)//1.5X 확대
{
	if (event == CV_EVENT_MOUSEMOVE)
	{
		if (drawing_box) {
			right_down.x = x;
			right_down.y = y;
			src.copyTo(tempImage); //tempImage 다시초기화
			cv::rectangle(tempImage, cvPoint(left_upper.x, left_upper.y), cvPoint(right_down.x, right_down.y), cv::Scalar(0, 0, 0));
			imshow("SrcImage", tempImage);
		}
	}
	else if (event == CV_EVENT_LBUTTONDOWN)
	{
		drawing_box = true;
		//좌 상단 정보 저장
		left_upper.x = x;
		left_upper.y = y;
		cout << left_upper.x << " " << left_upper.y << endl;
	}
	else if (event == CV_EVENT_LBUTTONUP)
	{
		Coor new_lu, new_rd; //new lower right, new right down
		cv::Mat rotated_img;
		cv::Mat flip_img;
		drawing_box = false;

		cv::Mat temp_img(src, cv::Range(left_upper.y, right_down.y), cv::Range(left_upper.x, right_down.x));//회전할 이미지
		temp_img.copyTo(flip_img);
		for (int i = 0; i < temp_img.rows; i++)
		{
			for (int j = 0; j < temp_img.cols; j++)
			{
				flip_img.at<cv::Vec3b>(i, j) = temp_img.at<cv::Vec3b>((temp_img.rows - 1) - i, j);
			}
		}
		Mat matRotation = cv::getRotationMatrix2D(Point(temp_img.cols/2, temp_img.rows/2),90, 1); //90도 만큼회전
		cv::transpose(flip_img, rotated_img);

		imshow("Rotated Image", flip_img);

		center.x = (left_upper.x + right_down.x) / 2;
		center.y = (left_upper.y + right_down.y) / 2;
		new_lu.x = center.x - rotated_img.cols / 2;
		new_rd.x = center.x + rotated_img.cols / 2;
		new_lu.y = center.y - rotated_img.rows / 2;
		new_rd.y = center.y + rotated_img.rows / 2;

		src.copyTo(dst);	
		for (int i = new_lu.x; i < new_rd.x; i++)
		{
			for (int j = new_lu.y; j < new_rd.y; j++)
			{
				//cout << j - new_lu.y << " " << i - new_lu.x << endl;
				dst.at<cv::Vec3b>(j, i) = rotated_img.at<cv::Vec3b>(j - new_lu.y, i - new_lu.x);
			}
		}

		imshow("Rotated Image", dst);
	}
}

int main(int argc, char ** argv)
{
	string command; // menu 선택 
	src = imread("usecase diagram.png", 1); //0은 gray scale,1은 color scale
	dst = cv::Mat::zeros(src.size(), src.type());

	cv::namedWindow("SrcImage");
	cv::imshow("SrcImage", src);
	cv::setMouseCallback("SrcImage", rotate, 0);
	cv::namedWindow("Rotated Image");
	cv::waitKey(0); //이미지를 띠울시간을 정해줌
	return 0;
}

