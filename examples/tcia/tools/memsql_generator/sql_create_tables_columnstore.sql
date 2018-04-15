DROP DATABASE tcia_1;
DROP DATABASE tcia_10;
DROP DATABASE tcia_100;

CREATE DATABASE tcia_1;
USE tcia_1;

CREATE TABLE PATIENTS ( id INT,
                        age_at_initial INT,
                        bcr_patient_barc VARCHAR(128),
                        bcr_patient_uuid VARCHAR(128),
                        birth_days_to VARCHAR(128),
                        days_to_initial_ VARCHAR(128),
                        death_days_to VARCHAR(128),
                        disease_code VARCHAR(128),
                        ecog_score VARCHAR(128),
                        ethnicity VARCHAR(128),
                        first_name VARCHAR(128),
                        gender VARCHAR(128),
                        history_neoadjuv VARCHAR(128),
                        history_other_ma VARCHAR(128),
                        icd_10 VARCHAR(128),
                        icd_o_3_histolog VARCHAR(128),
                        icd_o_3_site VARCHAR(128),
                        informed_consent VARCHAR(128),
                        initial_patholog VARCHAR(128),
                        karnofsky_score VARCHAR(128),
                        last_contact_day VARCHAR(128),
                        last_name VARCHAR(128),
                        new_tumor_event_ VARCHAR(128),
                        patient_id VARCHAR(128),
                        performance_stat VARCHAR(128),
                        project_code VARCHAR(128),
                        prospective_coll VARCHAR(128),
                        race VARCHAR(128),
                        radiation_treatm VARCHAR(128),
                        retrospective_co VARCHAR(128),
                        tissue_source_si VARCHAR(128),
                        treatment_outcom VARCHAR(128),
                        tumor_status VARCHAR(128),
                        vital_status VARCHAR(128),
                        -- KEY(id),
                        KEY (`age_at_initial`) USING CLUSTERED COLUMNSTORE );

CREATE TABLE DRUG_TREATMENT(id INT,
                            pat_id INT,
                            bcr_drug_barcode VARCHAR(128),
                            bcr_drug_uuid VARCHAR(128),
                            bcr_patient_barc VARCHAR(128),
                            bcr_patient_uuid VARCHAR(128),
                            clinical_trial_d VARCHAR(128),
                            drug_name VARCHAR(128),
                            pharma_adjuvant_ VARCHAR(128),
                            pharma_type_othe VARCHAR(128),
                            pharmaceutical_t VARCHAR(128),
                            prescribed_dose VARCHAR(128),
                            regimen_number VARCHAR(128),
                            route_of_adminis VARCHAR(128),
                            therapy_regimen VARCHAR(128),
                            total_dose VARCHAR(128),
                            treatment_best_r VARCHAR(128),
                            tx_on_clinical_t VARCHAR(128),
                            -- KEY(id),
                            KEY (`drug_name`) USING CLUSTERED COLUMNSTORE );

CREATE TABLE RADIATION_TREATMENT(id INT,
                                pat_id INT,
                                bcr_patient_barc VARCHAR(128),
                                bcr_patient_uuid VARCHAR(128),
                                bcr_radiation_ba VARCHAR(128),
                                bcr_radiation_uu VARCHAR(128),
                                course_number VARCHAR(128),
                                radiation_adjuva VARCHAR(128),
                                radiation_therap VARCHAR(128),
                                radiation_total_ VARCHAR(128),
                                radiation_type VARCHAR(128),
                                radiation_type_o VARCHAR(128),
                                therapy_regimen VARCHAR(128),
                                treatment_best_r VARCHAR(128),
                                KEY(id));

CREATE TABLE FOLLOW_UP( id INT,
                        pat_id INT,
                        bcr_followup_bar VARCHAR(128),
                        bcr_followup_uui VARCHAR(128),
                        bcr_patient_barc VARCHAR(128),
                        bcr_patient_uuid VARCHAR(128),
                        death_days_to VARCHAR(128),
                        ecog_score VARCHAR(128),
                        followup_lost_to VARCHAR(128),
                        followup_reason VARCHAR(128),
                        karnofsky_score VARCHAR(128),
                        last_contact_day VARCHAR(128),
                        new_tumor_event_ VARCHAR(128),
                        radiation_treatm VARCHAR(128),
                        treatment_outcom VARCHAR(128),
                        tumor_status VARCHAR(128),
                        vital_status VARCHAR(128),
                        KEY(id));

CREATE TABLE IMAGES(id INT,
                    scan_id INT,
                    imgPath VARCHAR(128),
                    name VARCHAR(128),
                    -- KEY(id),
                    KEY (`scan_id`) USING CLUSTERED COLUMNSTORE );

CREATE TABLE SCANS(id INT,
                    pat_id INT,
                    hash INT,
                    KEY(id));

LOAD DATA INFILE '/home/luisremi/hls/tools/memsql_generator/hls-graph_tables_1/patients_table.txt'
    INTO TABLE PATIENTS;
LOAD DATA INFILE '/home/luisremi/hls/tools/memsql_generator/hls-graph_tables_1/drug_treatment_table.txt'
    INTO TABLE DRUG_TREATMENT;
LOAD DATA INFILE '/home/luisremi/hls/tools/memsql_generator/hls-graph_tables_1/rad_treatment_table.txt'
    INTO TABLE RADIATION_TREATMENT;
LOAD DATA INFILE '/home/luisremi/hls/tools/memsql_generator/hls-graph_tables_1/follow_up_table.txt'
    INTO TABLE FOLLOW_UP;
LOAD DATA INFILE '/home/luisremi/hls/tools/memsql_generator/hls-graph_tables_1/images_table.txt'
    INTO TABLE IMAGES;
LOAD DATA INFILE '/home/luisremi/hls/tools/memsql_generator/hls-graph_tables_1/scans_table.txt'
    INTO TABLE SCANS;

-- CREATE INDEX pat_id_idx     ON PATIENTS (id);
-- CREATE INDEX par_age_idx    ON PATIENTS (age_at_initial);
-- CREATE INDEX drug_name_idx  ON DRUG_TREATMENT (drug_name);
-- CREATE INDEX scan_id_idx    ON SCANS (hash);
-- CREATE INDEX images_id_idx  ON IMAGES (id);


CREATE DATABASE tcia_10;
USE tcia_10;

CREATE TABLE PATIENTS ( id INT,
                        age_at_initial INT,
                        bcr_patient_barc VARCHAR(128),
                        bcr_patient_uuid VARCHAR(128),
                        birth_days_to VARCHAR(128),
                        days_to_initial_ VARCHAR(128),
                        death_days_to VARCHAR(128),
                        disease_code VARCHAR(128),
                        ecog_score VARCHAR(128),
                        ethnicity VARCHAR(128),
                        first_name VARCHAR(128),
                        gender VARCHAR(128),
                        history_neoadjuv VARCHAR(128),
                        history_other_ma VARCHAR(128),
                        icd_10 VARCHAR(128),
                        icd_o_3_histolog VARCHAR(128),
                        icd_o_3_site VARCHAR(128),
                        informed_consent VARCHAR(128),
                        initial_patholog VARCHAR(128),
                        karnofsky_score VARCHAR(128),
                        last_contact_day VARCHAR(128),
                        last_name VARCHAR(128),
                        new_tumor_event_ VARCHAR(128),
                        patient_id VARCHAR(128),
                        performance_stat VARCHAR(128),
                        project_code VARCHAR(128),
                        prospective_coll VARCHAR(128),
                        race VARCHAR(128),
                        radiation_treatm VARCHAR(128),
                        retrospective_co VARCHAR(128),
                        tissue_source_si VARCHAR(128),
                        treatment_outcom VARCHAR(128),
                        tumor_status VARCHAR(128),
                        vital_status VARCHAR(128),
                        -- KEY(id),
                        KEY (`age_at_initial`) USING CLUSTERED COLUMNSTORE );

CREATE TABLE DRUG_TREATMENT(id INT,
                            pat_id INT,
                            bcr_drug_barcode VARCHAR(128),
                            bcr_drug_uuid VARCHAR(128),
                            bcr_patient_barc VARCHAR(128),
                            bcr_patient_uuid VARCHAR(128),
                            clinical_trial_d VARCHAR(128),
                            drug_name VARCHAR(128),
                            pharma_adjuvant_ VARCHAR(128),
                            pharma_type_othe VARCHAR(128),
                            pharmaceutical_t VARCHAR(128),
                            prescribed_dose VARCHAR(128),
                            regimen_number VARCHAR(128),
                            route_of_adminis VARCHAR(128),
                            therapy_regimen VARCHAR(128),
                            total_dose VARCHAR(128),
                            treatment_best_r VARCHAR(128),
                            tx_on_clinical_t VARCHAR(128),
                            -- KEY(id),
                            KEY (`drug_name`) USING CLUSTERED COLUMNSTORE );

CREATE TABLE RADIATION_TREATMENT(id INT,
                                pat_id INT,
                                bcr_patient_barc VARCHAR(128),
                                bcr_patient_uuid VARCHAR(128),
                                bcr_radiation_ba VARCHAR(128),
                                bcr_radiation_uu VARCHAR(128),
                                course_number VARCHAR(128),
                                radiation_adjuva VARCHAR(128),
                                radiation_therap VARCHAR(128),
                                radiation_total_ VARCHAR(128),
                                radiation_type VARCHAR(128),
                                radiation_type_o VARCHAR(128),
                                therapy_regimen VARCHAR(128),
                                treatment_best_r VARCHAR(128),
                                KEY(id));

CREATE TABLE FOLLOW_UP( id INT,
                        pat_id INT,
                        bcr_followup_bar VARCHAR(128),
                        bcr_followup_uui VARCHAR(128),
                        bcr_patient_barc VARCHAR(128),
                        bcr_patient_uuid VARCHAR(128),
                        death_days_to VARCHAR(128),
                        ecog_score VARCHAR(128),
                        followup_lost_to VARCHAR(128),
                        followup_reason VARCHAR(128),
                        karnofsky_score VARCHAR(128),
                        last_contact_day VARCHAR(128),
                        new_tumor_event_ VARCHAR(128),
                        radiation_treatm VARCHAR(128),
                        treatment_outcom VARCHAR(128),
                        tumor_status VARCHAR(128),
                        vital_status VARCHAR(128),
                        KEY(id));

CREATE TABLE IMAGES(id INT,
                    scan_id INT,
                    imgPath VARCHAR(128),
                    name VARCHAR(128),
                    -- KEY(id),
                    KEY (`scan_id`) USING CLUSTERED COLUMNSTORE );

CREATE TABLE SCANS(id INT,
                    pat_id INT,
                    hash INT,
                    KEY(id));

LOAD DATA INFILE '/home/luisremi/hls/tools/memsql_generator/hls-graph_tables_10/patients_table.txt'
    INTO TABLE PATIENTS;
LOAD DATA INFILE '/home/luisremi/hls/tools/memsql_generator/hls-graph_tables_10/drug_treatment_table.txt'
    INTO TABLE DRUG_TREATMENT;
LOAD DATA INFILE '/home/luisremi/hls/tools/memsql_generator/hls-graph_tables_10/rad_treatment_table.txt'
    INTO TABLE RADIATION_TREATMENT;
LOAD DATA INFILE '/home/luisremi/hls/tools/memsql_generator/hls-graph_tables_10/follow_up_table.txt'
    INTO TABLE FOLLOW_UP;
LOAD DATA INFILE '/home/luisremi/hls/tools/memsql_generator/hls-graph_tables_10/images_table.txt'
    INTO TABLE IMAGES;
LOAD DATA INFILE '/home/luisremi/hls/tools/memsql_generator/hls-graph_tables_10/scans_table.txt'
    INTO TABLE SCANS;

-- CREATE INDEX pat_id_idx     ON PATIENTS (id);
-- CREATE INDEX par_age_idx    ON PATIENTS (age_at_initial);
-- CREATE INDEX drug_name_idx  ON DRUG_TREATMENT (drug_name);
-- CREATE INDEX scan_id_idx    ON SCANS (hash);
-- CREATE INDEX images_id_idx  ON IMAGES (id);

CREATE DATABASE tcia_100;
USE tcia_100;

CREATE TABLE PATIENTS ( id INT,
                        age_at_initial INT,
                        bcr_patient_barc VARCHAR(128),
                        bcr_patient_uuid VARCHAR(128),
                        birth_days_to VARCHAR(128),
                        days_to_initial_ VARCHAR(128),
                        death_days_to VARCHAR(128),
                        disease_code VARCHAR(128),
                        ecog_score VARCHAR(128),
                        ethnicity VARCHAR(128),
                        first_name VARCHAR(128),
                        gender VARCHAR(128),
                        history_neoadjuv VARCHAR(128),
                        history_other_ma VARCHAR(128),
                        icd_10 VARCHAR(128),
                        icd_o_3_histolog VARCHAR(128),
                        icd_o_3_site VARCHAR(128),
                        informed_consent VARCHAR(128),
                        initial_patholog VARCHAR(128),
                        karnofsky_score VARCHAR(128),
                        last_contact_day VARCHAR(128),
                        last_name VARCHAR(128),
                        new_tumor_event_ VARCHAR(128),
                        patient_id VARCHAR(128),
                        performance_stat VARCHAR(128),
                        project_code VARCHAR(128),
                        prospective_coll VARCHAR(128),
                        race VARCHAR(128),
                        radiation_treatm VARCHAR(128),
                        retrospective_co VARCHAR(128),
                        tissue_source_si VARCHAR(128),
                        treatment_outcom VARCHAR(128),
                        tumor_status VARCHAR(128),
                        vital_status VARCHAR(128),
                        -- KEY(id),
                        KEY (`age_at_initial`) USING CLUSTERED COLUMNSTORE );

CREATE TABLE DRUG_TREATMENT(id INT,
                            pat_id INT,
                            bcr_drug_barcode VARCHAR(128),
                            bcr_drug_uuid VARCHAR(128),
                            bcr_patient_barc VARCHAR(128),
                            bcr_patient_uuid VARCHAR(128),
                            clinical_trial_d VARCHAR(128),
                            drug_name VARCHAR(128),
                            pharma_adjuvant_ VARCHAR(128),
                            pharma_type_othe VARCHAR(128),
                            pharmaceutical_t VARCHAR(128),
                            prescribed_dose VARCHAR(128),
                            regimen_number VARCHAR(128),
                            route_of_adminis VARCHAR(128),
                            therapy_regimen VARCHAR(128),
                            total_dose VARCHAR(128),
                            treatment_best_r VARCHAR(128),
                            tx_on_clinical_t VARCHAR(128),
                            -- KEY(id),
                            KEY (`drug_name`) USING CLUSTERED COLUMNSTORE );

CREATE TABLE RADIATION_TREATMENT(id INT,
                                pat_id INT,
                                bcr_patient_barc VARCHAR(128),
                                bcr_patient_uuid VARCHAR(128),
                                bcr_radiation_ba VARCHAR(128),
                                bcr_radiation_uu VARCHAR(128),
                                course_number VARCHAR(128),
                                radiation_adjuva VARCHAR(128),
                                radiation_therap VARCHAR(128),
                                radiation_total_ VARCHAR(128),
                                radiation_type VARCHAR(128),
                                radiation_type_o VARCHAR(128),
                                therapy_regimen VARCHAR(128),
                                treatment_best_r VARCHAR(128),
                                KEY(id));

CREATE TABLE FOLLOW_UP( id INT,
                        pat_id INT,
                        bcr_followup_bar VARCHAR(128),
                        bcr_followup_uui VARCHAR(128),
                        bcr_patient_barc VARCHAR(128),
                        bcr_patient_uuid VARCHAR(128),
                        death_days_to VARCHAR(128),
                        ecog_score VARCHAR(128),
                        followup_lost_to VARCHAR(128),
                        followup_reason VARCHAR(128),
                        karnofsky_score VARCHAR(128),
                        last_contact_day VARCHAR(128),
                        new_tumor_event_ VARCHAR(128),
                        radiation_treatm VARCHAR(128),
                        treatment_outcom VARCHAR(128),
                        tumor_status VARCHAR(128),
                        vital_status VARCHAR(128),
                        KEY(id));

CREATE TABLE IMAGES(id INT,
                    scan_id INT,
                    imgPath VARCHAR(128),
                    name VARCHAR(128),
                    -- KEY(id),
                    KEY (`scan_id`) USING CLUSTERED COLUMNSTORE );

CREATE TABLE SCANS(id INT,
                    pat_id INT,
                    hash INT,
                    KEY(id));

LOAD DATA INFILE '/home/luisremi/hls/tools/memsql_generator/hls-graph_tables_100/patients_table.txt'
    INTO TABLE PATIENTS;
LOAD DATA INFILE '/home/luisremi/hls/tools/memsql_generator/hls-graph_tables_100/drug_treatment_table.txt'
    INTO TABLE DRUG_TREATMENT;
LOAD DATA INFILE '/home/luisremi/hls/tools/memsql_generator/hls-graph_tables_100/rad_treatment_table.txt'
    INTO TABLE RADIATION_TREATMENT;
LOAD DATA INFILE '/home/luisremi/hls/tools/memsql_generator/hls-graph_tables_100/follow_up_table.txt'
    INTO TABLE FOLLOW_UP;
LOAD DATA INFILE '/home/luisremi/hls/tools/memsql_generator/hls-graph_tables_100/images_table.txt'
    INTO TABLE IMAGES;
LOAD DATA INFILE '/home/luisremi/hls/tools/memsql_generator/hls-graph_tables_100/scans_table.txt'
    INTO TABLE SCANS;

-- CREATE INDEX pat_id_idx     ON PATIENTS (id);
-- CREATE INDEX par_age_idx    ON PATIENTS (age_at_initial);
-- CREATE INDEX drug_name_idx  ON DRUG_TREATMENT (drug_name);
-- CREATE INDEX scan_id_idx    ON SCANS (hash);
-- CREATE INDEX images_id_idx  ON IMAGES (id);