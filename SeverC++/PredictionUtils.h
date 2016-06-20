#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2\legacy\legacy.hpp"
#include <omp.h>
#include <opencv2/opencv.hpp>
#include <memory>
#include <utility>

using namespace std;
using namespace cv;

class PredictionUtils{
public:
	PredictionUtils();
	map<string, unique_ptr<CvSVM>>& getClassesClassifiers() { return classesClassifiers; }
	void TestImage(Mat& img, string& className);
private:
	map<string, Scalar> classes_colors;
	map<string, unique_ptr<CvSVM>> classesClassifiers;
	Mat vocabulary;
	Ptr<FeatureDetector > detector;
	Ptr<BOWImgDescriptorExtractor > bowide;
	Ptr<DescriptorMatcher > matcher;
	Ptr<DescriptorExtractor > extractor;
	void initSetUp();
};