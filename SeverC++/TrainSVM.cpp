#include "TrainSVM.h"

void train(){
	cout << "-------- train BOVW SVMs -----------" << endl;
	cout << "read vocabulary form file" << endl;
	Mat vocabulary;
	FileStorage fs("vocabulary_color_1000.yml", FileStorage::READ);
	fs["vocabulary"] >> vocabulary;
	fs.release();

	Ptr<FeatureDetector > detector(new SurfFeatureDetector()); //detector
	Ptr<DescriptorExtractor > extractor(new SurfDescriptorExtractor());//  extractor;
	Ptr<DescriptorMatcher > matcher(new BruteForceMatcher<L2<float> >());
	BOWImgDescriptorExtractor bowide(extractor, matcher);
	bowide.setVocabulary(vocabulary);

	TrainingUtils trainingUtils;
	//setup training data for classifiers
	map<string, Mat> trainingData; 
	trainingData.clear();
	cout << "extract_training_samples.." << endl;
	trainingUtils.trainingSamplesExtract(detector, bowide, trainingData);
	
	cout << "got " << trainingData.size() << " classes." << endl;
	for (map<string, Mat>::iterator it = trainingData.begin(); it != trainingData.end(); ++it) {
		cout << " class " << (*it).first << " has " << (*it).second.rows << " samples" << endl;
	}

	cout << "train SVMs\n"; 
	trainingUtils.trainSVM(trainingData, bowide.descriptorSize(), bowide.descriptorType());

}
	