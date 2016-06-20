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

using namespace std;
using namespace cv;

void train();
class TrainingUtils{
public:
	void trainSVM(map<string, Mat>& training_data_classes, int response_cols, int response_type);
	void trainingSamplesExtract(Ptr<FeatureDetector>& detector, BOWImgDescriptorExtractor& bowide, map<string, Mat>& classes_training_data);
};
