#include "CoinDetection.h"
#include "TestSVM.h"

#define DEG2RAD 0.017453293f 

void coinHoughCircle(Mat srcImg, Mat houghUsedImg){
	cout << "Running hough circle" << endl;
	// accumulator value
	double dp = 1;
	// minimum distance between the center coordinates of detected circles in pixels
	double minDist = srcImg.rows / 8;
	// min and max radiuis
	int minRadius = 0, maxRadius = 500;
	//Last two
	double iCanny = 100, accum = 25;


	vector<Vec3f> circles;//Store circles in
	HoughCircles(houghUsedImg, circles, CV_HOUGH_GRADIENT, dp, minDist, iCanny, accum, minRadius, maxRadius); //Run hough circle detection
	int count = 1;
	cout << "Number of circles = " << circles.size() << endl;
	/*Loops throughs each circle*/
	for (size_t i = 0; i < circles.size(); i++){
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1])); //Get center point of the circle
		int radius = cvRound(circles[i][2]); //Get the raduis of the circle
		Rect rec(Point(center.x - (radius + 1), center.y - (radius + 1)), Point(center.x + (radius + 1), center.y + (radius + 1))); //Create a bounding box of the circle
		if (rec.y > srcImg.cols || rec.x > srcImg.rows || rec.y > srcImg.cols || rec.x > srcImg.rows) { //Check if circle bounding box is out of bounds
			cout << "Circle " << count << " is out of bounds" << endl;
			count++;
			continue;
		}
		/*Save rectangle*/
		cout << "Circle " << count << " is saved" << endl;
		ostringstream recFileName;
		recFileName << "Coinimages/TEST/" << "Rect" << count << ".jpg";
		Mat rectangle(srcImg, rec);
		imwrite(recFileName.str(), rectangle);
		count++;
	}
}

void preProcessingIMG(Mat img){
	cout << "------Pre-processing of Img------" << endl;
	Mat grayImg, blurImg, threshImg, openingImg, closingImg, outlineImg,element;
	element = getStructuringElement(MORPH_RECT, Size(5, 5), Point(3, 3));
	
	cout << "Grayscale Img" << endl;
	cvtColor(img, grayImg, CV_RGB2GRAY); //Grayscale img
	imwrite("Pre/Gray_Image.jpg", grayImg);
	cout << "Blurring Img" << endl;
	GaussianBlur(grayImg, blurImg, Size(3, 3), 80); //Blur image
	imwrite("Pre/Blur_Image.jpg", blurImg);
	cout << "Threshing Img" << endl;
	threshold(blurImg, threshImg, 0, 255, THRESH_BINARY_INV + THRESH_OTSU); //Thresh image
	imwrite("Pre/Thresh_Image.jpg", threshImg);
	cout << "Opening Img" << endl;
	morphologyEx(threshImg, openingImg, MORPH_OPEN, element, Point(2, 2), 4); //Opening image
	imwrite("Pre/Open_Image.jpg", openingImg);
	cout << "Closing Img" << endl;
	morphologyEx(openingImg, closingImg, MORPH_CLOSE, element, Point(2, 2), 4);//Closing image
	imwrite("Pre/Close_Image.jpg", closingImg);
	cout << "Creating an outline of the Img" << endl;
	morphologyEx(closingImg, outlineImg, MORPH_GRADIENT, element, Point(2, 2), 1); //Outline image
	imwrite("Pre/Outline_Image.jpg", outlineImg);
	cout << "Thicken the outline of the Img" << endl;
	dilate(outlineImg, outlineImg, element, Point(2, 2), 1); //Thick outline image
	imwrite("Pre/OutlineThick.jpg", outlineImg);
	coinHoughCircle(img, outlineImg);
}

void createMat(char* filePath){
	cout << "Converting img to Mat" << endl;
	Mat img = imread(filePath);
	preProcessingIMG(img);
}






