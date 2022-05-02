#!/usr/bin/env python3

# Import modules
import json
import urllib
import http.client

class parseClient:
    def __init__(self, server, appId, restKey, port = 443, mountPath = "/parse"):
        self.server = server
        self.mountPath = mountPath
        self.appId = appId
        self.restKey = restKey
        self.port = port
    
    def createObject(self, Class, Object):
        connection = http.client.HTTPSConnection(self.server, self.port)
        connection.connect()
        
        if type(Object) is list:
            objectList = []
            for obj in Object:
                objectList.append({"method": "POST",
                                  "path": "/parse/classes/{}".format(Class),
                                  "body": obj
                                  })
            connection.request('POST', '{}/batch'.format(self.mountPath), json.dumps({"requests": objectList}),
             {
               "X-Parse-Application-Id": self.appId,
               "X-Parse-REST-API-Key": self.restKey,
               "Content-Type": "application/json"
             })
        else:
            connection.request('POST', '{}/classes/{}'.format(self.mountPath, Class), json.dumps(Object), {
               "X-Parse-Application-Id": self.appId,
               "X-Parse-REST-API-Key": self.restKey,
               "Content-Type": "application/json"
             })
        
        results = json.loads(connection.getresponse().read())
        return results
    
    def query(self, Class, where = ""):
        connection = http.client.HTTPSConnection(self.server, self.port)
        connection.connect()
        if(where == ""):
            connection.request('GET', '{}/classes/{}'.format(self.mountPath, Class), '', {
                "X-Parse-Application-Id": self.appId,
                "X-Parse-REST-API-Key": self.restKey
                })
        else:
            params = urllib.urlencode({"where": json.dumps(where)})
            connection.request('GET', '{}/classes/{}?where={}'.format(self.mountPath, Class, params), '', {
                "X-Parse-Application-Id": self.appId,
                "X-Parse-REST-API-Key": self.restKey
                })
        result = json.loads(connection.getresponse().read())
        return  result
    
    def update(self, Class, objectId, keys):
        connection = http.client.HTTPSConnection(self.server, self.port)
        connection.connect()
        connection.request('PUT', '{}/classes/{}/{}'.format(self.mountPath, Class, objectId), json.dumps(keys), {
               "X-Parse-Application-Id": self.appId,
               "X-Parse-REST-API-Key": self.restKey,
               "Content-Type": "application/json"
             })
        result = json.loads(connection.getresponse().read())
        return result