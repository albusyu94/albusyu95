#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <string>
#include <iostream>

using namespace std;
using namespace cv;
void rotate_arbitrarily_angle(Mat &src,Mat &dst,float angle);
RotatedRect FindMaxRect(Mat &src,vector< vector<cv::Point> > contours,vector<Vec4i> hierarchy);
void DrawRotateRect(RotatedRect rect,Mat &src);

int main(int argc, char *argv[])
{
	Mat frame;
	Mat src,binary;
	Mat roi_qr;
	Mat R;
	int k = 0;
	vector< vector<cv::Point> > contours1,contours2;   //轮廓 
	vector<Vec4i> hierarchy1,hierarchy2;   //层
	int thresh = 180;  
	VideoCapture cap("/dev/video1");  //0
    if(!cap.isOpened())
    {
        cout << "Cannot open a camera" << endl;
        return -4;
    }
	cout << "Camera Open" << endl;
    while(1)
    {
		string Img_Name =std::to_string(k) + ".jpg";
		cap >> frame;
		cvtColor(frame, src, COLOR_BGR2GRAY); //转灰度
		threshold(src, binary, 0, 255, THRESH_BINARY | THRESH_OTSU); //二值化 大津
	
		RotatedRect Rect1; //原图，旋转的矩形
		RotatedRect Rect2; //摆正的矩形
		Rect1 = FindMaxRect(binary,contours1,hierarchy1); //原图最大矩形
		rotate_arbitrarily_angle(binary,R,Rect1.angle);//原图矩形被旋转
		Rect2 = FindMaxRect(R,contours2,hierarchy2);//旋转后图片的最大矩形
		
		//findContours(R, contours2, hierarchy2, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
		Scalar color1 = Scalar(255,0,0);
		Scalar color2 = Scalar(0,255,0);
		
		DrawRotateRect(Rect1,frame); //画原图的矩形
		DrawRotateRect(Rect2,R); //旋转后的矩形
		float SizeError = 0;
		SizeError =fabs( Rect1.size.height * Rect1.size.width  > Rect2.size.height * Rect2.size.width);
		//cout << SizeError << endl;
		if(SizeError < 10) 
		{
			//cout << "catch" <<endl;
			//imwrite(Img_Name,R);
			k++;
		}
		imshow("Rect", frame);
		//imshow("R", R);
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

void DrawRotateRect(RotatedRect rect,Mat &src)
{
	Point2f vertices[4];      //定义矩形的4个顶点
    rect.points(vertices);   //计算矩形的4个顶点
	for (int i = 0; i < 4; i++)
	line(src, vertices[i], vertices[(i + 1) % 4], Scalar(0, 255, 0),2);
	//cv::circle(src, rect.center, 3, Scalar(0,0,255));
}

RotatedRect FindMaxRect(Mat &src,vector< vector<cv::Point> > contours,vector<Vec4i> hierarchy)
{
	findContours(src, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	vector< RotatedRect > Rect(contours.size());
	vector< vector<cv::Point> > polyContours(contours.size());
	float Error = 0;
	int maxRect = 0;
	for(int index = 0; index < contours.size(); index++)
	{
		int i = 0;
		approxPolyDP(contours[index],polyContours[index],10,true);
		if(contours[index].size()<60) i = index;
			Rect[index] = minAreaRect(polyContours[i]);
		Error = fabs(Rect[index].size.height - Rect[index].size.width);
		if(Rect[index].size.height * Rect[index].size.width  > Rect[maxRect].size.height * Rect[maxRect].size.width && Error < 2)
			maxRect = index;	
	}
	return Rect[maxRect];
}
