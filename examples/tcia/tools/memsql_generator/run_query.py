import time
from threading import Thread
import requests

import numpy
import cv2
from memsql.common import database

# Specify connection information for a MemSQL node
HOST = "hsw1.jf.intel.com"
local_H = "hsw3.jf.intel.com"
PORT = 3306
USER = "root"
PASSWORD = ""

def get_connection(db):
    """ Returns a new connection to the database. """
    return database.connect(host=HOST, port=PORT, user=USER, password=PASSWORD, database=db)

def retrieve_single_image():

    with get_connection(db="tcia_10") as conn:
        str_res = conn.query("""SELECT DISTINCT (IMAGES.imgPath)
            FROM IMAGES WHERE
            IMAGES.name = "VSD.Brain.XX.O.MR_Flair.35529.nii.0080..png-0"
            """
            )

    counter = 0
    for res in str_res:
        img = requests.get("http://"+HOST+"/"+res["imgPath"])
        # print len(img.content)
        array = numpy.frombuffer(img.content, dtype='uint8')
        cv_image = cv2.imdecode(array, 1)
        cv_image = cv2.resize(cv_image, (100, 100))
        # cv2.imwrite("save_" + str(counter) + ".png", cv_image)
        counter +=1

def retrieve_scan():

    with get_connection(db="tcia_10") as conn:
        str_res = conn.query(""" SELECT DISTINCT (IMAGES.imgPath)
            FROM SCANS, IMAGES WHERE
            IMAGES.scan_id = SCANS.id AND
            SCANS.hash = 5416361 """
            )

    counter = 0
    total_mb = 0
    for res in str_res:
        img = requests.get("http://"+HOST+"/"+res["imgPath"])
        total_mb += len(img.content)
        array = numpy.frombuffer(img.content, dtype='uint8')
        cv_image = cv2.imdecode(array, 1)
        cv_image = cv2.resize(cv_image, (100, 100))
        # cv2.imwrite("save_" + str(counter) + ".png", cv_image)
        counter +=1

    print counter
    print total_mb / 2**20
    return counter

def run_full_query():

    with get_connection(db="tcia_10") as conn:
        str_res = conn.query("""SELECT DISTINCT (IMAGES.imgPath)
            FROM PATIENTS, IMAGES, SCANS, DRUG_TREATMENT WHERE
            SCANS.pat_id = PATIENTS.id AND
            IMAGES.scan_id = SCANS.id AND
            PATIENTS.age_at_initial >= 75 AND
            DRUG_TREATMENT.pat_id = PATIENTS.id AND
            DRUG_TREATMENT.drug_name = "Temodar" """
            )

    counter = 0
    total_mb = 0
    for res in str_res:
        img = requests.get("http://"+HOST+"/"+res["imgPath"])
        total_mb += len(img.content)
        array = numpy.frombuffer(img.content, dtype='uint8')
        cv_image = cv2.imdecode(array, 1)
        cv_image = cv2.resize(cv_image, (100, 100))
        # cv2.imwrite("save_" + str(counter) + ".png", cv_image)
        counter +=1

    print counter
    print total_mb / 2**20
    return counter

def run_query(db_size, arr, idx, quer):

    # print db_size

    start = time.time()
    with get_connection(db="tcia_" + str(db_size)) as conn:
        str_res = conn.query("SELECT DISTINCT " + quer + """(IMAGES.imgPath)
            FROM PATIENTS, IMAGES, SCANS, DRUG_TREATMENT WHERE
            SCANS.pat_id = PATIENTS.id AND
            IMAGES.scan_id = SCANS.id AND
            PATIENTS.age_at_initial >= 70 AND
            DRUG_TREATMENT.pat_id = PATIENTS.id AND
            DRUG_TREATMENT.drug_name = "Temodar" """
            )

    end = time.time()
    exe_time = (end-start)*1000
    # print (str_res)
    arr[idx] = exe_time

if __name__ == '__main__':

    # start = time.time()
    # retrieve_single_image()
    # end = time.time()
    # exe_time = (end-start)
    # print "read single:",1 / exe_time

    # start = time.time()
    # ret = retrieve_scan()
    # end = time.time()
    # exe_time = (end-start)
    # print "IPS scan:",ret / exe_time

    # start = time.time()
    # ret = run_full_query()
    # end = time.time()
    # exe_time = (end-start)
    # print "IPS all:",ret / exe_time

    try:

        total_threads = 4
        exe_array = [0] * total_threads
        sum_all = 0
        iterations = 5

        print "memsql-c1, ",
        for size in [1,10,100]:
            for i in range(0,iterations):
                thread_array = []
                for i in range(1,total_threads):
                    t = Thread(target=run_query, args=(size,exe_array,
                                                       i, "count"))
                    thread_array.append(t)
                    t.start()

                for t in thread_array:
                    t.join()

                for x in exe_array:
                    sum_all += int(x)

            print str(sum_all/(iterations*len(exe_array)))+", ",

        print "\n",
        print "memsql-p2, ",

        for size in [1,10,100]:
            for i in range(0,iterations):
                thread_array = []
                for i in range(1,total_threads):
                    t = Thread(target=run_query, args=(size,exe_array,
                                                       i, ""))
                    thread_array.append(t)
                    t.start()

                for t in thread_array:
                    t.join()

                for x in exe_array:
                    sum_all += int(x)

            print str(sum_all/(iterations*len(exe_array))) + ", ",

        print "\n",
        print "done"
    except KeyboardInterrupt:
        print("Interrupted... exiting...")
