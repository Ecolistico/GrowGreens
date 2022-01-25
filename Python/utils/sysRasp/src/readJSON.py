import json
import os

if(os.path.isfile('/var/www/html/data/data')):
    with open('/var/www/html/data/data.json') as json_file:
        data = json.load(json_file)
        print(data['containerID'])
