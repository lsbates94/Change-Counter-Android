#include "PredictionUtils.h"

PredictionUtils::PredictionUtils(){
	initSetUp();
	Ptr<FeatureDetector > detectorA(new SurfFeatureDetector());
	Ptr<DescriptorMatcher > matcherA(new BruteForceMatcher<L2<float> >());
	Ptr<DescriptorExtractor > extractorA(new SurfDescriptorExtractor());
	detector = detectorA;
	matcher = matcherA;
	extractor = extractorA;
	bowide = Ptr<BOWImgDescriptorExtractor>(new BOWImgDescriptorExtractor(extractor, matcher));
	bowide->setVocabulary(vocabulary);

}

void PredictionUtils::initSetUp(){
	string dir, filepath;
	DIR *dp;
	struct dirent *dirp;
	struct stat filestat;

	cout << "load SVM classifiers" << endl;
	dir = ".";
	dp = opendir(dir.c_str());

	while ((dirp = readdir(dp))){
		filepath = dir + "/" + dirp->d_name;
		if (stat(filepath.c_str(), &filestat)) continue;
		if (S_ISDIR(filestat.st_mode))         continue;
		if (filepath.find("SVM_classifier") != string::npos){
			string className = filepath.substr(filepath.rfind('_') + 1, filepath.rfind('.') - filepath.rfind('_') - 1);
			cout << "load " << filepath << ", class: " << className << endl;
			classesClassifiers.insert(make_pair(className, unique_ptr<CvSVM>(new CvSVM())));
			classesClassifiers[className]->load((filepath.c_str()));
		}
	}
	closedir(dp);

	int count = 0;
	for (map<string, unique_ptr<CvSVM>>::iterator it = classesClassifiers.begin(); it != classesClassifiers.end(); ++it) {
		classes_colors[(*it).first] = Scalar((float)(count++) / (float)(classesClassifiers.size())*180.0f, 255, 255);
	}

	cout << "Loading vocab" << endl;
	FileStorage fs("vocabulary_color_1000.yml", FileStorage::READ);
	fs["vocabulary"] >> vocabulary;
	fs.release();
}

void PredictionUtils::TestImage(Mat& img, string& className){
	map<string, pair<int, float> > found_classes;
	Mat response_hist, imgCopy;
	img.copyTo(imgCopy);
	vector<KeyPoint> keypoints;
	detector->detect(imgCopy, keypoints);
	bowide->compute(img, keypoints, response_hist);
	if (response_hist.cols == 0 || response_hist.rows == 0){
		cout << "No response" << endl;
		return;
	}
	try {
		float minf = FLT_MAX; string minclass;
		for (map<string, unique_ptr<CvSVM>>::iterator it = classesClassifiers.begin(); it != classesClassifiers.end(); ++it) {
			float res = (*it).second->predict(response_hist, true);
			if (res > 1.0) continue;
			if (res < minf) {
				minf = res; 
				minclass = (*it).first;
			}
			found_classes[minclass].first++;
			found_classes[minclass].second += minf;
		}
		className.clear();
		className = minclass;
	}
	catch (cv::Exception){

	}
}