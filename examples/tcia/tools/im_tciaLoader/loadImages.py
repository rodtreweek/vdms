#! /usr/bin/python
#%matplotlib inline
from threading import Thread
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
import sys
import os
import urllib
import time
import json
import csv
import cv2
import random
import vdms

def insertImage(patient_id, png_name, rep):
    img_str = []
    all_queries = []

    print patient_id

    findE = {}

    patient_ref = int(random.uniform(1, 10) * 1000)
    findE["_ref"] = patient_ref

    findE["class"] = "patient"
    findE["unique"] = True

    constraints = {}
    constraints["bcr_patient_barc"] = [ "==", patient_id]

    results = {}
    results["list"] = ["bcr_patient_barc"]
    # results["count"] = []

    findE["constraints"] = constraints
    findE["results"] = results

    query_pat = {}
    query_pat["FindEntity"] = findE

    all_queries.append(query_pat)

    addE = {}
    addE["class"] = "Scan"
    scan_ref = int(random.uniform(1, 10) * 100)
    addE["_ref"] = scan_ref
    properties = {}
    hash_num = int(random.uniform(1, 10) * 100000)
    properties["hash"] = hash_num
    addE["properties"] = properties
    link = {}
    link["ref"] = patient_ref
    addE["link"] = link

    add_scan = {}
    add_scan["AddEntity"] = addE
    all_queries.append(add_scan)

    connect = {}
    connect["class"] = "had_scan"
    connect["ref1"] = scan_ref
    connect["ref2"] = patient_ref
    query_connect = {}
    query_connect["Connect"] = connect
    query_connect["Connect"] = connect
    all_queries.append(query_connect)

    counter = 0
    for png in png_name:
        fd = open(png)
        img_str.append(fd.read())

        # Operations and its parameters will be defined in the interface
        op_params = {}
        op_params["value"] = 155
        op_params["type"] = "threshold"

        op_params_resize = {}
        op_params_resize["height"] = 1024*16
        op_params_resize["width"]  = 1024*16
        op_params_resize["type"] = "resize"

        # A Category is a vdms-defined Entity. It only has a name.
        # User can assign multimedia to categories without knowing that
        # a graph database is being used underneath :)
        # User will be happy.
        collections = ["BrainScans"]

        # A name is a unique identifier that the user sets.
        # We should always honor this identifiar unless there is another
        # Image with that name.
        # If no name is specified, we get to pick.
        # print os.path.basename(png_name)
        # img_params["name"] = os.path.splitext(os.path.basename(png_name))[0]
        properties = {}
        properties["name"] = os.path.basename(png) + "-" + str(rep)
        properties["number"] = counter
        counter += 1

        link = {}
        link["ref"] = scan_ref

        img_params = {}
        # img_params["operations"] = [op_params_resize]
        # img_params["collections"] = collections
        img_params["properties"] = properties
        img_params["link"] = link
        img_params["format"] = "png"
        # img_params["timing"] = True

        query = {}
        query["AddImage"] = img_params
        all_queries.append(query)

    json_query = json.dumps(all_queries, indent=4, sort_keys=False)
    start = time.time()
    response, img_array = db.query(json_query, [img_str])
    end = time.time()

    res_json = json.loads(response)
    # print json.dumps(res_json, indent=2, sort_keys=True)

    try:
        if res_json[0]["FindEntity"]["status"] != 0:
            print "error find patient!"
        if res_json[1]["AddEntity"]["status"] != 0:
            print "error add scan!"
        if res_json[2]["Connect"]["status"] != 0:
            print "error connect!"
        for i in range(0,155):
            if res_json[i+3]["AddImage"]["status"] != 0:
                print "error image!"
    except:
        print 'Error - exception'
        print json.dumps(all_queries, indent=2, sort_keys=True)
        print json.dumps(res_json, indent=2, sort_keys=True)

    # print "Querytime[ms]:" + str((end - start)*1000)
    return json.dumps(response, indent=2, sort_keys=True)

db = vdms.VDMS()
db.connect('hsw3.jf.intel.com', 55559)

root_path = "/home/luisremi/vcs/apps/hls/"
img_path = root_path + "images_hls/pngs/"
map_path = root_path + "tools/tciaLoader/data/imagesMaping.txt"

fres = open('result.txt', 'w')
times = []
multiplier = int(sys.argv[1])
print "Multiplier:",multiplier
scan_counter = 0
with open(map_path, 'rb') as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
        png_name = row["png_name"]
        patient_id = row['bcr_patient_barcode']
        # if patient_id == "TCGA-12-1601": # for some reason, problem with this
        #     continue
        if (png_name != "INVALID"):
            # print "\"" + patient_id + "\",\\"
            # scan_counter = scan_counter + 1
            # continue
            prefixed = [filename for filename in os.listdir(img_path) \
                                 if filename.startswith(png_name)]
            prefixed.sort()
            path_to_pngs = []
            counter = 0
            for png in prefixed:
                path_to_pngs.append(img_path + png)
                # path_to_pngs.append('largeImages/large1.jpg')
                counter = counter + 1
                # if counter == 2:
                    # break

            for i in range(0,multiplier):
                start = time.time()
                # print patient_id + '-' + str(i)
                # this will insert all the images for a particular patient
                res = insertImage(patient_id + '-' + str(i), path_to_pngs, i)
                end = time.time()
                # print "call time: ", (end - start)*1000
                times.append((end - start)*1000)
                fres.write(res)
        # break

plt.ylabel('Insert 155 photos(ms)')
plt.plot(times)
plt.savefig("queryPlot_" + str(multiplier) + ".png",
            format='png', bbox_inches='tight')
plt.close()

fres.close()


# Write times to a file
# ftimes = open('times.txt', 'w')
# for line in times:
#     ftimes.write(str(line) + '\n')
# ftimes.close()

