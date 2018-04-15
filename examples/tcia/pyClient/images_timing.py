#! /usr/bin/python
import time
import json
import cv2
import numpy

import athena

hostname = "hsw3.jf.intel.com"

def retrieve_scan():

    db = athena.Athena()
    db.connect(hostname)
    all_queries = []

    query = {}

    scan_ref = 1
    constraints = {}
    constraints["hash"] = ["==", 5416361]

    results = {}
    results["list"] = ["hash"]

    op_resize = {}
    op_resize["height"] = 100
    op_resize["width"]  = 100
    op_resize["type"] = "resize"

    findScan = {}
    findScan["_ref"] = scan_ref
    findScan["class"] = "Scan"
    findScan["constraints"] = constraints
    findScan["results"] = results

    query_scan = {}
    query_scan["FindEntity"] = findScan

    all_queries.append(query_scan)

    link = {}
    link["ref"] = scan_ref

    results = {}
    results["list"] = ["name"]

    img_params = {}
    img_params["operation"] = [op_resize]
    # img_params["results"] = results
    img_params["link"] = link

    query_imgs = {}
    query_imgs["FindImage"] = img_params

    all_queries.append(query_imgs)

    # res = json.loads(all_queries)
    # athena.aux_print_json(json.dumps(all_queries, indent=2, sort_keys=True))
    res, img_array = db.query(all_queries)
    # athena.aux_print_json(res)
    res = json.loads(res)
    # print json.dumps(res, indent=2, sort_keys=True)
    # print "Patients Found:",res[0]["FindEntity"]["returned"]

    counter = 0
    for img in img_array:
        array = numpy.frombuffer(img, dtype='uint8')
        cv_image = cv2.imdecode(array, 1)
        # cv2.imwrite("save_ath_" + str(counter) + ".png", cv_image)
        counter +=1

    return counter

    # pat_array = []
    # for pat in res[1]["FindEntity"]["entities"]:
    #     pat_array.append(pat[ATHENA_PATIENT_ID_PROP])

    # return pat_array

def retrieve_single_image():

    db = athena.Athena()
    db.connect(hostname)
    all_queries = []

    constraints = {}
    constraints["name"] = ["==",
        "VSD.Brain.XX.O.MR_Flair.35529.nii.0080..png-0"]

    op_resize = {}
    op_resize["height"] = 100
    op_resize["width"]  = 100
    op_resize["type"] = "resize"

    results = {}
    results["list"] = ["name"]

    img_params = {}
    img_params["operation"] = [op_resize]
    img_params["constraints"] = constraints
    img_params["results"] = results

    query_imgs = {}
    query_imgs["FindImage"] = img_params

    all_queries.append(query_imgs)

    # res = json.loads(all_queries)
    # athena.aux_print_json(json.dumps(all_queries, indent=2, sort_keys=True))
    res, img_array = db.query(all_queries)
    # athena.aux_print_json(res)
    res = json.loads(res)
    # print json.dumps(res, indent=2, sort_keys=True)
    # print "Patients Found:",res[0]["FindEntity"]["returned"]

    counter = 0
    for img in img_array:
        array = numpy.frombuffer(img, dtype='uint8')
        cv_image = cv2.imdecode(array, 1)
        # cv2.imwrite("save_ath_" + str(counter) + ".png", cv_image)
        counter +=1

    return counter

def run_full_query():

    db = athena.Athena()
    db.connect(hostname)

    ATHENA_PATIENT_TAG = "patient"
    ATHENA_PATIENT_ID_PROP = "bcr_patient_barc"
    ATHENA_DRUG_TREATMENT = "drug_treatment"

    all_queries = []

    query = {}

    constraints = {}
    constraints["drug_name"] = ["==", "Temodar"]
    results = {}
    # results["list"] = ["drug_name"]
    results["count"] = []

    findtreatments = {}
    findtreatments["class"] = ATHENA_DRUG_TREATMENT
    findtreatments["_ref"] = 1234
    findtreatments["constraints"] = constraints
    findtreatments["results"] = results

    query["FindEntity"] = findtreatments
    all_queries.append(query)

    query = {}

    constraints = {}
    constraints["age_at_initial"] = [">=", 75]
    link = {}
    link["ref"] = 1234
    results = {}
    # results["list"] = [ATHENA_PATIENT_ID_PROP]
    results["count"] = []

    findpatients = {}
    findpatients["class"] = ATHENA_PATIENT_TAG
    findpatients["results"] = results
    findpatients["link"] = link
    findpatients["_ref"] = 3333
    findpatients["constraints"] = constraints

    query["FindEntity"] = findpatients

    all_queries.append(query)

    scan_ref = 45
    findScan = {}
    findScan["class"] = "Scan"
    findScan["_ref"] = scan_ref
    findScan["results"] = {"list": ["hash"]}
    findScan["results"] = {"count": []}
    link = {}
    link["ref"] = 3333
    findScan["link"] = link

    query_scan = {}
    query_scan["FindEntity"] = findScan

    all_queries.append(query_scan)

    link = {}
    link["ref"] = scan_ref

    results = {}
    # results["list"]  = ["name"]
    # results["count"] = []

    op_resize = {}
    op_resize["height"] = 100
    op_resize["width"]  = 100
    op_resize["type"] = "resize"

    img_params = {}
    img_params["link"] = link
    # img_params["results"] = results
    img_params["operations"] = [op_resize]

    query_imgs = {}
    query_imgs["FindImage"] = img_params

    all_queries.append(query_imgs)

    # athena.aux_print_json(json.dumps(all_queries, indent=2, sort_keys=True))

    # start = time.time()
    response, img_array = db.query(all_queries)
    # end = time.time()
    # print "all time: ", (end - start)*1000

    # athena.aux_print_json(str(response))
    return len(img_array)

    # print "total images:", len(img_array)

if __name__ == '__main__':

    start = time.time()
    retrieve_single_image()
    end = time.time()
    exe_time = (end-start)
    print "read single:",1 / exe_time

    start = time.time()
    ret = retrieve_scan()
    end = time.time()
    exe_time = (end-start)
    print "IPS scan:",ret / exe_time

    start = time.time()
    ret = run_full_query()
    end = time.time()
    exe_time = (end-start)
    print "IPS all:",ret / exe_time



