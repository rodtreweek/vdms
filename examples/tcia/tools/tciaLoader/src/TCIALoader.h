#pragma once
#define __STDC_FORMAT_MACROS

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <list>
#include <unordered_map>
#include <map>

#include "pmgd.h"

class TCIALoader{

public:

    typedef std::list<std::reference_wrapper<PMGD::Node>> NodeList;

    // < DrugName, <efective, not-efective> >
    typedef std::map<std::string,std::pair<int, int>> Histogram;

    // This struct will contains the path to the files to create
    // the database
    struct TCIALoaderConfig {
        std::string patients;
        std::string follow_up;
        std::string nte; // new_tumor_event
        std::string omf;
        std::string drug_treatment;
        std::string radiation_treatment;
        std::string images_set;
        TCIALoaderConfig();
    };

    TCIALoader(std::string database_path, int multiplier);

    void getGraphStats();

    void insertDataset(TCIALoaderConfig& config);

    PMGD::Graph* getGraph(){return _db;}

private:

    PMGD::Graph* _db;
    std::string _db_path;
    TCIALoaderConfig _config;

    int _rep_counter;
    int _multiplier; // To increase data size

    char _separator;

    // This is not working correctly. Does not compile
    std::unordered_map <std::string,
                        std::reference_wrapper<PMGD::Node> > _ref_map;

    PMGD::Node& get_node(PMGD::StringID tag,
                            const PMGD::PropertyPredicate& pp);

    // Parses "YYYY-MM-DD" formar to Jarvis Time.
    PMGD::Time parseDate(std::string date);

    void updateHistogram(NodeList& list, Histogram& histo, int pat_score);

    std::vector<std::string> readHeader(std::ifstream& filein);
    unsigned getNumberOfNodes(std::string tag);

    void parseLine(std::string line, std::string tag, std::string edge_tag,
                std::vector<std::string>& properties);

    void insertPatients();
    void insertImageSet();

    void insertDrugTreatment();
    void insertRadiationTreatment();
    void insertOMF();

    void insertFollowUp();
    void insertNewTumorEvent();
};
