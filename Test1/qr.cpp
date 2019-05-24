#include <opencv2/opencv.hpp> 
#include <math.h> 
#include <iostream> 
using namespace cv; 
using namespace std; 
bool isXCorner(Mat &image);
bool isYCorner(Mat &image); 
Mat transformCorner(Mat &image, RotatedRect &rect); 

int main(int argc, char** argv) 
{ 
	Mat src = imread("/home/albusyu95/桌面/Test1/1.png"); 
	if (src.empty()) 
	{ 
		printf("could not load image...\n"); 
		return -1; 
	} 
	namedWindow("input image", CV_WINDOW_AUTOSIZE); 
	imshow("input image", src); 
	
	Mat gray, binary; 
	cvtColor(src, gray, COLOR_BGR2GRAY); 
	imwrite("/home/albusyu95/桌面/Test1/qrcode_gray.jpg", gray); 
	threshold(gray, binary, 0, 255, THRESH_BINARY | THRESH_OTSU); 
	imwrite("/home/albusyu95/桌面/Test1/qrcode_binary.jpg", binary); // detect rectangle now 
	vector<vector<Point> > contours; 
	vector<Vec4i> hireachy; 
	findContours(binary.clone(), contours, hireachy, RETR_LIST, CHAIN_APPROX_SIMPLE, Point()); 
	Mat result = Mat::zeros(src.size(), CV_8UC3); 
	for (size_t t = 0; t < contours.size(); t++) 
	{ 
		double area = contourArea(contours[t]); 
		if (area < 100) continue; 
		RotatedRect rect = minAreaRect(contours[t]); // 根据矩形特征进行几何分析 
		float w = rect.size.width; 
		float h = rect.size.height; 
		float rate = min(w, h) / max(w, h); 
		if (rate > 0.85 && w < src.cols/4 && h<src.rows/4) 
		{ 
			printf("angle : %.2f\n", rect.angle); 
			Mat qr_roi = transformCorner(src, rect); 
			if (isXCorner(qr_roi) && isYCorner(qr_roi)) 
			{ 
				drawContours(src, contours, static_cast<int>(t), Scalar(0, 0, 255), 2, 8); 
				imwrite(format("/home/albusyu95/桌面/Test1/contour_%d.jpg", static_cast<int>(t)), qr_roi); 
				drawContours(result, contours, static_cast<int>(t), Scalar(255, 0, 0), 2, 8); 
			} 
		} 
	} 
	imshow("result", src); 
	imwrite("/home/albusyu95/桌面/Test1//qrcode_patters.jpg", src); 
	waitKey(0); 
	return 0; 
}

