#include "BuildVocabolary.h"
#include "TrainSVM.h"

void buildTrainingFile(string dir, string classMember){
	DIR *dp;
	struct dirent *dirp;
	struct stat filestat;

	dp = opendir(dir.c_str());
	string filepath;
	ofstream file("Training.txt", ios::out | ios::app);
	dp = opendir(dir.c_str());
	while ((dirp = readdir(dp)))
	{
		filepath = dir + "/" + dirp->d_name;

		// If the file is a directory (or is in some way invalid) we'll skip it 
		if (stat(filepath.c_str(), &filestat)) continue;
		if (S_ISDIR(filestat.st_mode))         continue;

		file << filepath << " " << classMember << "\n";
	}
	file.close();
	closedir(dp);
}

void buildVocab(){
	buildTrainingFile("Coinimages/TRAIN/2p", "2p");
	buildTrainingFile("Coinimages/TRAIN/10p", "10p");
	buildTrainingFile("Coinimages/TRAIN/5p", "5p");
	buildTrainingFile("Coinimages/TRAIN/20p", "20p");
	buildTrainingFile("Coinimages/TRAIN/50p", "50p");
	buildTrainingFile("Coinimages/TRAIN/1p", "1p");
	buildTrainingFile("Coinimages/TRAIN/£1", "1pound");
	buildTrainingFile("Coinimages/TRAIN/£2", "2pound");

	string dir = "Coinimages/TRAIN/2p", filepath;
	DIR *dp;
	struct dirent *dirp;
	struct stat filestat;

	dp = opendir(dir.c_str());
	// detecting keypoints
	SurfFeatureDetector detector(1000);
	vector<KeyPoint> keypoints;

	// computing descriptors
	Ptr<DescriptorExtractor > extractor(new SurfDescriptorExtractor());//  extractor;
	Mat descriptors;
	Mat training_descriptors(1, extractor->descriptorSize(), extractor->descriptorType());
	Mat img;
	int count = 0;
	int x = 1;
	cout << "------- build vocabulary ---------\n";
	while (x <= 8){
		dp = opendir(dir.c_str());
		while (count++ < 15 && (dirp = readdir(dp)))
		{
			filepath = dir + "/" + dirp->d_name;

			// If the file is a directory (or is in some way invalid) we'll skip it 
			if (stat(filepath.c_str(), &filestat)) continue;
			if (S_ISDIR(filestat.st_mode))         continue;

			img = imread(filepath);
			detector.detect(img, keypoints);
			extractor->compute(img, keypoints, descriptors);

			training_descriptors.push_back(descriptors);
			cout << filepath << endl;
		}
		closedir(dp);
		x++;
		cout << "\nNEXT ROW" << x << endl;
		if (x == 2){
			dir = "Coinimages/TRAIN/10p";
		}
		else if (x == 3) {
			dir = "Coinimages/TRAIN/5p";
		}
		else if (x == 4) {
			dir = "Coinimages/TRAIN/1p";
		}
		else if (x == 5) {
			dir = "Coinimages/TRAIN/20p";
		}
		else if (x == 6) {
			dir = "Coinimages/TRAIN/50p";
		}
		else if (x == 7) {
			dir = "Coinimages/TRAIN/£1";
		}
		else if (x == 8) {
			dir = "Coinimages/TRAIN/£2";
		}
		count = 0;
	}
	cout << endl;
	cout << "Total descriptors: " << training_descriptors.rows << endl;
	FileStorage fs("training_descriptors.yml", FileStorage::WRITE);
	fs << "training_descriptors" << training_descriptors;
	fs.release();

	BOWKMeansTrainer bowtrainer(150); //num clusters
	bowtrainer.add(training_descriptors);
	cout << "cluster BOW features" << endl;
	Mat vocabulary = bowtrainer.cluster();

	FileStorage fs1("vocabulary_color_1000.yml", FileStorage::WRITE);
	fs1 << "vocabulary" << vocabulary;
	fs1.release();

}

