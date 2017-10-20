#include <iostream>
#include <fstream>
#include <Windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <thread>
#include ".\include\GL\freeglut.h"
#include "opencv2\opencv.hpp"
#include "opencv2\core\ocl.hpp"
#include "glm.hpp"
#include <math.h>

#pragma comment(lib, "opencv_world310.lib")

#define	FRAME_WIDTH		640 //1080
#define	FRAME_HEIGHT	480 //

///< unit: cm
#define	MARKER_HEIGHT	7
#define MARKER_WIDTH	7

#define	zNear	1.0
#define	zFar	1000000

cv::Mat gMarkerImg1;	///< 마커 이미지1
cv::Mat gMarkerImg2;	///< 마커 이미지2
cv::Mat gSceneImg;	///< 카메라로 캡쳐한 이미지
cv::Mat	gOpenGLImg;	///< OpenGL로 렌더링할 이미지
cv::VideoCapture gVideoCapture;	///< 카메라 캡쳐 객체

cv::Ptr<cv::ORB> detector1;	///< ORB 특징점 추출기1
cv::Ptr<cv::ORB> detector2;	///< ORB 특징점 추출기2
cv::Ptr<cv::DescriptorMatcher> matcher1;	///< ORB 특징정보 매칭 객체1
cv::Ptr<cv::DescriptorMatcher> matcher2;	///< ORB 특징정보 매칭 객체2

///< 마커 및 카메라로 캡쳐한 이미지의 ORB 특징정보(keypoints)
std::vector<cv::KeyPoint> gvMarkerKeypoints1, gvSceneKeypoints1;
std::vector<cv::KeyPoint> gvMarkerKeypoints2, gvSceneKeypoints2;

///< 마커 및 카메라로 캡쳐한 이미지의 ORB 특징정보(descriprtors)
cv::Mat gMarkerDescriptors1, gSceneDescriptors1;
cv::Mat gMarkerDescriptors2, gSceneDescriptors2;

cv::Mat E1;	///< 마커 좌표계에서 카메라 좌표계로의 변환 행렬
cv::Mat E2;

cv::Mat K;	///< 카메라 내부 파라메터

glm::vec3 ExtractCameraPos_NoScale(const glm::mat4 & a_modelView)
{
	glm::mat3 rotMat(a_modelView);
	glm::vec3 d(a_modelView[3]);

	glm::vec3 retVec = -d * rotMat;
	return retVec;
}

void init(int marker_no)
{
	///< 마커에서 카메라로의 변환 행렬을 초기화 한다.
	E1 = cv::Mat::eye(4, 4, CV_64FC1);
	E2 = cv::Mat::eye(4, 4, CV_64FC1);
	K = cv::Mat::eye(3, 3, CV_64FC1);

	///< 카메라 내부 파라메터 초기화
	K.at<double>(0, 0) = 689.958461;
	K.at<double>(1, 1) = 698.647064;
	K.at<double>(0, 2) = 329.759390;
	K.at<double>(1, 2) = 230.330858;

	if (marker_no == 1)
	{
		///< 마커 이미지를 읽는다.
		gMarkerImg1 = cv::imread("marker3.jpg", 0);

		///< 카메라를 초기화

		if (!gMarkerImg1.data || !gVideoCapture.open(0)) {
			std::cerr << "초기화를 수행할 수 없습니다." << std::endl;
			exit(-1);
		}

		///< 특징정보 추출기와 매칭 객체 초기화
		detector1 = cv::ORB::create();
		matcher1 = cv::DescriptorMatcher::create("BruteForce-Hamming");

		///< 마커 영상의 특징정보 추출
		detector1->detect(gMarkerImg1, gvMarkerKeypoints1);
		detector1->compute(gMarkerImg1, gvMarkerKeypoints1, gMarkerDescriptors1);

		///< 마커 영상의 실제 크기 측정
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
		///< 마커 이미지를 읽는다.
		gMarkerImg2 = cv::imread("marker2.jpg", 0);

		///< 카메라를 초기화

		if (!gMarkerImg2.data || !gVideoCapture.open(0)) {
			std::cerr << "초기화를 수행할 수 없습니다." << std::endl;
			exit(-1);
		}

		///< 특징정보 추출기와 매칭 객체 초기화
		detector2 = cv::ORB::create();
		matcher2 = cv::DescriptorMatcher::create("BruteForce-Hamming");

		///< 마커 영상의 특징정보 추출
		detector2->detect(gMarkerImg2, gvMarkerKeypoints2);
		detector2->compute(gMarkerImg2, gvMarkerKeypoints2, gMarkerDescriptors2);

		///< 마커 영상의 실제 크기 측정
		for (int i = 0; i < (int)gvMarkerKeypoints2.size(); i++) {
			gvMarkerKeypoints2[i].pt.x /= gMarkerImg2.cols;
			gvMarkerKeypoints2[i].pt.y /= gMarkerImg2.rows;

			gvMarkerKeypoints2[i].pt.x -= 0.5;
			gvMarkerKeypoints2[i].pt.y -= 0.5;

			gvMarkerKeypoints2[i].pt.x *= MARKER_WIDTH;
			gvMarkerKeypoints2[i].pt.y *= MARKER_HEIGHT;
		}
	}
	///< OpenGL 초기화
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

///< 카메라 내부 파마메터에서 OpenGL 내부 파라메터 변환
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

///< 호모그래피로부터 마커에서 카메라로의 변환 행렬 추출
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

///< 카메라로부터 영상을 읽고, 특징정보 추출한 후, 마커 영상과의 매칭 및 호모그래피를 추정
///< 추정한 호모그래피로부터 마커에서 카메라로의 변환 행렬 추정
void processVideoCapture1(void)
{
	cv::Mat grayImg;

	///< 카메라로부터 영상획득
	gVideoCapture >> gSceneImg;

	///< 특징정보 추출을 위하여 흑백영상으로 변환
	cv::cvtColor(gSceneImg, grayImg, CV_BGR2GRAY);

	///< 카메라로부터 획득한 영상의 특징정보 추출
	detector1->detect(grayImg, gvSceneKeypoints1);
	detector1->compute(grayImg, gvSceneKeypoints1, gSceneDescriptors1);

	///< 마커 특징정보와 매칭 수행
	std::vector<std::vector<cv::DMatch>> matches;
	matcher1->knnMatch(gMarkerDescriptors1, gSceneDescriptors1, matches, 2);

	std::vector<cv::DMatch> good_matches;
	for (int i = 0; i < (int)matches.size(); i++) {
		if (matches[i][0].distance < 0.9 * matches[i][1].distance) {
			good_matches.push_back(matches[i][0]);
		}
	}

	///< 마커 특징정보와 충분한 대응점이 있는 경우에....
	if (good_matches.size() > 10) {
		std::vector<cv::Point2f> vMarkerPts;
		std::vector<cv::Point2f> vScenePts;

		///< 호모그래피 추정
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

		///< 호모그래피로부터 마커에서 카메라 좌표로의 변환 행렬 추정
		if (calculatePoseFromH(H, R, T)) {
			R.copyTo(E1.rowRange(0, 3).colRange(0, 3));
			T.copyTo(E1.rowRange(0, 3).col(3));

			static double changeCoordArray[4][4] = { { 1, 0, 0, 0 },{ 0, -1, 0, 0 },{ 0, 0, -1, 0 },{ 0, 0, 0, 1 } };
			static cv::Mat changeCoord(4, 4, CV_64FC1, changeCoordArray);

			E1 = changeCoord * E1;
			//첫번째 마커에 대한 T벡터set
			std::cout << "T1:" << std::endl;
			t1 = true;
			T.copyTo(T1);
			std::cout << T1.rows << " " << T1.cols << std::endl;
			std::cout << T1.at<double>(0) << " " << T1.at<double>(1) << " " << T1.at<double>(2) << std::endl;
			//gMatFlipImage.at<cv::Vec3b>(i, j)
			
			std::cout << std::endl;

			double sum = 0.0;

			if (t1 == true && t2 == true)
			{
				std::cout << "T1 ~ T2 distance : " << sqrt(pow(T1.at<double>(0) - T2.at<double>(0), 2) + pow(T1.at<double>(1) - T2.at<double>(1), 2) + pow(T1.at<double>(2) - T2.at<double>(2), 2)) << std::endl;
			}
		}
	}


	if (gSceneImg.data)
		cv::flip(gSceneImg, gOpenGLImg, 0);

	glutPostRedisplay();
}

void processVideoCapture2(void)
{
	cv::Mat grayImg;

	///< 카메라로부터 영상획득
	gVideoCapture >> gSceneImg;

	///< 특징정보 추출을 위하여 흑백영상으로 변환
	cv::cvtColor(gSceneImg, grayImg, CV_BGR2GRAY);

	///< 카메라로부터 획득한 영상의 특징정보 추출
	detector2->detect(grayImg, gvSceneKeypoints2);
	detector2->compute(grayImg, gvSceneKeypoints2, gSceneDescriptors2);

	///< 마커 특징정보와 매칭 수행
	std::vector<std::vector<cv::DMatch>> matches;
	matcher2->knnMatch(gMarkerDescriptors2, gSceneDescriptors2, matches, 2);

	std::vector<cv::DMatch> good_matches;
	for (int i = 0; i < (int)matches.size(); i++) {
		if (matches[i][0].distance < 0.9 * matches[i][1].distance) {
			good_matches.push_back(matches[i][0]);
		}
	}

	///< 마커 특징정보와 충분한 대응점이 있는 경우에....
	if (good_matches.size() > 10) {
		std::vector<cv::Point2f> vMarkerPts;
		std::vector<cv::Point2f> vScenePts;

		///< 호모그래피 추정
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

		///< 호모그래피로부터 마커에서 카메라 좌표로의 변환 행렬 추정
		if (calculatePoseFromH(H, R, T)) {
			R.copyTo(E2.rowRange(0, 3).colRange(0, 3));
			T.copyTo(E2.rowRange(0, 3).col(3));

			static double changeCoordArray[4][4] = { { 1, 0, 0, 0 },{ 0, -1, 0, 0 },{ 0, 0, -1, 0 },{ 0, 0, 0, 1 } };
			static cv::Mat changeCoord(4, 4, CV_64FC1, changeCoordArray);

			E2 = changeCoord * E2;

			//두번째 마커에 대한 T벡터set
			std::cout << "T2 :" << std::endl;
			t2 = true;
			T.copyTo(T2);
			std::cout << T2.rows << " " << T2.cols << std::endl;
			std::cout << T2.at<double>(0) << " " << T2.at<double>(1) << " " << T2.at<double>(2) << std::endl;
			//gMatFlipImage.at<cv::Vec3b>(i, j)

			std::cout << std::endl;

			if (t1 == true && t2 == true)
			{
				std::cout << "T1 ~ T2 distance : " << sqrt(pow(T1.at<double>(0) - T2.at<double>(0), 2) + pow(T1.at<double>(1) - T2.at<double>(1), 2) + pow(T1.at<double>(2) - T2.at<double>(2), 2)) << std::endl;
			}
		}
	}


	if (gSceneImg.data)
		cv::flip(gSceneImg, gOpenGLImg, 0);

	glutPostRedisplay();
}

float angle1 = 30.0f;

void displayThread1() {
	///< 마커로부터 카메라로의 변환행렬을 통해 마커좌표계로의 변환
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
	glColor3f(1.0f, 1.0f, 0.0);
	///< 마커 좌표계의 중심에서 객체 렌더링

	if (angle1 >= 360.0f)
	{
		angle1 = 0.0f;
	}
	glRotatef(angle1, 0.0f, 1.0f, 0.0f);
	glTranslated(5, 0, 0);
	glRotatef(angle1, 0.0f, 1.0f, 0.0f); //자전

	angle1 += 30.0f;

	glutWireTeapot(2.5);
	
}

float angle2 = -30.0f;

void displayThread2() {
	///< 마커로부터 카메라로의 변환행렬을 통해 마커좌표계로의 변환
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	cv::Mat temp2 = E2.t();

	glMultMatrixd((double *)temp2.data);
	//obj2.draw2("");
	glLineWidth(3.0f);
	// Render a color-cube consisting of 6 quads with different colors
//	glLoadIdentity();                 // Reset the model-view matrix
	glTranslatef(1.5f, 0.0f, -7.0f);  // Move right and into the screen

	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f); glVertex3d(0.0, 0.0, 0.0); glVertex3d(10.0, 0.0, 0.0);
	glColor3f(0.0f, 1.0f, 0.0f); glVertex3d(0.0, 0.0, 0.0); glVertex3d(0.0, 10.0, 0.0);
	glColor3f(0.0f, 0.0f, 1.0f); glVertex3d(0.0, 0.0, 0.0); glVertex3d(0.0, 0.0, 10.0);
	glEnd();

	glRotated(-90.0, 1.0, 0.0, 0.0);
	glLineWidth(4.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	///< 마커 좌표계의 중심에서 객체 렌더링

	if (angle2 <= -360.0f)
	{
		angle2 = 0.0f;
	}
	glRotatef(angle2, 0.0f, 1.0f, 0.0f);
	glTranslated(5, 0, 0);
	glRotatef(angle2, 0.0f, 1.0f, 0.0f); //자전
	angle2 -= 30.0f;
	glutWireCube(2.0);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	///< 배경 영상 렌더링
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

int main(int argc, char** argv)
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

	return 0;
}