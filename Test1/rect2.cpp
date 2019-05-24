#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <string>
#include <iostream>

using namespace std;
using namespace cv;
void rotate_arbitrarily_angle(Mat &src,Mat &dst,float angle);
//RotatedRect FindMaxRect(Mat &src,Mat &dst);
int main(int argc, char *argv[])
{
	//for(;;)
	//{
	Mat frame;
	Mat src,binary;
	Mat roi_qr;
	Mat R;
	vector< vector<cv::Point> > contours; 
	vector<Vec4i> hierarchy;
	int thresh = 180;
	VideoCapture cap("/dev/video1");  //0
    if(!cap.isOpened())
    {
        cout << "Cannot open a camera" << endl;
        return -4;
    }
    while(1)
    {
	cap >> frame;
	cvtColor(frame, src, COLOR_BGR2GRAY); //转灰度
	Mat polyPic = Mat::zeros(src.size(),CV_8UC3);
	//threshold(src,binary,0,255,cv::THRESH_BINARY); //二值化
	threshold(src, binary, 0, 255, THRESH_BINARY | THRESH_OTSU); 
	//Canny(binary, binary, 30, 30 * 2, 3);
	//medianBlur(binary,binary,7);//中值滤波
	findContours(binary, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); // 轮廓提取
	//drawContours(frame,contours,-1,cv::Scalar::all(255));
	vector< vector<cv::Point> > polyContours(contours.size());
	//vector< vector<cv::Point> > rects_ploy(contours.size());
	cv::Point2f center;
	vector< RotatedRect > rects_ploy(contours.size());
	//vector< Rect > rects_ploy(contours.size());	
	int maxArea = 0;
	int maxRect = 0;
	for(int index = 0; index < contours.size(); index++)
	{
		int i = 0;
		approxPolyDP(contours[index],polyContours[index],10,true);
		if(contours[index].size()<60) i = index;
		//rects_ploy[index] = boundingRect(polyContours[i]);
		rects_ploy[index] = minAreaRect(polyContours[i]);
		//if(rects_ploy[index].area() > rects_ploy[maxRect].area())
		if(rects_ploy[index].size.height * rects_ploy[index].size.width  > rects_ploy[maxRect].size.height * rects_ploy[maxRect].size.width)
			maxRect = index;
		//cout << rects_ploy[index].size <<endl;
		if(contourArea(contours[index]) > contourArea(contours[maxArea])&& contours[index].size()>32 && contours[index].size()<=40)
			maxArea = index;
		approxPolyDP(contours[index],polyContours[index],10,true);		
	}
	Scalar color1 = Scalar(255,0,0);
	Scalar color2 = Scalar(0,255,0);
	//center.x = rects_ploy[maxRect].x + cvRound(rects_ploy[maxRect].width/2.0);
    //center.y = rects_ploy[maxRect].y + cvRound(rects_ploy[maxRect].height/2.0);	
	//rectangle(frame,rects_ploy[maxRect],color1,1,8);
	Point2f vertices[4];      //定义矩形的4个顶点
    rects_ploy[maxRect].points(vertices);   //计算矩形的4个顶点
	for (int i = 0; i < 4; i++)
	    line(frame, vertices[i], vertices[(i + 1) % 4], Scalar(0, 255, 0),2);
	cv::circle(frame, rects_ploy[maxRect].center, 3, Scalar(0,0,255));
		//先找出四边型 再找出面积最大的
	//drawContours(polyPic,polyContours,maxArea,cv::Scalar::all(255));
	//roi_qr
	cout << rects_ploy[maxRect].angle << endl;
	drawContours(src,polyContours,maxArea,color2);	
	rotate_arbitrarily_angle(frame,R,rects_ploy[maxRect].angle);
	imshow("Rect", frame);
	imshow("Contours", R);
	imshow("binary", binary);
	if(waitKey(10)>0) break;
   }
	return 0;
}


void rotate_arbitrarily_angle(Mat &src,Mat &dst,float angle)
{
    float radian = (float) (angle /180.0 * CV_PI);
 
    //填充图像
    int maxBorder =(int) (max(src.cols, src.rows)* 1.414 ); //即为sqrt(2)*max
    int dx = (maxBorder - src.cols)/2;
    int dy = (maxBorder - src.rows)/2;
    copyMakeBorder(src, dst, dy, dy, dx, dx, BORDER_CONSTANT);
 
    //旋转
    Point2f center( (float)(dst.cols/2) , (float) (dst.rows/2));
    Mat affine_matrix = getRotationMatrix2D( center, angle, 1.0 );//求得旋转矩阵
    warpAffine(dst, dst, affine_matrix, dst.size());
 
    //计算图像旋转之后包含图像的最大的矩形
    float sinVal = abs(sin(radian));
    float cosVal = abs(cos(radian));
    Size targetSize( (int)(src.cols * cosVal +src.rows * sinVal),
                     (int)(src.cols * sinVal + src.rows * cosVal) );
 
    //剪掉多余边框
    int x = (dst.cols - targetSize.width) / 2;
    int y = (dst.rows - targetSize.height) / 2;
    Rect rect(x, y, targetSize.width, targetSize.height);
    dst = Mat(dst,rect);
}
/*

*/
