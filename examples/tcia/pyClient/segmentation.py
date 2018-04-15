#! /usr/bin/python
from threading import Thread
from IPython import display

import sys
import os
import urllib
import time
import json
import unittest
import cv2
import numpy

import vdms

mapImgPat = {}

ATHENA_PATIENT_TAG = "patient"
ATHENA_PATIENT_ID_PROP = "bcr_patient_barc"
ATHENA_DRUG_TREATMENT = "drug_treatment"

hostname = "hsw3.jf.intel.com"

op_thr = {}
op_thr["value"] = 20
op_thr["type"] = "threshold"

op_resize = {}
op_resize["height"] = 240
op_resize["width"]  = 240
op_resize["type"] = "resize"

op_crop = {}
op_crop["x"] = 100
op_crop["x"] = 100
op_crop["height"] = 200
op_crop["width"]  = 200
op_crop["type"] = "crop"

def retrieve_patient_list(db):

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
    # img_params["timing"] = True
    img_params["results"] = results
    #img_params["operations"] = [op_resize, op_thr]

    query_imgs = {}
    query_imgs["FindImage"] = img_params

    all_queries.append(query_imgs)

    response, img_array = db.query(all_queries)
    vdms.aux_print_json(str(response))
    response = json.loads(response)

    names = []
    if len(response) == 3:
        for ent in response[2]["FindImage"]["entities"]:
            names.append(ent["name"])
            # print ent["name"]

    return img_array, names


def get_patient_folder(input_image):
    #print input_image
    newval = input_image[:29]
    #print mapImgPat[newval]
    
    return mapImgPat[newval]

def get_segmented_image(input_image):
    
    img_number = input_image[35:38]
    img_number = int(img_number)
    img_number +=1
    img_number = str(img_number).zfill(3)
    
    patient_folder = get_patient_folder(input_image)
    
    sub = None
    dirname_j = os.path.join('./segmentations/', patient_folder)
    for dirname, dirnames, filenames in os.walk(dirname_j):
        sub = dirname
    
    if not sub:
        return "error"
    
    dirname_j = sub
    sub = None
    for dirname, dirnames, filenames in os.walk(dirname_j):
        sub = dirname
    
    if not sub:
        return "error"
    
    dirname_j = sub
    
    for dirname, dirnames, filenames in os.walk(dirname_j):
         for file in filenames:
                if file.find("VSD_"+img_number) != -1:
                        return os.path.join(dirname_j, file)
    return "error"

def run_segmentation(img_array, img_ids):

    #display.display(Image("save_aux.png"))
    seg_images = []
    # print "Running Segmentation..."
    counter = 0
    print "ids:",len(img_ids)
    print "images:",len(img_array)
    for image in img_array:
        # file_bytes = numpy.asarray(image, dtype=numpy.uint8)
        array = numpy.frombuffer(image, dtype='uint8')
        cv_image_or = cv2.imdecode(array, 1)
        original_name = "save_or.png"
        
        # cv2.imwrite(original_name, cv_image_or)
        # ret, cv_image = cv2.threshold(cv_image_or,20,0,cv2.THRESH_TOZERO)
        segmented_name = get_segmented_image(img_ids[counter])

        #print segmented_name
        if segmented_name == "error":
            #print "Error"
            return img_array
        
        #print "ok",counter
        
        segmented_img = cv2.imread(segmented_name)
        
        vis = numpy.concatenate((cv_image_or, segmented_img), axis=1)
        cv2.imwrite("contact.png", vis)
        
        # ret, encoded = cv2.imencode(".png", segmented_img)
        
        display.clear_output(wait=True)
        display.display(display.Image("contact.png"))
        time.sleep(0.05)
        
        fd = open(segmented_name)
        seg_images.append(fd.read())
        counter += 1

    return seg_images

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

def push_images(db, img_array, img_ids):

    for i in range(0,len(img_array)):

        all_queries = []

        constraints = {}
        constraints["name"] = [ "==", img_ids[i]]

        results = {}
        results["list"] = ["name"]

        img_params = {}
        img_params["class"] = "AT:IMAGE"
        img_params["_ref"] = 4545
        img_params["constraints"] = constraints
        img_params["results"] = results

        query_imgs = {}
        query_imgs["FindEntity"] = img_params

        all_queries.append(query_imgs)

        link = {}
        link["ref"] = 4545

        properties = {}
        properties["name"] = img_ids[i] + "_segmented"

        img_params = {}
        img_params["properties"] = properties
        img_params["link"] = link
        img_params["format"] = "png"

        query = {}
        query["AddImage"] = img_params
        all_queries.append(query)

        # print len(img_array[i])

        arr = []
        arr.append(img_array[i])
        response, r = db.query(all_queries, [arr])
        # athena.aux_print_json(str(response))

def run():

    db = vdms.VDMS()
    db.connect(hostname)
    
    import csv
    with open('imagesMaping.txt', 'rb') as csvfile:
        file = csv.reader(csvfile, delimiter=',')
        for row in file:
            mapImgPat[row[5]] = row[6]

    # get_image_by_name(db, "VSD.Brain.XX.O.MR_Flair.35529.nii.0080..png-0")
    pat_array = retrieve_patient_list(db)


    print "Patients Found:", len(pat_array)

    total_images = 0
    for pat in pat_array:
        img_array, img_ids = retrieve_patient_images(db, pat)
        print "Running Segmentation on",len(img_array),"images..."
        total_images += len(img_array)
        seg_images = run_segmentation(img_array, img_ids)
        push_images(db, seg_images, img_ids)

        if total_images > 2000:
            break


