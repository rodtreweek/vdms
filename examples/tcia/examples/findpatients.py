import athena

db = athena.Athena()
db.connect(host="localhost")

#Get information about all the patients
query = """
[
   {
      "FindEntity" : {
         "class" : "patient",
         "results" : {
            "list" : [ "bcr_patient_barc", "gender", "age_at_initial"],
            "sort": "age_at_initial"
         },
         "constraints": { "age_at_initial": [">=", 80]}
      }
   }
]
"""

response, images = db.query(query)
athena.aux_print_json(str(response))
