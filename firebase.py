import firebase_admin
from firebase_admin import firestore
import serial
import time

cred_obj = firebase_admin.credentials.Certificate('./years-1264e-firebase-adminsdk-h3c89-737fc7d7fc.json')
default_app = firebase_admin.initialize_app(cred_obj)
db = firestore.client()

ser = serial.Serial(                                                                                                                                                                                                                                                    port = '/dev/ttyACM0',
    baudrate = 9600,
    )

def update_firebase(latitude, longitude):
    buses_ref = db.collection(u'Buses')
    history_ref = db.collection(u'History')
    docs = buses_ref.stream()
    for doc in docs:
        id = doc.id
        data = doc.to_dict()
        old_location = data['geometry'] # Not a full copy
        data['geometry'].latitude = latitude
        data['geometry'].longitude = longitude
        print(data['geometry'].latitude,data['geometry'].longitude)
        db.collection(u'Buses').document(id).set(data)

        history_doc_ref = history_ref.document(id)
        history = history_doc_ref.get()
        
        # Actually just stores the new one in history because it is not a full copy 
        # Can be fixed but it is what it is
        if(history.exists):
            history_array = history.to_dict().get('history')
            history_array.append(old_location)
            history_ref.document(id).set({
                u'history': history_array
            })
            print("Added to history")
        else:
            history_array = [old_location]
            history_ref.document(id).set({
                u'history': history_array
            })
            print("Created History doc")
        print("Success, Now we sleep")
        # Do not delete the break here
        break 
    time.sleep(15)

while True:
    pushStr = ""

    for line in ser.read():
        pushStr += chr(line)

    latitude, longitude = pushStr.split(",")
    print(f"Read: {latitude}, {longitude}")
    update_firebase(latitude, longitude)


# lats = 27.70026783492603, 85.33948577371456
# update_firebase(lats[0], lats[1])