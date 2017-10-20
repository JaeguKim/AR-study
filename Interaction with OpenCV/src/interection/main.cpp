#include <iostream>
#include <fstream>
#include <Windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <thread>
#include ".\include\GL\freeglut.h"
#include "opencv2\opencv.hpp"
#include "opencv2\core\ocl.hpp"
#include <math.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2\opencv.hpp>

using namespace cv;

#pragma comment(lib, "opencv_world310.lib")

#define	FRAME_WIDTH		640 //1080
#define	FRAME_HEIGHT	480 //

///< unit: cm
#define	MARKER_HEIGHT	7
#define MARKER_WIDTH	7

#define	zNear	1.0
#define	zFar	1000000

cv::Mat gMarkerImg1;	///< ��Ŀ �̹���1
cv::Mat gMarkerImg2;	///< ��Ŀ �̹���2
cv::Mat gSceneImg;	///< ī�޶�� ĸ���� �̹���
cv::Mat	gOpenGLImg;	///< OpenGL�� �������� �̹���
cv::VideoCapture gVideoCapture;	///< ī�޶� ĸ�� ��ü

cv::Ptr<cv::ORB> detector1;	///< ORB Ư¡�� �����1
cv::Ptr<cv::ORB> detector2;	///< ORB Ư¡�� �����2
cv::Ptr<cv::DescriptorMatcher> matcher1;	///< ORB Ư¡���� ��Ī ��ü1
cv::Ptr<cv::DescriptorMatcher> matcher2;	///< ORB Ư¡���� ��Ī ��ü2

											///< ��Ŀ �� ī�޶�� ĸ���� �̹����� ORB Ư¡����(keypoints)
std::vector<cv::KeyPoint> gvMarkerKeypoints1, gvSceneKeypoints1;
std::vector<cv::KeyPoint> gvMarkerKeypoints2, gvSceneKeypoints2;

///< ��Ŀ �� ī�޶�� ĸ���� �̹����� ORB Ư¡����(descriprtors)
cv::Mat gMarkerDescriptors1, gSceneDescriptors1;
cv::Mat gMarkerDescriptors2, gSceneDescriptors2;

cv::Mat E1;	///< ��Ŀ ��ǥ�迡�� ī�޶� ��ǥ����� ��ȯ ���
cv::Mat E2;

cv::Mat K;	///< ī�޶� ���� �Ķ����

void init(int marker_no)
{
	///< ��Ŀ���� ī�޶���� ��ȯ ����� �ʱ�ȭ �Ѵ�.
	E1 = cv::Mat::eye(4, 4, CV_64FC1);
	E2 = cv::Mat::eye(4, 4, CV_64FC1);
	K = cv::Mat::eye(3, 3, CV_64FC1);

	///< ī�޶� ���� �Ķ���� �ʱ�ȭ
	K.at<double>(0, 0) = 689.958461;
	K.at<double>(1, 1) = 698.647064;
	K.at<double>(0, 2) = 329.759390;
	K.at<double>(1, 2) = 230.330858;

	if (marker_no == 1)
	{
		///< ��Ŀ �̹����� �д´�.
		gMarkerImg1 = cv::imread("marker1.jpg", 0);

		///< ī�޶� �ʱ�ȭ
		
		if (!gMarkerImg1.data) {
			std::cerr << "�ʱ�ȭ�� ������ �� �����ϴ�." << std::endl;
			exit(-1);
		}

		///< Ư¡���� ������ ��Ī ��ü �ʱ�ȭ
		detector1 = cv::ORB::create();
		matcher1 = cv::DescriptorMatcher::create("BruteForce-Hamming");

		///< ��Ŀ ������ Ư¡���� ����
		detector1->detect(gMarkerImg1, gvMarkerKeypoints1);
		detector1->compute(gMarkerImg1, gvMarkerKeypoints1, gMarkerDescriptors1);

		///< ��Ŀ ������ ���� ũ�� ����
		for (int i = 0; i < (int)gvMarkerKeypoints1.size(); i++) {
			gvMarkerKeypoints1[i].pt.x /= gMarkerImg1.cols;
			gvMarkerKeypoints1[i].pt.y /= gMarkerImg1.rows;

			gvMarkerKeypoints1[i].pt.x -= 0.5;
			gvMarkerKeypoints1[i].pt.y -= 0.5;

			gvMarkerKeypoints1[i].pt.x *= MARKER_WIDTH;
			gvMarkerKeypoints1[i].pt.y *= MARKER_HEIGHT;
		}
	}
	else if (marker_no == 2)
	{
		///< ��Ŀ �̹����� �д´�.
		gMarkerImg2 = cv::imread("marker2.jpg", 0);

		///< ī�޶� �ʱ�ȭ

		if (!gMarkerImg2.data) {
			std::cerr << "�ʱ�ȭ�� ������ �� �����ϴ�." << std::endl;
			exit(-1);
		}

		///< Ư¡���� ������ ��Ī ��ü �ʱ�ȭ
		detector2 = cv::ORB::create();
		matcher2 = cv::DescriptorMatcher::create("BruteForce-Hamming");

		///< ��Ŀ ������ Ư¡���� ����
		detector2->detect(gMarkerImg2, gvMarkerKeypoints2);
		detector2->compute(gMarkerImg2, gvMarkerKeypoints2, gMarkerDescriptors2);

		///< ��Ŀ ������ ���� ũ�� ����
		for (int i = 0; i < (int)gvMarkerKeypoints2.size(); i++) {
			gvMarkerKeypoints2[i].pt.x /= gMarkerImg2.cols;
			gvMarkerKeypoints2[i].pt.y /= gMarkerImg2.rows;

			gvMarkerKeypoints2[i].pt.x -= 0.5;
			gvMarkerKeypoints2[i].pt.y -= 0.5;

			gvMarkerKeypoints2[i].pt.x *= MARKER_WIDTH;
			gvMarkerKeypoints2[i].pt.y *= MARKER_HEIGHT;
		}
	}
	///< OpenGL �ʱ�ȭ
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

///< ī�޶� ���� �ĸ����Ϳ��� OpenGL ���� �Ķ���� ��ȯ
void	convertFromCaemraToOpenGLProjection(double* mGL)
{
	cv::Mat P = cv::Mat::zeros(4, 4, CV_64FC1);

	P.at<double>(0, 0) = 2 * K.at<double>(0, 0) / FRAME_WIDTH;
	P.at<double>(1, 0) = 0;
	P.at<double>(2, 0) = 0;
	P.at<double>(3, 0) = 0;

	P.at<double>(0, 1) = 0;
	P.at<double>(1, 1) = 2 * K.at<double>(1, 1) / FRAME_HEIGHT;
	P.at<double>(2, 1) = 0;
	P.at<double>(3, 1) = 0;

	P.at<double>(0, 2) = 1 - 2 * K.at<double>(0, 2) / FRAME_WIDTH;
	P.at<double>(1, 2) = -1 + (2 * K.at<double>(1, 2) + 2) / FRAME_HEIGHT;
	P.at<double>(2, 2) = (zNear + zFar) / (zNear - zFar);
	P.at<double>(3, 2) = -1;

	P.at<double>(0, 3) = 0;
	P.at<double>(1, 3) = 0;
	P.at<double>(2, 3) = 2 * zNear*zFar / (zNear - zFar);
	P.at<double>(3, 3) = 0;

	for (int ix = 0; ix < 4; ix++)
	{
		for (int iy = 0; iy < 4; iy++)
		{
			mGL[ix * 4 + iy] = P.at<double>(iy, ix);
		}
	}

}

///< ȣ��׷��Ƿκ��� ��Ŀ���� ī�޶���� ��ȯ ��� ����
bool	calculatePoseFromH(const cv::Mat& H, cv::Mat& R, cv::Mat& T)
{
	cv::Mat InvK = K.inv();
	cv::Mat InvH = InvK * H;
	cv::Mat h1 = H.col(0);
	cv::Mat h2 = H.col(1);
	cv::Mat h3 = H.col(2);

	double dbNormV1 = cv::norm(InvH.col(0));

	if (dbNormV1 != 0) {
		InvK /= dbNormV1;

		cv::Mat r1 = InvK * h1;
		cv::Mat r2 = InvK * h2;
		cv::Mat r3 = r1.cross(r2);

		T = InvK * h3;

		cv::Mat R1 = cv::Mat::zeros(3, 3, CV_64FC1);

		r1.copyTo(R1.rowRange(cv::Range::all()).col(0));
		r2.copyTo(R1.rowRange(cv::Range::all()).col(1));
		r3.copyTo(R1.rowRange(cv::Range::all()).col(2));

		cv::SVD svd(R1);

		R = svd.u * svd.vt;

		return true;
	}
	else
		return false;
}

cv::Mat T1, T2;
bool t1 = false;
bool t2 = false;

//���� ���� ���� ���� Ƚ���� ������ ����
int SissorsCnt = 0;
int RockCnt = 0;
int PaperCnt = 0;

///< ī�޶�κ��� ������ �а�, Ư¡���� ������ ��, ��Ŀ ������� ��Ī �� ȣ��׷��Ǹ� ����
///< ������ ȣ��׷��Ƿκ��� ��Ŀ���� ī�޶���� ��ȯ ��� ����
void processVideoCapture1(void)
{
	cv::Mat grayImg;

	///< ī�޶�κ��� ����ȹ��
	//gVideoCapture >> gSceneImg;

	if (gSceneImg.rows == 0 && gSceneImg.dims == 0) //ȭ���� �о���� �������� �Լ�����
		return;

	///< Ư¡���� ������ ���Ͽ� ��鿵������ ��ȯ
	cv::cvtColor(gSceneImg, grayImg, CV_BGR2GRAY);

	///< ī�޶�κ��� ȹ���� ������ Ư¡���� ����
	detector1->detect(grayImg, gvSceneKeypoints1);
	detector1->compute(grayImg, gvSceneKeypoints1, gSceneDescriptors1);

	///< ��Ŀ Ư¡������ ��Ī ����
	std::vector<std::vector<cv::DMatch>> matches;
	matcher1->knnMatch(gMarkerDescriptors1, gSceneDescriptors1, matches, 2);

	std::vector<cv::DMatch> good_matches;
	for (int i = 0; i < (int)matches.size(); i++) {
		if (matches[i][0].distance < 0.9 * matches[i][1].distance) {
			good_matches.push_back(matches[i][0]);
		}
	}

	///< ��Ŀ Ư¡������ ����� �������� �ִ� ��쿡....
	if (good_matches.size() > 10) {
		std::vector<cv::Point2f> vMarkerPts;
		std::vector<cv::Point2f> vScenePts;

		///< ȣ��׷��� ����
		for (int i = 0; i < (int)good_matches.size(); i++) {
			vMarkerPts.push_back(gvMarkerKeypoints1[matches[i][0].queryIdx].pt);
			vScenePts.push_back(gvSceneKeypoints1[matches[i][0].trainIdx].pt);
		}

		cv::Mat H = cv::findHomography(vMarkerPts, vScenePts, CV_RANSAC);

		std::vector<cv::Point2f> obj_corners(4);
		obj_corners[0] = cv::Point(-MARKER_WIDTH / 2, -MARKER_HEIGHT / 2);
		obj_corners[1] = cv::Point(MARKER_WIDTH / 2, -MARKER_HEIGHT / 2);
		obj_corners[2] = cv::Point(MARKER_WIDTH / 2, MARKER_HEIGHT / 2);
		obj_corners[3] = cv::Point(-MARKER_WIDTH / 2, MARKER_HEIGHT / 2);

		std::vector<cv::Point2f> scene_corners(4);

		cv::perspectiveTransform(obj_corners, scene_corners, H);

		cv::line(gSceneImg, scene_corners[0], scene_corners[1], cv::Scalar(0, 255, 0), 2);
		cv::line(gSceneImg, scene_corners[1], scene_corners[2], cv::Scalar(0, 255, 0), 2);
		cv::line(gSceneImg, scene_corners[2], scene_corners[3], cv::Scalar(0, 255, 0), 2);
		cv::line(gSceneImg, scene_corners[3], scene_corners[0], cv::Scalar(0, 255, 0), 2);

		cv::Mat R, T;

		///< ȣ��׷��Ƿκ��� ��Ŀ���� ī�޶� ��ǥ���� ��ȯ ��� ����
		if (calculatePoseFromH(H, R, T)) {
			R.copyTo(E1.rowRange(0, 3).colRange(0, 3));
			T.copyTo(E1.rowRange(0, 3).col(3));

			static double changeCoordArray[4][4] = { { 1, 0, 0, 0 },{ 0, -1, 0, 0 },{ 0, 0, -1, 0 },{ 0, 0, 0, 1 } };
			static cv::Mat changeCoord(4, 4, CV_64FC1, changeCoordArray);

			E1 = changeCoord * E1;
			
			//gMatFlipImage.at<cv::Vec3b>(i, j)

			
		}
	}


	if (gSceneImg.data)
		cv::flip(gSceneImg, gOpenGLImg, 0);

	glutPostRedisplay();
}

void processVideoCapture2(void)
{
	cv::Mat grayImg;

	///< ī�޶�κ��� ����ȹ��
	//gVideoCapture >> gSceneImg;

	if (gSceneImg.rows == 0 && gSceneImg.dims == 0) //ȭ���� �о���� �������� �Լ�����
		return;

	///< Ư¡���� ������ ���Ͽ� ��鿵������ ��ȯ
	cv::cvtColor(gSceneImg, grayImg, CV_BGR2GRAY);

	///< ī�޶�κ��� ȹ���� ������ Ư¡���� ����
	detector2->detect(grayImg, gvSceneKeypoints2);
	detector2->compute(grayImg, gvSceneKeypoints2, gSceneDescriptors2);

	///< ��Ŀ Ư¡������ ��Ī ����
	std::vector<std::vector<cv::DMatch>> matches;
	matcher2->knnMatch(gMarkerDescriptors2, gSceneDescriptors2, matches, 2);

	std::vector<cv::DMatch> good_matches;
	for (int i = 0; i < (int)matches.size(); i++) {
		if (matches[i][0].distance < 0.9 * matches[i][1].distance) {
			good_matches.push_back(matches[i][0]);
		}
	}

	///< ��Ŀ Ư¡������ ����� �������� �ִ� ��쿡....
	if (good_matches.size() > 10) {
		std::vector<cv::Point2f> vMarkerPts;
		std::vector<cv::Point2f> vScenePts;

		///< ȣ��׷��� ����
		for (int i = 0; i < (int)good_matches.size(); i++) {
			vMarkerPts.push_back(gvMarkerKeypoints2[matches[i][0].queryIdx].pt);
			vScenePts.push_back(gvSceneKeypoints2[matches[i][0].trainIdx].pt);
		}

		cv::Mat H = cv::findHomography(vMarkerPts, vScenePts, CV_RANSAC);

		std::vector<cv::Point2f> obj_corners(4);
		obj_corners[0] = cv::Point(-MARKER_WIDTH / 2, -MARKER_HEIGHT / 2);
		obj_corners[1] = cv::Point(MARKER_WIDTH / 2, -MARKER_HEIGHT / 2);
		obj_corners[2] = cv::Point(MARKER_WIDTH / 2, MARKER_HEIGHT / 2);
		obj_corners[3] = cv::Point(-MARKER_WIDTH / 2, MARKER_HEIGHT / 2);

		std::vector<cv::Point2f> scene_corners(4);

		cv::perspectiveTransform(obj_corners, scene_corners, H);

		cv::line(gSceneImg, scene_corners[0], scene_corners[1], cv::Scalar(0, 255, 0), 2);
		cv::line(gSceneImg, scene_corners[1], scene_corners[2], cv::Scalar(0, 255, 0), 2);
		cv::line(gSceneImg, scene_corners[2], scene_corners[3], cv::Scalar(0, 255, 0), 2);
		cv::line(gSceneImg, scene_corners[3], scene_corners[0], cv::Scalar(0, 255, 0), 2);

		cv::Mat R, T;

		///< ȣ��׷��Ƿκ��� ��Ŀ���� ī�޶� ��ǥ���� ��ȯ ��� ����
		if (calculatePoseFromH(H, R, T)) {
			R.copyTo(E2.rowRange(0, 3).colRange(0, 3));
			T.copyTo(E2.rowRange(0, 3).col(3));

			static double changeCoordArray[4][4] = { { 1, 0, 0, 0 },{ 0, -1, 0, 0 },{ 0, 0, -1, 0 },{ 0, 0, 0, 1 } };
			static cv::Mat changeCoord(4, 4, CV_64FC1, changeCoordArray);

			E2 = changeCoord * E2;

			//gMatFlipImage.at<cv::Vec3b>(i, j)

		}
	}


	if (gSceneImg.data)
		cv::flip(gSceneImg, gOpenGLImg, 0);

	glutPostRedisplay();
}

//�������ú�����
float angle1 = 30.0f;
float angle2 = -30.0f;
bool isSetAngle1 = false; //������ ��ü�� ���� ������ �Ϸ�Ǿ����� true �ƴϸ� false
bool isSetAngle2 = false; //ť�갴ü�� ���� ������ �Ϸ�Ǿ����� true �ƴϸ� false
float radius = 5.0;

//������ú�����
char currentColor1 = 'r'; //��������� ������ ����
char currentColor2 = 'r';
bool isSetColor1 = false;
bool isSetColor2 = false;

//��������� ������
char currentShape1 = 's';
char currentShape2 = 's';
bool isSetShape1 = false;
bool isSetShape2 = false;

int change_cnt = 0; //����ȭȽ���� ���� - ���� : 4, ���� : 2, �� : 10

void displayThread1() {
	///< ��Ŀ�κ��� ī�޶���� ��ȯ����� ���� ��Ŀ��ǥ����� ��ȯ
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	cv::Mat temp1 = E1.t();

	glMultMatrixd((double *)temp1.data);
	//obj1.draw1("");
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f); glVertex3d(0.0, 0.0, 0.0); glVertex3d(10.0, 0.0, 0.0);
	glColor3f(0.0f, 1.0f, 0.0f); glVertex3d(0.0, 0.0, 0.0); glVertex3d(0.0, 10.0, 0.0);
	glColor3f(0.0f, 0.0f, 1.0f); glVertex3d(0.0, 0.0, 0.0); glVertex3d(0.0, 0.0, 10.0);
	glEnd();

	glRotated(-90.0, 1.0, 0.0, 0.0);
	glLineWidth(1.0f);

	if (angle1 >= 360.0f) //360������ ũ�ų� ������ 0���� reset
	{
		angle1 = 0.0f;
	}
	if (RockCnt >= 2) //������ 5���̻� �νĵǸ�
	{
			isSetAngle1 = true;
			glRotatef(angle1, 0.0f, 1.0f, 0.0f); //��Ŀ�� �߽����� ����
			glTranslated(radius, 0, 0);
			glRotatef(angle1, 0.0f, 1.0f, 0.0f); //����
			angle1 += 30.0f; //30�� ȸ��	

		if (isSetAngle1 && isSetAngle2) //�ΰ�ü�� ���� ������ ��� �����Ǿ�����
		{
			isSetAngle1 = false;
			isSetAngle2 = false;
			RockCnt = 0;//������ ���� Ƚ���� 0���� �ʱ�ȭ
		}
	}

	if (SissorsCnt >= 10) //������ 5���̻� �νĵǸ�
	{
		if (currentColor1 == 'r')
		{
			currentColor1 = 'g';
		}
		else if (currentColor1 == 'g')
		{
			currentColor1 = 'b';
		}
		else if (currentColor1 == 'b')
		{
			currentColor1 = 'r';
		}
		isSetColor1 = true;
		if (isSetColor1 && isSetColor2) //�ΰ�ü�� ���� ������ ��� �����Ǿ�����
		{
			isSetColor1 = false;
			isSetColor2 = false;
			SissorsCnt = 0;//������ ���� Ƚ���� 0���� �ʱ�ȭ
		}
	}

	//���� ���¿����� ������ ����
	if (currentColor1 == 'r')
	{
		glColor3f(1.0f, 0.0f, 0.0);
	}
	else if (currentColor1 == 'g')
	{
		glColor3f(0.0f, 1.0f, 0.0f);;
	}
	else if (currentColor1 == 'b')
	{
		glColor3f(0.0f, 0.0f, 1.0f);
	}
	
	///< ��Ŀ ��ǥ���� �߽ɿ��� ��ü ������

	if (PaperCnt >= 1) //���� 5���̻� �νĵǸ�
	{
		if (isSetShape1 == false)
		{
			if (currentShape1 == 's')
			{
				currentShape1 = 'w';
			}
			else if (currentShape1 == 'w')
			{
				currentShape1 = 's';
			}

			isSetShape1 = true;
		}

		if (isSetShape1 && isSetShape2) //�ΰ�ü�� ���� ����� ��� �����Ǿ�����
		{
			isSetShape1 = false;
			isSetShape2 = false;
			PaperCnt = 0;//���� ���� Ƚ���� 0���� �ʱ�ȭ
		}
	}

	//���� ���¿����� ��ü�� ����� ����
	if (currentShape1 == 's')
	{
		glutSolidTeapot(2.5);
	}
	else if (currentShape1 == 'w')
	{
		glutWireTeapot(2.5);
	}

}



void displayThread2() {
	///< ��Ŀ�κ��� ī�޶���� ��ȯ����� ���� ��Ŀ��ǥ����� ��ȯ
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	cv::Mat temp2 = E2.t();

	glMultMatrixd((double *)temp2.data);
	//obj2.draw2("");
	glLineWidth(3.0f);
	// Render a color-cube consisting of 6 quads with different colors
	//	glLoadIdentity();                 // Reset the model-view matrix
	//glTranslatef(1.5f, 0.0f, -7.0f);  // Move right and into the screen
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f); glVertex3d(0.0, 0.0, 0.0); glVertex3d(10.0, 0.0, 0.0);
	glColor3f(0.0f, 1.0f, 0.0f); glVertex3d(0.0, 0.0, 0.0); glVertex3d(0.0, 10.0, 0.0);
	glColor3f(0.0f, 0.0f, 1.0f); glVertex3d(0.0, 0.0, 0.0); glVertex3d(0.0, 0.0, 10.0);
	glEnd();

	glRotated(-90.0, 1.0, 0.0, 0.0);
	glLineWidth(4.0f);

	if (angle2 <= -360.0f)
	{
		angle2 = 0.0f;
	}

	if (RockCnt >= 2) //������ 5���̻� �νĵǸ�
	{
		//glTranslatef(1.5f, 0.0f, -7.0f);
			isSetAngle2 = true;
			glRotatef(angle2, 0.0f, 1.0f, 0.0f); //��Ŀ�� �߽����� ����
			glTranslated(radius, 0, 0);
			glRotatef(angle2, 0.0f, 1.0f, 0.0f); //����
			angle2 -= 30.0f; //-30�� ȸ��

		if (isSetAngle1 && isSetAngle2) //�� ��ü��� ������ ��� �����Ǿ�����
		{
			isSetAngle1 = false;
			isSetAngle2 = false;
			RockCnt = 0;
		}
	}
	if (SissorsCnt >= 10) //������ 5���̻� �νĵǸ�
	{
		if (currentColor2 == 'r')
		{
			currentColor2 = 'g';
		}
		else if (currentColor2 == 'g')
		{
			currentColor2 = 'b';
		}
		else if (currentColor2 == 'b')
		{
			currentColor2 = 'r';
		}
		isSetColor2 = true;
		if (isSetColor1 && isSetColor2) //�ΰ�ü�� ���� ������ ��� �����Ǿ�����
		{
			isSetColor1 = false;
			isSetColor2 = false;
			SissorsCnt = 0;//������ ���� Ƚ���� 0���� �ʱ�ȭ
		}
	}

	//���� ���¿����� ������ ����
	if (currentColor2 == 'r')
	{
		glColor3f(1.0f, 0.0f, 0.0);
	}
	else if (currentColor2 == 'g')
	{
		glColor3f(0.0f, 1.0f, 0.0f);;
	}
	else if (currentColor2 == 'b')
	{
		glColor3f(0.0f, 0.0f, 1.0f);
	}

	///< ��Ŀ ��ǥ���� �߽ɿ��� ��ü ������


	if (PaperCnt >= 1) //���� 5���̻� �νĵǸ�
	{
		if (isSetShape2 == false)
		{
			if (currentShape2 == 's')
			{
				currentShape2 = 'w';
			}
			else if (currentShape2 == 'w')
			{
				currentShape2 = 's';
			}

			isSetShape2 = true;
		}
		if (isSetShape1 && isSetShape2) //�ΰ�ü�� ���� ����� ��� �����Ǿ�����
		{
			isSetShape1 = false;
			isSetShape2 = false;
			PaperCnt = 0;//���� ���� Ƚ���� 0���� �ʱ�ȭ
		}
	}

	//���� ���¿����� ��ü�� ����� ����
	if (currentShape2 == 's')
	{
		glutSolidCube(2.5);
	}
	else if (currentShape2 == 'w')
	{
		glutWireCube(2.5);
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	///< ��� ���� ������
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDrawPixels(gOpenGLImg.cols, gOpenGLImg.rows, GL_BGR_EXT, GL_UNSIGNED_BYTE, (void *)gOpenGLImg.data);

	displayThread1();
	displayThread2();
	glutSwapBuffers();
}

void idle(void)
{
	std::thread t1(&processVideoCapture1);
	std::thread t2(&processVideoCapture2);
	t1.join();
	t2.join();
	
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	double P[16] = { 0 };

	convertFromCaemraToOpenGLProjection(P);

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(P);
	glMatrixMode(GL_MODELVIEW);
}

void MultiMarkerTracking(int argc, char ** argv)
{
	cv::ocl::setUseOpenCL(false);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutCreateWindow("AR Homework");
	init(1);
	init(2);

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutMainLoop();
}


void interaction()
{
	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
		exit(-1);
	Mat g_YCrCb;
	Mat g_RGB;
	Mat channels[3];
	Mat g_Gray;
	Mat g_Bgr;
	Mat temp;
	Mat result;

	cvNamedWindow("src", 0);

	for (;;)
	{
		cv::Mat frame;
		cap >> frame; // get a new frame from camera
		frame.copyTo(gSceneImg);
		//cvtColor(frame, g_RGB, CV_RGBA2RGB);
		//cvtColor(g_RGB, g_YCrCb, CV_RGB2YCrCb);
		cvtColor(frame, g_YCrCb, CV_RGB2YCrCb);
	
		split(g_YCrCb, channels);

		imshow("src", frame); //�������� ���

		cv::inRange(channels[1], cv::Scalar(77), cv::Scalar(150), channels[1]);
		cv::inRange(channels[2], cv::Scalar(133), cv::Scalar(173), channels[2]);
		bitwise_and(channels[1], channels[2], g_Gray);
		cvtColor(g_Gray, g_Bgr, CV_GRAY2BGR); //COLOR_GRAY2BGRA�� �ϸ� �ȵ�

		g_Bgr.copyTo(temp);
		cv::rectangle(temp, cv::Point(203, 342), cv::Point(406, 462), cv::Scalar(255, 0, 0));
		cv::line(temp, cv::Point(203, (342 + 462) / 2), cv::Point(406, (342 + 462) / 2), cv::Scalar(255, 0, 0));

		imshow("result", temp);
		//imshow("result", g_Bgr);

		int line_y = (342 + 462) / 2;

		change_cnt = 0; //����ȭ Ƚ�� 0���� �ʱ�ȭ

		for (int x = 203; x<406; x++)
		{
			// get pixel
			Vec3b color = g_Bgr.at<Vec3b>(Point(x, line_y));
			Vec3b color2 = g_Bgr.at<Vec3b>(Point(x + 1, line_y));
			if (color.val[0] == color2.val[0] && color.val[1] == color2.val[1] && color.val[2] == color2.val[2]) //����ȭ�� �������
			{

			}
			else //����ȭ �������
			{
				change_cnt++;
			}
		}

		std::cout << "change_cnt : " << change_cnt << std::endl;
		//���� ���� �� �Ǵ�
		if (change_cnt == 2)
		{
			std::cout << "Rock" << std::endl;
			RockCnt++;
		}
		else if (change_cnt == 4)
		{
			std::cout << "Sissors" << std::endl;
			SissorsCnt++;
		}
		else if (change_cnt == 10)
		{
			std::cout << "Paper" << std::endl;
			PaperCnt++;
		}
		else
			std::cout << "Unknown" << std::endl;
		if (waitKey(30) >= 0) break;

	}
}

int main(int argc, char ** argv) 
{
		std::thread t1(&interaction);

		MultiMarkerTracking(argc, argv);
		t1.join();
		

	return 0;
}

