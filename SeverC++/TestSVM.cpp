#include "TestSVM.h"

float testSVM(){
	PredictionUtils p;
	map<string, unique_ptr<CvSVM>>& classes_classifiers = p.getClassesClassifiers();
	map<string, map<string, int> > confusion_matrix;
	for (map<string, unique_ptr<CvSVM>>::iterator it = classes_classifiers.begin(); it != classes_classifiers.end(); ++it) {
		for (map<string, unique_ptr<CvSVM>>::iterator it1 = classes_classifiers.begin(); it1 != classes_classifiers.end(); ++it1) {
			string class1 = (*it).first;
			string class2 = (*it1).first;
			confusion_matrix[class1][class2] = 0;
		}
	}

	string dir, filepath;
	DIR *dp;
	struct dirent *dirp;
	struct stat filestat;

	//get images
	dir = "Coinimages/TEST";
	dp = opendir(dir.c_str());
	float totalCount = 0.00f;
	while (dirp = readdir(dp))

	{
		filepath = dir + "/" + dirp->d_name;

		// If the file is a directory (or is in some way invalid) we'll skip it 
		if (stat(filepath.c_str(), &filestat)) continue;
		if (S_ISDIR(filestat.st_mode))         continue;
		if (dirp->d_name[0] == '.')			  continue; //hidden file!

		Mat img = imread(filepath);
		if (!img.data){
			cout << "Img contains no data" << endl;
			continue;
		}
		string className;
		p.TestImage(img, className);
		cout << "Filepath: " << filepath << " BEST CLASS: " << className << endl;
		if (className == "1p"){
			totalCount += 0.01f;
		}
		else if (className == "2p") {
			totalCount += 0.02f;
		}
		else if (className == "5p"){
			totalCount += 0.05f;
		}
		else if (className == "10p"){
			totalCount += 0.10f;
		}
		else if (className == "20p"){
			totalCount += 0.20f;
		}
		else if (className == "50p"){
			totalCount += 0.50f;
		}
		else if (className == "1pound"){
			totalCount += 1.00f;
		}
		else if (className == "2pound"){
			totalCount += 2.00f;
		}
	}
	closedir(dp);
	return totalCount;
}