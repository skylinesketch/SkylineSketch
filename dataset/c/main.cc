#include "DataGenerator.h"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <cstring>

using namespace std;

enum DatasetType { INDP, CORL, ANTI };

struct Config {
	int nNumObj;
	int nNumAtt;
	DatasetType type;
	char filename[100];

	Config() : nNumObj(1000000), nNumAtt(11), type(INDP) {
		memset(filename, 0, 100*sizeof(char));
	}
};

void free_dataset(vector<double*> &dataset) {
	for (auto t : dataset) {
		delete [] t;
	}
}

int main(int argc, char *argv[]) {
	Config cfg;
	char c;

	while ((c = getopt(argc, argv, "n:a:t:o:")) != -1) {
		switch(c) {
			case 'n':
				cfg.nNumObj = atoi(optarg);
				break;
			case 'a':
				cfg.nNumAtt = atoi(optarg);
				break;
			case 't':
				cfg.type = DatasetType(atoi(optarg));
				break;
			case 'o':
				strcpy(cfg.filename, optarg);
				break;
			default:
				cerr << "option error!" << endl;
				return 1;
		}
	}

	vector<double*> dataset;

	switch (cfg.type) {
		case INDP:
			GenerateIndependent(cfg.nNumObj, cfg.nNumAtt, false, dataset);
			break;
		case CORL:
			GenerateCorrelated(cfg.nNumObj, cfg.nNumAtt, false, dataset);
			break;
		case ANTI:
			GenerateAntiCorrelated(cfg.nNumObj, cfg.nNumAtt, false, dataset);
			break;
		default:
			cerr << "unknown type!" << endl;
	}

	cout << dataset.size() << endl;

	ofstream output(cfg.filename);
	for (auto t : dataset) {
		for (int i = 0; i < cfg.nNumAtt; ++i)
			output << t[i] << ' ';
		output << endl;
	}

	free_dataset(dataset);
	return 0;
}
