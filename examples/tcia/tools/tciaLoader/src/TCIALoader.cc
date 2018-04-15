#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>

#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */

#include "util.h"
#include "neighbor.h"

#include "TCIALoader.h"
#include "Chrono.h"

// NODE TAGS
#define PATIENT_TAG         "patient"
#define FOLLOWUP_TAG        "follow_up"
#define RAD_TREATMENT_TAG   "rad_treatment"
#define DRUG_TREATMENT_TAG  "drug_treatment"
#define DRUG_OBJECT_TAG     "drug_object"
#define OMF_TAG             "omf"
#define NTE_TAG             "nte"
#define SCAN_TAG            "Scan"  // New Tumor Event

// EDGE TAGS
#define FOLLOW_UP_EDGE_TAG      "had_follow_up"
#define RAD_TREATMENT_EDGE_TAG  "had_r_trearment"
#define DRUG_TREATMENT_EDGE_TAG "had_d_trearment"
#define OMF_EDGE_TAG            "had_omf"
#define NTE_EDGE_TAG            "had_nte"
#define IMAGE_SET_EDGE_TAG      "had_scan"
#define PAT_DRUG_EDGE           "used_drug"

// OTHERS
#define PATIENT_ID_PROP     "bcr_patient_barc"
#define PATIENT_AGE_PROP    "age_at_initial"
#define KARNOFKY_SCORE_ID   "karnofsky_score"
#define DATE_FORM_ID        "form_completion_"
#define DRUG_NAME_ID        "drug_name"
#define RAD_NAME_ID         "radiation_type"
#define SCAN_HASH           "hash"

TCIALoader::TCIALoaderConfig::TCIALoaderConfig(){
}

TCIALoader::TCIALoader(std::string database_path, int multiplier):
    _db_path(database_path),
    _separator('\t'),
    _multiplier(multiplier)
{
    PMGD::Graph::Config pmgd_config;
    pmgd_config.default_region_size = 0x1000000000;

    try{
        _db = new PMGD::Graph(_db_path.c_str(),
            PMGD::Graph::ReadWrite, &pmgd_config);
    }
    catch(PMGD::Exception e) {
        print_exception(e);

        try{
            _db = new PMGD::Graph(_db_path.c_str(),
                PMGD::Graph::Create, &pmgd_config);

            PMGD::Transaction tx(*_db, PMGD::Transaction::ReadWrite);

            _db->create_index(PMGD::Graph::NodeIndex,
                PATIENT_TAG, PATIENT_ID_PROP, PMGD::PropertyType::String);
            _db->create_index(PMGD::Graph::NodeIndex,
                PATIENT_TAG, PATIENT_AGE_PROP, PMGD::PropertyType::Integer);
            _db->create_index(PMGD::Graph::NodeIndex,
                "AT:IMAGE", "name", PMGD::PropertyType::String);
            _db->create_index(PMGD::Graph::NodeIndex,
                "AT:IMAGE", "imgPath", PMGD::PropertyType::String);
            _db->create_index(PMGD::Graph::NodeIndex,
                SCAN_TAG, SCAN_HASH, PMGD::PropertyType::Integer);
            _db->create_index(PMGD::Graph::NodeIndex,
                DRUG_TREATMENT_TAG, DRUG_NAME_ID, PMGD::PropertyType::String);

            tx.commit();
        }
        catch(PMGD::Exception e) {
            print_exception(e);
            printf("FATAL ERROR OPENING/CREATING PMGD DATABASE\n");
        }
    }
}

void TCIALoader::insertDataset(TCIALoaderConfig& config)
{
    _config = config;

    try {
        for (int i = 0; i < _multiplier; ++i) {
            _rep_counter = i;
            // Load all files of the TCIA dataset
            if (!_config.patients.empty()) {

                insertPatients();
            }
            else{
                std::cout << "FATAL ERROR: NO PATIENTS" << std::endl;
                return;
            }

            // if (!_config.images_set.empty())
            //     insertImageSet();
            if (!_config.omf.empty())
                insertOMF();
            if (!_config.drug_treatment.empty())
                insertDrugTreatment();
            if (!_config.radiation_treatment.empty())
                insertRadiationTreatment();
            if (!_config.follow_up.empty())
                insertFollowUp();
            if (!_config.nte.empty())
                insertNewTumorEvent();
        }
    }
    catch (PMGD::Exception e) {
        print_exception(e);
        printf("FATAL ERROR ADDING ELEMENTS TO PMGD DATABASE\n");
    }
}

// For checking if the image node already exists
PMGD::Node& TCIALoader::get_node(PMGD::StringID tag,
                                    const PMGD::PropertyPredicate& pp)
{
    // Optimization, first thing to check if something fails :)
    // if ( _ref_map.count(pp.v1.string_value()) > 0 )
    //     return _ref_map[pp.v1.string_value()];

    PMGD::NodeIterator i = _db->get_nodes(tag, pp);
    if (i)
        return *i;

    PMGD::Node& n = _db->add_node(tag);
    n.set_property(pp.id,  pp.v1);

    // _ref_map[pp.v1.string_value()] = n;

    return n;
}

PMGD::Time TCIALoader::parseDate(std::string date_str)
{
    std::istringstream iss(date_str);
    std::string aux;

    // std::cout << date_str << std::endl;

    struct tm in_time;
    memset(&in_time, 0, sizeof(struct tm));

    std::getline(iss, aux, '-');
    in_time.tm_year = atoi(aux.c_str()) - 1900;
    std::getline(iss, aux, '-');
    in_time.tm_mon = atoi(aux.c_str());
    std::getline(iss, aux, '-');
    in_time.tm_mday = atoi(aux.c_str());

    // std::cout << in_time.tm_year << "/" << in_time.tm_mon << "/"
    //           << in_time.tm_mday << std::endl;

    return PMGD::Time(&in_time, 0, 0);
}

std::vector<std::string> TCIALoader::readHeader(std::ifstream& filein)
{
    std::vector<std::string> tokens;
    std::vector<std::string> properties;

    std::string line;
    std::string token;

    std::getline(filein, line); // read header
    std::istringstream iss_prop(line);

    while(std::getline(iss_prop, token, _separator)) {
        // property names should be less than 16 char TODO
        // token.resize(16);
        properties.push_back(token);
        //std::cout << token << std::endl;
    }

    std::getline(filein, line); // read extra header
    std::getline(filein, line); // read extra header

    return properties;
}

void TCIALoader::parseLine(std::string line, std::string tag,
                std::string edge_tag,
                std::vector<std::string>& properties)
{
    std::vector<std::string > tokens;

    tokens.clear();
    std::istringstream iss_line(line);
    std::string token;

    // Read all values in a line
    while(std::getline(iss_line, token, _separator)) {
        tokens.push_back(token);
    }

    PMGD::Transaction tx(*_db, PMGD::Transaction::ReadWrite);

    std::string pat_id = tokens[1];
    pat_id += "-" + std::to_string(_rep_counter);

    // ID will be field ** bcr_patient_barcode **
    PMGD::PropertyPredicate pps1(PATIENT_ID_PROP,
                PMGD::PropertyPredicate::Eq, pat_id.c_str());
    PMGD::NodeIterator pat = _db->get_nodes(PATIENT_TAG, pps1);

    if (!pat) {
        std::cout << "ERROR - PATIENT NOT FOUND" << std::endl;
        exit(0);
    }
    PMGD::Node &newnode = _db->add_node(tag.c_str());

    assert(properties.size() == tokens.size());
    for (int i = 0; i < properties.size(); ++i){
        std::string prop_name = properties[i];
        std::size_t found = prop_name.find("date");
        prop_name = prop_name.substr(0,16);
        // std::cout << prop_name << std::endl;

        if (found !=std::string::npos) { // Propery is a date
            newnode.set_property(prop_name.c_str(), parseDate(tokens[i]));
        }
        else{
            if (i == 1) // This is the patient id, which is changed
                newnode.set_property(prop_name.c_str(), pat_id);
            else
                newnode.set_property(prop_name.c_str(), tokens[i]);
        }
    }

    // if (tag == DRUG_TREATMENT_TAG) {
    //     PMGD::PropertyPredicate pps2(DRUG_NAME_ID,
    //                                     PMGD::PropertyPredicate::Eq,
    //                                     newnode.get_property(DRUG_NAME_ID));
    //     PMGD::Node &drug = get_node(DRUG_OBJECT_TAG, pps2);
    //     PMGD::Edge &e = _db->add_edge(drug, *pat, PAT_DRUG_EDGE);
    //     std::cout << "Edge" << std::endl;
    // }

    PMGD::Edge &e = _db->add_edge(newnode, *pat, edge_tag.c_str());
    //e.set_property("testprop", "empy test prop");

    tx.commit();
}

unsigned TCIALoader::getNumberOfNodes(std::string tag)
{
    unsigned counter = 0;
    PMGD::Transaction tx(*_db, PMGD::Transaction::ReadWrite);
    for (PMGD::NodeIterator i = _db->get_nodes(tag.c_str());
                                i; i.next()) {
        counter++;
    }
    tx.commit();

    return counter;
}

void TCIALoader::insertPatients()
{
    srand(time(NULL));
    std::ifstream filein(_config.patients);
    std::ifstream filelname("data/lastnames.txt"); // To personify a little :)

    std::vector<std::string> properties = readHeader(filein);
    std::vector<std::string> tokens;
    std::vector<std::string> lastnames;

    std::string line;
    std::string token;

    while(std::getline(filelname, line)) {
        line[0] = std::toupper(line[0]);
        lastnames.push_back(line);
    }

    while (std::getline(filein, line)) {

        tokens.clear();
        std::istringstream iss_line(line);

        // Read all values in a line
        while(std::getline(iss_line, token, _separator)) {
            tokens.push_back(token);
        }

        PMGD::Transaction tx(*_db, PMGD::Transaction::ReadWrite);

        // ID will be field ** bcr_patient_barcode **
        PMGD::PropertyPredicate pps1(PATIENT_ID_PROP,
                    PMGD::PropertyPredicate::Eq, tokens[1].c_str());
        PMGD::NodeIterator i = _db->get_nodes(PATIENT_TAG, pps1);

        if (true) { // asumes not repeated patient
            PMGD::Node &npatient = _db->add_node(PATIENT_TAG);

            assert(properties.size() == tokens.size());
            for (int i = 0; i < properties.size(); ++i){
                std::string prop_name = properties[i];

                std::size_t found_id   = prop_name.find(PATIENT_ID_PROP);
                std::size_t found_date = prop_name.find("date");
                std::size_t found_age  = prop_name.find(PATIENT_AGE_PROP);
                prop_name = prop_name.substr(0,16);

                if(found_id == 0){
                    std::string id = tokens[i];
                    id += "-" + std::to_string(_rep_counter);
                    npatient.set_property(prop_name.c_str(),
                                            id.c_str());
                }
                else if (found_date !=std::string::npos) { // Property is a date
                    npatient.set_property(prop_name.c_str(),
                                            parseDate(tokens[i]));
                }
                else if (found_age !=std::string::npos) { // Property is age
                    npatient.set_property(PATIENT_AGE_PROP,
                                            std::stoi(tokens[i]));
                }
                else
                    npatient.set_property(prop_name.c_str(), tokens[i]);
            }
            npatient.set_property("first_name",
                                  lastnames[rand()%lastnames.size()]);
            npatient.set_property("last_name",
                                  lastnames[rand()%lastnames.size()]);
            // std::cout << tokens[1] << std::endl;
        }

        tx.commit();
    }
}

void TCIALoader::insertImageSet()
{
    std::ifstream filein(_config.images_set);

    std::vector<std::string> tokens;
    std::vector<std::string> properties;

    std::string line;
    std::string token;

    // READ HEADER
    std::getline(filein, line); // read header
    std::istringstream iss_prop(line);
    // std::cout << "******************" << std::endl;
    // std::cout << line << std::endl;
    // std::cout << "******************" << std::endl;

    while(std::getline(iss_prop, token, ',')) {
        // property names should be less than 16 char TODO
        properties.push_back(token);
        // std::cout << token << std::endl;
    }

    // READ ROWS
    while (std::getline(filein, line)) {

        tokens.clear();
        std::istringstream iss_line(line);

        // Read all values in a line
        while(std::getline(iss_line, token, ',')) {
            tokens.push_back(token);
        }

        PMGD::Transaction tx(*_db, PMGD::Transaction::ReadWrite);

        // ID will be field ** bcr_patient_barcode **
        PMGD::PropertyPredicate pps1(PATIENT_ID_PROP,
                    PMGD::PropertyPredicate::Eq, tokens[1].c_str());
        PMGD::NodeIterator i = _db->get_nodes(PATIENT_TAG, pps1);

        if (i) {
        // if (i && tokens[5].compare(std::string("INVALID"))!=0 ) {

            PMGD::Node &nimageset = _db->add_node(SCAN_TAG);

            assert(properties.size() == tokens.size());
            for (int i = 0; i < properties.size(); ++i){
                std::string prop_name = properties[i];
                std::size_t found = prop_name.find("date");
                prop_name = prop_name.substr(0,16);
                // std::cout << prop_name << std::endl;

                if (found !=std::string::npos) { // Propery is a date
                    nimageset.set_property(prop_name.c_str(),
                                            parseDate(tokens[i]));
                }
                else
                    nimageset.set_property(prop_name.c_str(), tokens[i]);
            }

            PMGD::Edge &e = _db->add_edge(nimageset, *i, IMAGE_SET_EDGE_TAG);
        }
        else{

            std::cout << "ImageSet: PATIENT NOT FOUND: "
                      << tokens[0].c_str() << " "
                      << tokens[1].c_str() << std::endl;
        }
        tx.commit();
    }
}

void TCIALoader::insertFollowUp()
{
    std::ifstream filein(_config.follow_up);

    std::vector<std::string> properties = readHeader(filein);
    std::string line;

    while (std::getline(filein, line)) {
        parseLine(line, FOLLOWUP_TAG, FOLLOW_UP_EDGE_TAG, properties);
    }
}
void TCIALoader::insertNewTumorEvent()
{
    std::ifstream filein(_config.nte);

    std::vector<std::string> properties = readHeader(filein);
    std::string line;

    while (std::getline(filein, line)) {
        parseLine(line, NTE_TAG, NTE_EDGE_TAG, properties);
    }
}

void TCIALoader::insertOMF()
{
    std::ifstream filein(_config.omf);

    std::vector<std::string> properties = readHeader(filein);
    std::string line;

    while (std::getline(filein, line)) {
        parseLine(line, OMF_TAG, OMF_EDGE_TAG, properties);
    }
}

void TCIALoader::insertDrugTreatment()
{
    std::ifstream filein(_config.drug_treatment);

    std::vector<std::string> properties = readHeader(filein);
    std::string line;

    while (std::getline(filein, line)) {
        parseLine(line, DRUG_TREATMENT_TAG,
                    DRUG_TREATMENT_EDGE_TAG, properties);
    }
}

void TCIALoader::insertRadiationTreatment()
{
    std::ifstream filein(_config.radiation_treatment);

    std::vector<std::string> properties = readHeader(filein);
    std::string line;

    while (std::getline(filein, line)) {
        parseLine(line, RAD_TREATMENT_TAG,
                    RAD_TREATMENT_EDGE_TAG, properties);
    }
}

void TCIALoader::getGraphStats()
{
    std::cout << "Patients Inserted: "
                << getNumberOfNodes(PATIENT_TAG) << std::endl;

    std::cout << "RadiationTreatment Inserted: " <<
                getNumberOfNodes(RAD_TREATMENT_TAG) << std::endl;

    std::cout << "DrugTreatment Inserted: " <<
                getNumberOfNodes(DRUG_TREATMENT_TAG) << std::endl;

    std::cout << "OMF Inserted: " <<
                getNumberOfNodes(OMF_TAG) << std::endl;

    std::cout << "Folow UP Inserted: " <<
                getNumberOfNodes(FOLLOWUP_TAG) << std::endl;

    std::cout << "NTE Inserted: " <<
                getNumberOfNodes(NTE_TAG) << std::endl;

    std::cout << "Image Sets: " <<
                getNumberOfNodes(SCAN_TAG) << std::endl;
}
