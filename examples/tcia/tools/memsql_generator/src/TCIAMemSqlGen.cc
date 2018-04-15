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

#include "TCIAMemSqlGen.h"

// NODE TAGS
#define PATIENT_TAG         "patient"
#define FOLLOWUP_TAG        "follow_up"
#define RAD_TREATMENT_TAG   "rad_treatment"
#define DRUG_TREATMENT_TAG  "drug_treatment"
#define DRUG_OBJECT_TAG     "drug_object"
#define OMF_TAG             "omf"
#define NTE_TAG             "nte"
#define SCAN_TAG            "Scan"

#define AT_IMAGE            "AT:IMAGE"
#define AT_IM_EDGE          "AT:IMG_LINK"

// EDGE TAGS
#define FOLLOW_UP_EDGE_TAG      "had_follow_up"
#define RAD_TREATMENT_EDGE_TAG  "had_r_trearment"
#define DRUG_TREATMENT_EDGE_TAG "had_d_trearment"
#define OMF_EDGE_TAG            "had_omf"
#define NTE_EDGE_TAG            "had_nte"
#define SCAN_EDGE_TAG           "had_scan"
#define PAT_DRUG_EDGE           "used_drug"

// OTHERS
#define PATIENT_ID_PROP     "bcr_patient_barc"
#define SCAN_HASH_PROP      "hash"
#define PATIENT_AGE_PROP    "age_at_initial"
#define KARNOFKY_SCORE_ID   "karnofsky_score"
#define DATE_FORM_ID        "form_completion_"
#define DRUG_NAME_ID        "drug_name"
#define RAD_NAME_ID         "radiation_type"

#define IMAGES_PER_SCAN 150

TCIAMemSqlGen::TCIAMemSqlGen(std::string database_path):
    _db_path(database_path),
    _separator('\t')
{
    Jarvis::Graph::Config pmgd_config;
    pmgd_config.default_region_size = 0x1000000000;

    try{
        _db = new Jarvis::Graph(_db_path.c_str(),
            Jarvis::Graph::ReadWrite, &pmgd_config);
    }
    catch(Jarvis::Exception e) {
        print_exception(e);

        try{
            _db = new Jarvis::Graph(_db_path.c_str(),
                Jarvis::Graph::Create, &pmgd_config);

            Jarvis::Transaction tx(*_db, Jarvis::Transaction::ReadWrite);

            _db->create_index(Jarvis::Graph::NodeIndex,
                PATIENT_TAG, PATIENT_ID_PROP, Jarvis::PropertyType::String);
            _db->create_index(Jarvis::Graph::NodeIndex,
                "AT:IMAGE", "name", Jarvis::PropertyType::String);
            tx.commit();
        }
        catch(Jarvis::Exception e) {
            print_exception(e);
            printf("FATAL ERROR OPENING/CREATING PMGD DATABASE\n");
        }
    }
}

// For checking if the image node already exists
Jarvis::Node& TCIAMemSqlGen::get_node(Jarvis::StringID tag,
                                    const Jarvis::PropertyPredicate& pp)
{
    // Optimization, first thing to check if something fails :)
    // if ( _ref_map.count(pp.v1.string_value()) > 0 )
    //     return _ref_map[pp.v1.string_value()];

    Jarvis::NodeIterator i = _db->get_nodes(tag, pp);
    if (i)
        return *i;

    Jarvis::Node& n = _db->add_node(tag);
    n.set_property(pp.id,  pp.v1);

    // _ref_map[pp.v1.string_value()] = n;

    return n;
}

unsigned TCIAMemSqlGen::getNumberOfNodes(std::string tag)
{
    unsigned counter = 0;
    Jarvis::Transaction tx(*_db, Jarvis::Transaction::ReadWrite);
    for (Jarvis::NodeIterator i = _db->get_nodes(tag.c_str());
                                i; i.next()) {
        counter++;
    }
    tx.commit();

    return counter;
}

void TCIAMemSqlGen::getGraphStats()
{
    std::cout << "Patients: "
                << getNumberOfNodes(PATIENT_TAG) << std::endl;

    std::cout << "RadiationTreatment: " <<
                getNumberOfNodes(RAD_TREATMENT_TAG) << std::endl;

    std::cout << "DrugTreatment: " <<
                getNumberOfNodes(DRUG_TREATMENT_TAG) << std::endl;

    std::cout << "OMF: " <<
                getNumberOfNodes(OMF_TAG) << std::endl;

    std::cout << "Folow UP: " <<
                getNumberOfNodes(FOLLOWUP_TAG) << std::endl;

    std::cout << "NTE: " <<
                getNumberOfNodes(NTE_TAG) << std::endl;

    std::cout << "Image Sets: " <<
                getNumberOfNodes(SCAN_TAG) << std::endl;
}

// SQL Tables generator

std::vector<std::string> TCIAMemSqlGen::generate_properties(std::string tag)
{
    std::map<std::string, int> prop_counter;

    Jarvis::Transaction tx(*_db, Jarvis::Transaction::ReadWrite);
    for (Jarvis::NodeIterator i = _db->get_nodes(tag.c_str()); i; i.next()) {

        for (Jarvis::PropertyIterator p= i->get_properties(); p; p.next()) {
            prop_counter[p->id().name()] = prop_counter[p->id().name()] + 1;
        }
    }
    tx.commit();

    std::vector<std::string> ret;

    for (auto& prop : prop_counter) {
        if (prop.second == getNumberOfNodes(tag)) {
            ret.push_back(prop.first);
        }
    }

    return ret;
}

// This method will go over all the nodes with a particular tag
// and generate a vector with the properties that appear in
// all the nodes (because this dataset has different properties for
// some entities of the same kind)
void TCIAMemSqlGen::generate_patients_table(std::string output_file)
{
    std::ofstream out(output_file);
    unsigned counter_pk = 0;

    std::vector<std::string> props = generate_properties(PATIENT_TAG);

    std::ofstream out_k(output_file + "_keys.txt");
    bool key_flag = true;
    // for (auto& prop: props) {
    //     out_k << prop << std::endl;
    // }

    Jarvis::Transaction tx(*_db, Jarvis::Transaction::ReadOnly);
    for (Jarvis::NodeIterator i = _db->get_nodes(PATIENT_TAG);
         i; i.next(), counter_pk++) {

        std::string pat_id = i->get_property(PATIENT_ID_PROP).string_value();
        patient_pk[pat_id] = counter_pk;

        out << counter_pk;
        if (key_flag)
        out_k << "id" << " INT," << std::endl;

        int counter = 0;
        for (auto& prop : props) {
            if (prop == "tumor_tissue_sit")
                continue;

            Jarvis::Property p = i->get_property(prop.c_str());
            if (p.type() == Jarvis::PropertyType::String) {
                std::string prop_str = p.string_value();
                out << "\t" << prop_str;
                if (key_flag)
                    out_k << prop << " VARCHAR(128)," << std::endl;
            }
            if (p.type() == Jarvis::PropertyType::Integer) {
                int val = p.int_value();
                out << "\t" << val;
                if (key_flag)
                    out_k << prop << " INT," << std::endl;
            }
        }
        key_flag = false;
        out << std::endl;
    }
    tx.commit();
}

void TCIAMemSqlGen::generate_drug_table(std::string output_file)
{
    std::ofstream out(output_file);
    unsigned counter_pk = 0;

    std::vector<std::string> props = generate_properties(DRUG_OBJECT_TAG);

    Jarvis::Transaction tx(*_db, Jarvis::Transaction::ReadOnly);
    for (Jarvis::NodeIterator i = _db->get_nodes(DRUG_TREATMENT_TAG);
         i; i.next(), counter_pk++) {

        std::string drug_id = i->get_property(DRUG_NAME_ID).string_value();
        drug_pk[drug_id] = counter_pk;

        // out << counter_pk << "\t" << drug_id;
        // out << std::endl;
    }
    tx.commit();

    counter_pk = 0;
    for (auto& drug : drug_pk) {
        out << counter_pk++ << "\t" << drug.first;
        out << std::endl;
        drug_pk[drug.first] = counter_pk;
    }
}

void TCIAMemSqlGen::generate_table(std::string tag)
{
    std::ofstream out(tag + "_table.txt");
    unsigned counter_pk = 0;

    std::vector<std::string> props = generate_properties(tag);

    std::ofstream out_k(tag + "_keys.txt");
    bool key_flag = true;

    Jarvis::Transaction tx(*_db, Jarvis::Transaction::ReadOnly);
    for (Jarvis::NodeIterator i = _db->get_nodes(tag.c_str());
         i; i.next(), counter_pk++) {

        std::string pat_id = i->get_property(PATIENT_ID_PROP).string_value();

        out << counter_pk << "\t" << patient_pk[pat_id];
        if (key_flag) {
            out_k << "id" << " INT," << std::endl;
            out_k << "pat_id" << " INT," << std::endl;
        }

        int counter = 0;
        for (auto& prop : props) {
            if (prop == "therapy_regimen_" || // Only for rad
                prop == "performance_stat")   // only for followup
                continue;

            Jarvis::Property p = i->get_property(prop.c_str());
            if (p.type() == Jarvis::PropertyType::String) {
                std::string prop_str = p.string_value();
                out << "\t" << prop_str;
                if (key_flag)
                    out_k << prop << " VARCHAR(128)," << std::endl;
            }
            if (p.type() == Jarvis::PropertyType::Integer) {
                int val = p.int_value();
                out << "\t" << val;
                if (key_flag)
                    out_k << prop << " INT," << std::endl;
            }
        }

        key_flag = false;
        out << std::endl;
    }
    tx.commit();
}

void TCIAMemSqlGen::generate_images_table(std::string output_file)
{
    std::ofstream out(output_file);
    unsigned counter_pk = 0;

    std::vector<std::string> props = generate_properties(AT_IMAGE);

    std::ofstream out_k(output_file + "_table_keys.txt");
    bool key_flag = true;

    Jarvis::Transaction tx(*_db, Jarvis::Transaction::ReadWrite);
    for (Jarvis::NodeIterator scan_node = _db->get_nodes(SCAN_TAG);
         scan_node; scan_node.next()) {

        std::string scan_id = std::to_string(scan_node->get_property(SCAN_HASH_PROP).int_value());

        EdgeConstraint constr;
        constr.tag = AT_IM_EDGE;
        constr.dir = Jarvis::Any;
        Jarvis::NodeIterator imgs = get_neighbors(*scan_node, constr);

        for (Jarvis::NodeIterator imgs_it = imgs; imgs_it;
                    imgs_it.next(), counter_pk++) {

            out << counter_pk << "\t" << scans_pk[scan_id];
            if (key_flag) {
                out_k << "id" << " INT," << std::endl;
                out_k << "scan_id" << " INT," << std::endl;
            }

            for (auto& prop : props) {
                if (prop == "therapy_regimen_" || // Only for rad
                    prop == "performance_stat")   // only for followup
                    continue;

                Jarvis::Property p = imgs_it->get_property(prop.c_str());
                if (p.type() == Jarvis::PropertyType::String) {
                    std::string prop_str = p.string_value();
                    out << "\t" << prop_str;
                    if (key_flag)
                        out_k << prop << " VARCHAR(128)," << std::endl;
                }
                if (p.type() == Jarvis::PropertyType::Integer) {
                    int val = p.int_value();
                    out << "\t" << val;
                    if (key_flag)
                        out_k << prop << " INT," << std::endl;
                }
            }

            key_flag = false;
            out << std::endl;
        }
    }
    tx.commit();
}

void TCIAMemSqlGen::generate_scans_table(std::string output_file)
{
    std::ofstream out(output_file);
    unsigned counter_pk = 0;

    std::vector<std::string> props = generate_properties(SCAN_TAG);

    std::ofstream out_k("scans_table_keys.txt");
    bool key_flag = true;

    Jarvis::Transaction tx(*_db, Jarvis::Transaction::ReadWrite);
    for (Jarvis::NodeIterator pat_node = _db->get_nodes(PATIENT_TAG);
        pat_node; pat_node.next()) {

        std::string pat_id = pat_node->
                             get_property(PATIENT_ID_PROP).string_value();

        EdgeConstraint constr;
        constr.tag = SCAN_EDGE_TAG;
        constr.dir = Jarvis::Any;
        Jarvis::NodeIterator scans = get_neighbors(*pat_node, constr);

        for (Jarvis::NodeIterator scan_it = scans; scan_it;
                    scan_it.next(), counter_pk++) {

            out << counter_pk << "\t" << patient_pk[pat_id];
            std::string scan_hash = std::to_string(scan_it->get_property(SCAN_HASH_PROP).int_value());
            scans_pk[scan_hash] = counter_pk;

            if (key_flag) {
                out_k << "id" << " INT," << std::endl;
                out_k << "pat_id" << " INT," << std::endl;
            }

            for (auto& prop : props) {
                if (prop == "therapy_regimen_" || // Only for rad
                    prop == "performance_stat")   // only for followup
                    continue;

                Jarvis::Property p = scan_it->get_property(prop.c_str());
                if (p.type() == Jarvis::PropertyType::String) {
                    std::string prop_str = p.string_value();
                    out << "\t" << prop_str;
                    if (key_flag)
                        out_k << prop << " VARCHAR(128)," << std::endl;
                }
                if (p.type() == Jarvis::PropertyType::Integer) {
                    int val = p.int_value();
                    out << "\t" << val;
                    if (key_flag)
                        out_k << prop << " INT," << std::endl;
                }
            }

            key_flag = false;
            out << std::endl;
        }
    }
    tx.commit();
}

void TCIAMemSqlGen::generateSQLTables()
{
    std::string path(_db_path);
    std::string filename;

    size_t pos = path.find_last_of("/");
    if(pos != std::string::npos)
        filename.assign(path.begin() + pos + 1, path.end());
    else
        filename = path;

    std::cout << "converting " << filename << "..." << std::endl;

    int r;
    std::string cmd;

    cmd = "rm -r " + filename + "_tables";
    r = system(cmd.c_str());

    cmd = "mkdir " + filename + "_tables";
    r = system(cmd.c_str());

    generate_patients_table("patients_table.txt");
    generate_scans_table("scans_table.txt");
    generate_images_table("images_table.txt");

    generate_drug_table("drugs_table.txt");
    generate_table(DRUG_TREATMENT_TAG);
    generate_table(RAD_TREATMENT_TAG);
    generate_table(FOLLOWUP_TAG);
    generate_table(OMF_TAG);


    cmd = "mv *.txt " + filename + "_tables";
    r = system(cmd.c_str());
}
