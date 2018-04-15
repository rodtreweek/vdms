USE tcia_10
DROP ALL FROM PLANCACHE;
SELECT DISTINCT (IMAGES.imgPath) FROM
    PATIENTS, IMAGES, SCANS, DRUG_TREATMENT WHERE
    SCANS.pat_id = PATIENTS.id AND
    IMAGES.scan_id = SCANS.id AND
    PATIENTS.age_at_initial >= 70 AND
    DRUG_TREATMENT.pat_id = PATIENTS.id AND
    DRUG_TREATMENT.drug_name = "Temodar"
    -- PATIENTS.bcr_patient_barc = "TCGA-14-1459-7"
-- INTO OUTFILE 'results_sql.txt';