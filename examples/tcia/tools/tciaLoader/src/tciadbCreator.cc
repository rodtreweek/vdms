#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>

#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */

#include "pmgd.h"
#include "util.h"
#include "neighbor.h"

#include "TCIALoader.h"
#include "Chrono.h"

int main(int argc, char const *argv[])
{
    int multiplier = 1;
	if (argc < 2) {
        std::cout << "Please specify the db path" << std::endl;
        exit(0);
    }
    else if (argc == 3){
        /* code */
        multiplier = atoi(argv[2]);
    }

    std::string db_path(argv[1]);

	TCIALoader loader(db_path, multiplier);

	struct TCIALoader::TCIALoaderConfig lgg;
    lgg.patients =
    "data/TCGA-LGG/nationwidechildrens.org_clinical_patient_lgg.txt";
    lgg.drug_treatment =
    "data/TCGA-LGG/nationwidechildrens.org_clinical_drug_lgg.txt";
    lgg.radiation_treatment =
    "data/TCGA-LGG/nationwidechildrens.org_clinical_radiation_lgg.txt";
    lgg.omf =
    "data/TCGA-LGG/nationwidechildrens.org_clinical_omf_v4.0_lgg.txt";
    lgg.follow_up =
    "data/TCGA-LGG/nationwidechildrens.org_clinical_follow_up_v1.0_lgg.txt";
    lgg.nte =
    "data/TCGA-LGG/nationwidechildrens.org_clinical_nte_lgg.txt";

    struct TCIALoader::TCIALoaderConfig gbm;
    gbm.patients =
    "data/TCGA-GBM/nationwidechildrens.org_clinical_patient_gbm.txt";
    gbm.drug_treatment =
    "data/TCGA-GBM/nationwidechildrens.org_clinical_drug_gbm.txt";
    gbm.radiation_treatment =
    "data/TCGA-GBM/nationwidechildrens.org_clinical_radiation_gbm.txt";
    gbm.omf =
    "data/TCGA-GBM/nationwidechildrens.org_clinical_omf_v4.0_gbm.txt";
    gbm.follow_up =
    "data/TCGA-GBM/nationwidechildrens.org_clinical_follow_up_v1.0_gbm.txt";
    gbm.nte =
    "data/TCGA-GBM/nationwidechildrens.org_clinical_nte_gbm.txt";
    gbm.images_set = "data/imagesMaping.txt";

    loader.insertDataset(lgg);
    loader.insertDataset(gbm);
    loader.getGraphStats();

	return 0;
}
