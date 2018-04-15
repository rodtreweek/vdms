#pragma once
#define __STDC_FORMAT_MACROS

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <list>
#include <unordered_map>
#include <map>

#include "jarvis.h"

class TCIAMemSqlGen{

public:

    typedef std::list<std::reference_wrapper<Jarvis::Node>> NodeList;

    // < DrugName, <efective, not-efective> >
    typedef std::map<std::string,std::pair<int, int>> Histogram;

    // SQL Tables generator

    std::map<std::string, int> patient_pk;
    std::map<std::string, int> drug_pk;
    std::map<std::string, int> scans_pk;

    TCIAMemSqlGen(std::string database_path);

    void getGraphStats();
    void generateSQLTables();

    Jarvis::Graph* getGraph(){return _db;}

private:

    Jarvis::Graph* _db;
    std::string _db_path;

    int _rep_counter;

    char _separator;

    Jarvis::Node& get_node(Jarvis::StringID tag,
                           const Jarvis::PropertyPredicate& pp);

    unsigned getNumberOfNodes(std::string tag);

    std::vector<std::string> generate_properties(std::string tag);

    // This must be called first
    void generate_patients_table(std::string output_file);

    void generate_drug_table(std::string output_file);
    void generate_table(std::string tag);

    void generate_scans_table(std::string output_file);
    void generate_images_table(std::string output_file);
};
