#include "TrainSVM.h"

void TrainingUtils::trainSVM(map<string, Mat>& training_data_classes, int response_cols, int response_type){

	cout << "train 1-vs-all SVMs" << endl;
	vector<string> classes_names;
	for (map<string, Mat>::iterator it = training_data_classes.begin(); it != training_data_classes.end(); ++it) {
		classes_names.push_back((*it).first);
	}

	for (int i = 0; i < classes_names.size(); i++) {
		string class_ = classes_names[i];
		cout <<  " training class: " << class_ << ".." << endl;

		Mat samples(0, response_cols, response_type);
		Mat labels(0, 1, CV_32FC1);

		//copy class samples and label
		cout << "adding " << training_data_classes[class_].rows << " positive" << endl;
		samples.push_back(training_data_classes[class_]);
		Mat class_label = Mat::ones(training_data_classes[class_].rows, 1, CV_32FC1);
		labels.push_back(class_label);

		//copy rest samples and label
		for (map<string, Mat>::iterator it1 = training_data_classes.begin(); it1 != training_data_classes.end(); ++it1) {
			string not_class_ = (*it1).first;
			if (not_class_.compare(class_) == 0) continue;
			samples.push_back(training_data_classes[not_class_]);
			class_label = Mat::zeros(training_data_classes[not_class_].rows, 1, CV_32FC1);
			labels.push_back(class_label);
		}

		cout << "Train.." << endl;
		Mat samples_32f; samples.convertTo(samples_32f, CV_32F);
		if (samples.rows == 0) continue; //phantom class?!
		CvSVM classifier;
		classifier.train(samples_32f, labels);

		{
			stringstream ss;
			ss << "SVM_classifier_";
			ss << class_ << ".yml";
			cout << "Save.." << endl;
			classifier.save(ss.str().c_str());
		}
	}
}

//Extract training samples
void TrainingUtils::trainingSamplesExtract(Ptr<FeatureDetector>& detector, BOWImgDescriptorExtractor& bowide, map<string, Mat>& training_data_classes){
	cout << "look in train data" << endl;
	Ptr<ifstream> ifs(new ifstream("Training.txt"));
	int totalSamples = 0;
	vector<string> classesNames;

	char buf[255];
	int count = 0;
	vector<string> lines;
	do{ //Read training file
		ifs->getline(buf, 255);
		if (buf[0] == '\0') {
			cout << "END" << endl;
			continue;
		}
		lines.push_back(buf);
	} while (!ifs->eof());

	for (int i = 0; i < lines.size(); i++) {
		vector<KeyPoint> keypoints;
		Mat response_hist;
		Mat img;
		string filepath;

		string line(lines[i]);
		istringstream iss(line);
		iss >> filepath;
		char delim;
		string className;
		iss >> className >> delim;
		className = "class_" + className;
		if (className.size() == 0) continue;

		img = imread(filepath);
		detector->detect(img, keypoints); //Detect
		bowide.compute(img, keypoints, response_hist); //Compute
		cout << ".";
		cout.flush();
		if (training_data_classes.count(className) == 0) { //Class not created
			training_data_classes[className].create(0, response_hist.cols, response_hist.type());
			classesNames.push_back(className);
		}
		training_data_classes[className].push_back(response_hist);
		totalSamples++;
	}
	cout << endl;

	cout << "save to file.." << endl;
	{
		FileStorage fs("training_samples.yml", FileStorage::WRITE);
		for (map<string, Mat>::iterator it = training_data_classes.begin(); it != training_data_classes.end(); ++it) {
			cout << "save " << (*it).first << endl;
			fs << (*it).first << (*it).second;
		}
	}
}
