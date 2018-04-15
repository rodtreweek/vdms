import athena
from IPython.display import Image

op_thr = {}
op_thr["value"] = 180
op_thr["type"] = "threshold"

op_crop = {}
op_crop["x"] = 110
op_crop["y"] = 130
op_crop["height"] = 60
op_crop["width"]  = 60
op_crop["type"] = "crop"

op_resize = {}
op_resize["height"] = 512
op_resize["width"]  = 512
op_resize["type"] = "resize"

ATHENA_PATIENT_TAG = "patient"
ATHENA_PATIENT_ID_PROP = "bcr_patient_barc"
ATHENA_DRUG_TREATMENT = "drug_treatment"

def run_query (pat, seg=True, resize=False, verbose=False):
    
    db = athena.Athena()
    db.connect('hsw3.jf.intel.com')
    
    all_queries = []

    pat_ref = 43

    constraints = {}
    constraints[ATHENA_PATIENT_ID_PROP] = [ "==", pat]

    results = {}
    results["list"] = [ATHENA_PATIENT_ID_PROP]

    findPatient = {}
    findPatient["class"] = ATHENA_PATIENT_TAG
    findPatient["_ref"] = pat_ref
    findPatient["unique"] = True
    findPatient["results"] = results
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

    constraints = {}
    
    if seg:
        img_name = "VSD.Brain.XX.O.MR_Flair.35619.nii.0040..png-0_segmented"
    else:
        img_name = "VSD.Brain.XX.O.MR_Flair.35619.nii.0040..png-0"
        
    constraints["name"] = [ "==", img_name]
        
    results = {}
    results["list"] = ["name"]

    img_params = {}
    img_params["constraints"] = constraints
    if resize:
        img_params["operations"] = [op_resize, op_thr]
    img_params["results"] = results

    query_imgs = {}
    query_imgs["FindImage"] = img_params

    all_queries.append(query_imgs)
    
    if verbose:
        print "Query"
        athena.aux_print_json(all_queries)

    response, img_array = db.query(all_queries)

    if verbose:
        print "Response:"
        athena.aux_print_json(str(response))

    print "Response:"
    
    if len(img_array) == 0:
        print "Not found"

    # Displaying images on Jupyter
    counter = 0
    for im in img_array:
        img_file = 'images/res_' + str(counter) + '.jpg'
        counter = counter + 1
        fd = open(img_file, 'w+')
        fd.write(im)
        fd.close()
        display(Image(img_file))
        
    db.disconnect()
