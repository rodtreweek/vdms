#! /usr/bin/python
from threading import Thread
import sys
import os
import urllib
import time
import json
import unittest
import cv2
import numpy

import athena

ATHENA_PATIENT_TAG = "patient"
ATHENA_PATIENT_ID_PROP = "bcr_patient_barc"
ATHENA_DRUG_TREATMENT = "drug_treatment"

hostname = "hsw3.jf.intel.com"

op_thr = {}
op_thr["value"] = 180
op_thr["type"] = "threshold"

op_resize = {}
op_resize["height"] = 512
op_resize["width"]  = 512
op_resize["type"] = "resize"

op_crop = {}
op_crop["x"] = 100
op_crop["x"] = 100
op_crop["height"] = 200
op_crop["width"]  = 200
op_crop["type"] = "crop"

def retrieve_patient_list(db):

    # This query is:
    # [
    #    {
    #       "FindEntity" : {
    #          "class" : "patient",
    #          "results" : {
    #             "list" : [ "bcr_patient_barc" ]
    #          }
    #       }
    #    }
    # ]

    all_queries = []

    query = {}

    constraints = {}
    constraints["drug_name"] = ["==", "Temodar"]
    results = {}
    results["list"] = ["drug_name"]

    findtreatments = {}
    findtreatments["class"] = ATHENA_DRUG_TREATMENT
    findtreatments["_ref"] = 1234
    findtreatments["constraints"] = constraints
    findtreatments["results"] = results

    query["FindEntity"] = findtreatments
    all_queries.append(query)

    query = {}

    constraints = {}
    constraints["age_at_initial"] = [">=", 70]
    link = {}
    link["ref"] = 1234
    results = {}
    results["list"] = [ATHENA_PATIENT_ID_PROP]

    findpatients = {}
    findpatients["class"] = ATHENA_PATIENT_TAG
    findpatients["results"] = results
    findpatients["link"] = link
    findpatients["constraints"] = constraints

    query["FindEntity"] = findpatients

    all_queries.append(query)

    # res = json.loads(all_queries)
    # athena.aux_print_json(json.dumps(all_queries, indent=2, sort_keys=True))
    res, img_array = db.query(all_queries)
    # athena.aux_print_json(res)
    res = json.loads(res)
    # print json.dumps(res, indent=2, sort_keys=True)
    # print "Patients Found:",res[0]["FindEntity"]["returned"]

    pat_array = []
    for pat in res[1]["FindEntity"]["entities"]:
        pat_array.append(pat[ATHENA_PATIENT_ID_PROP])

    return pat_array

def retrieve_patient_images(db, pat_id):

    print "Retrieve Patient",pat_id,"Images"
    all_queries = []

    pat_ref = 43

    constraints = {}
    constraints[ATHENA_PATIENT_ID_PROP] = [ "==", pat_id]

    results = {}
    results["list"] = [ATHENA_PATIENT_ID_PROP]

    findPatient = {}
    findPatient["class"] = ATHENA_PATIENT_TAG
    findPatient["_ref"] = pat_ref
    findPatient["unique"] = True
    findPatient["constraints"] = constraints

    query_pat = {}
    query_pat["FindEntity"] = findPatient

    all_queries.append(query_pat)

    scan_ref = 45
    findScan = {}
    findScan["class"] = "Scan"
    findScan["_ref"] = scan_ref
    findScan["results"] = {"list": ["hash"]}
    link = {}
    link["ref"] = pat_ref
    findScan["link"] = link

    query_scan = {}
    query_scan["FindEntity"] = findScan

    all_queries.append(query_scan)

    link = {}
    link["ref"] = scan_ref

    results = {}
    results["list"] = ["name"]

    img_params = {}
    # img_params["operation"] = op_params_resize
    img_params["link"] = link
    img_params["timing"] = True
    img_params["results"] = results
    img_params["operations"] = [op_resize, op_thr]

    query_imgs = {}
    query_imgs["FindImage"] = img_params

    all_queries.append(query_imgs)

    response, img_array = db.query(all_queries)
    # athena.aux_print_json(str(response))

    response = json.loads(response)
    if len(response) > 1:
    #scans_array = []
        for scan in response[1]["FindEntity"]["entities"]:
            #scan_array.append(scan["hash"])
            print "hash:",scan["hash"]

    return img_array

def get_image_by_name(db, img_name):

    print "Get images by name:",img_name
    all_queries = []

    constraints = {}
    constraints["name"] = [ "==", img_name]

    results = {}
    results["list"] = ["name"]

    img_params = {}
    img_params["constraints"] = constraints
    img_params["operations"] = [op_resize]
    img_params["results"] = results

    query_imgs = {}
    query_imgs["FindImage"] = img_params

    all_queries.append(query_imgs)
    # athena.aux_print_json(query_imgs)

    response, img_array = db.query(all_queries)

    athena.aux_print_json(str(response))

    counter = 0
    for image in img_array:
        counter += 1
        # file_bytes = numpy.asarray(image, dtype=numpy.uint8)
        array = numpy.frombuffer(image, dtype='uint8')
        cv_image = cv2.imdecode(array, 1)
        cv2.imwrite("brain_" + str(counter) + ".png", cv_image)

def run_full_query(arr, idx):

    db = athena.Athena()
    db.connect(hostname)
    all_queries = []

    query = {}

    constraints = {}
    constraints["drug_name"] = ["==", "Temodar"]
    results = {}
    results["list"] = ["drug_name"]

    findtreatments = {}
    findtreatments["class"] = ATHENA_DRUG_TREATMENT
    findtreatments["_ref"] = 1234
    findtreatments["constraints"] = constraints
    # findtreatments["results"] = results

    query["FindEntity"] = findtreatments
    all_queries.append(query)

    query = {}

    constraints = {}
    constraints["age_at_initial"] = [">=", 70]
    link = {}
    link["ref"] = 1234
    results = {}
    results["list"] = [ATHENA_PATIENT_ID_PROP]

    findpatients = {}
    findpatients["class"] = ATHENA_PATIENT_TAG
    # findpatients["results"] = results
    findpatients["link"] = link
    findpatients["_ref"] = 3333
    findpatients["constraints"] = constraints

    query["FindEntity"] = findpatients

    all_queries.append(query)

    scan_ref = 45
    findScan = {}
    findScan["class"] = "Scan"
    findScan["_ref"] = scan_ref
    # findScan["results"] = {"list": ["hash"]}
    link = {}
    link["ref"] = 3333
    findScan["link"] = link

    query_scan = {}
    query_scan["FindEntity"] = findScan

    all_queries.append(query_scan)

    link = {}
    link["ref"] = scan_ref

    results = {}
    results["list"]  = ["imgPath"]
    #results["count"] = []

    img_params = {}
    # img_params["operation"] = op_params_resize
    img_params["link"] = link
    # img_params["timing"] = True
    img_params["results"] = results
    img_params["class"] = "AT:IMAGE"
    # img_params["operations"] = [op_resize, op_thr]

    query_imgs = {}
    query_imgs["FindEntity"] = img_params

    all_queries.append(query_imgs)

    start = time.time()
    response, img_array = db.query(all_queries)
    end = time.time()
    arr[idx] = (end - start)*1000
    # print "call time: ", (end - start)*1000

    # athena.aux_print_json(str(response))
    # print len(str(response))

    # print "total images:", len(img_array)

if __name__ == '__main__':

    db = athena.Athena()
    db.connect(hostname)

    # get_image_by_name(db, "VSD.Brain.XX.O.MR_Flair.35529.nii.0080..png-0")

    for i in range(0,2):
        run_full_query()
    start = time.time()
    # run_full_query(db)
    end = time.time()
    # print "call time: ", (end - start)*1000

    total_threads = 4
    exe_array = [0] * total_threads
    sum_all = 0
    iterations = 5

    for i in range(0,iterations):
        thread_array = []
        for i in range(0,total_threads):
            t = Thread(target=run_full_query, args=(exe_array, i))
            thread_array.append(t)
            t.start()

        for t in thread_array:
            t.join()

        for x in exe_array:
            sum_all += int(x)

    print "avg tx time:",str(sum_all/(iterations*len(exe_array)))



