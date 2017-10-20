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
cv::Mat tempImage; //�簢���� �׷��� ������ mattrix
Coor left_upper, right_down, center;
bool drawing_box;

void scale(int event, int x, int y, int flag, void * param)//1.5X Ȯ��
{
	if (event == CV_EVENT_MOUSEMOVE)
	{
		if (drawing_box) {
			right_down.x = x;
			right_down.y = y;
			src.copyTo(tempImage); //tempImage �ٽ��ʱ�ȭ
			cv::rectangle(tempImage, cvPoint(left_upper.x, left_upper.y), cvPoint(right_down.x, right_down.y), cv::Scalar(0, 0, 0));
			imshow("SrcImage", tempImage);
		}
	}
	else if (event == CV_EVENT_LBUTTONDOWN)
	{
		drawing_box = true;
		//�� ��� ���� ����
		left_upper.x = x;
		left_upper.y = y;
		cout << left_upper.x << " " << left_upper.y << endl;
	}
	else if (event == CV_EVENT_LBUTTONUP)
	{
		Coor new_lu, new_rd; //new lower right, new right down
		cv::Mat scaled_img; //Ȯ��� �̹���
		cv::Mat temp = cv::Mat::zeros(src.size(), src.type());;
		drawing_box = false;

		cv::Mat temp_img(src,cv::Range(left_upper.y, right_down.y), cv::Range(left_upper.x, right_down.x));//Ȯ���� �̹���

		cv::resize(temp_img, scaled_img, cv::Size(temp_img.cols*1.5, temp_img.rows*1.5), 0, 0, cv::INTER_LINEAR);//1.5�� Ȯ���Ͽ� ����																							 //	src.copyTo(dst);

		center.x = (left_upper.x + right_down.x)/2;
		center.y = (left_upper.y + right_down.y)/2;
		new_lu.x = center.x - scaled_img.cols/2;
		new_rd.x = center.x + scaled_img.cols / 2;
		new_lu.y = center.y - scaled_img.rows / 2;
		new_rd.y = center.y + scaled_img.rows/2;

		cout << x << " " << y << endl;
		cout << new_lu.x << " " << new_lu.y << endl;
		cout << new_rd.x << " " << new_rd.y << endl;
	
		cout << scaled_img.rows << " " << scaled_img.cols << endl;
		src.copyTo(dst);
		imshow("Scaled Image", dst);
		for (int i = new_lu.x; i < new_rd.x; i++)
		{
			for (int j = new_lu.y; j < new_rd.y; j++)
			{
				/*cout << j - new_lu.y << " " << i - new_lu.x << endl;*/
				dst.at<cv::Vec3b>(j, i) = scaled_img.at<cv::Vec3b>(j-new_lu.y, i-new_lu.x);
			}
		}

		imshow("Scaled Image" ,dst);
		
	}
}

int main(int argc, char ** argv) 
{
	string command; // menu ���� 
	src = imread("usecase diagram.png", 1); //0�� gray scale,1�� color scale
	dst = cv::Mat::zeros(src.size(), src.type());

	cv::namedWindow("SrcImage");
	cv::imshow("SrcImage", src);
	cv::setMouseCallback("SrcImage", scale, 0);
	cv::namedWindow("Scaled Image");
	cv::waitKey(0); //�̹����� ���ð��� ������
	return 0;
}

