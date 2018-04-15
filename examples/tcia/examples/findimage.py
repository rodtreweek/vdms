import athena

db = athena.Athena()
db.connect("localhost")

query = """
[
{
  "FindImage" : {
     "constraints":{
         "name": ["==", "VSD.Brain.XX.O.MR_Flair.35619.nii.0085..png-0"]
     },
     "results" : {
        "list" : [ "name" ]
     },
     "operations": [
         {
             "type": "resize",
             "height": 512,
             "width": 512
         },
         {
             "type": "threshold",
             "value": 180
         }
     ],
  }
}
]
"""

response, images = db.query(query)
athena.aux_print_json(str(response))

counter = 0
for im in images:
    img_file = 'images/res_' + str(counter) + '.jpg'
    counter = counter + 1
    fd = open(img_file, 'w+')
    fd.write(im)
    fd.close()
