#!/usr/bin/env python3

# Import modules
import pytz
import json
import urllib
import http.client
from datetime import datetime

def genDate(date, format = "%Y-%m-%d %H:%M:%S", region = "America/Cancun"):
    try:
        local_time = pytz.timezone(region)
        if isinstance(date, str): naive = datetime.strptime(date, format)
        elif isinstance(date, datetime): naive = date
        else: return None
        
        local_dt = local_time.localize(naive, is_dst=None)
        utc_dt = local_dt.astimezone(pytz.utc).isoformat()
        myDate = {"__type": "Date", "iso": utc_dt}
        #print(myDate)
        return myDate
    
    except Exception as e:
        print("ERROR in genDate()- {}".format(e))
        return None
    
class parseClient:
    def __init__(self, server, appId, restKey, port = 443, mountPath = "/parse"):
        self.server = server
        self.mountPath = mountPath
        self.appId = appId
        self.restKey = restKey
        self.port = port
        self.myQuery = {}
    
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
    
    def _query(self, Class, where = {"objectId": {"$exists": True}}, order = "updatedAt", limit = 1000, skip = 0, keys = [] , excludeKeys = [], include = [], count = 0):
        """ Query keys
        Parameter -> Use
        order -> Specify a field to sort by
        limit -> Limit the number of objects returned by the query
        skip -> Use with limit to paginate through results
        keys -> Restrict the fields returned by the query
        excludeKeys -> Exclude specific fields from the returned query
        include -> Use on Pointer columns to return the full object
        """
        param = {
            "where": json.dumps(where),
            "order": order,
            "limit": limit,
            "skip": skip,
            "keys": keys,
            "excludeKeys": excludeKeys,
            "include": include,
            "count": count}
        params = urllib.parse.urlencode(param)
        
        connection = http.client.HTTPSConnection(self.server, self.port)
        connection.connect()
        connection.request('GET', '{}/classes/{}?{}'.format(self.mountPath, Class, params), '', {
            "X-Parse-Application-Id": self.appId,
            "X-Parse-REST-API-Key": self.restKey
            })
        result = json.loads(connection.getresponse().read())
        return  result
    
    def query(self, Class, where = {"objectId": {"$exists": True}}, order = "updatedAt", limit = 1000, keys = [] , excludeKeys = [], include = []):
        Results = []
        #print(where) # DEBUG
        r = self._query(Class, where = where, order = order, count = 1, limit = 0)
        
        for i in range(0, r["count"], limit):
            r = self._query(Class, where = where, order = order, limit = limit, skip = i, keys = keys, excludeKeys = excludeKeys, include = include)
            try: Results += r["results"]
            except: print("ERROR: {}").format(r)
            
        self.myQuery = {}
        return Results
        
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
    
    def addQuery(self, variable, value = None, lt = None, lte = None, gt = None, gte = None, ne = None, _in = None, nin = None, exists = None):        
        if(not variable in self.myQuery): self.myQuery[variable] = {}
        
        if value != None: self.myQuery[variable] = value
        if lt != None: self.myQuery[variable]["$lt"] = lt
        if lte != None: self.myQuery[variable]["$lte"] = lte
        if lt != None: self.myQuery[variable]["$lt"] = lt
        if gt != None: self.myQuery[variable]["$gt"] = gt
        if gte != None: self.myQuery[variable]["$gte"] = gte
        if _in != None: self.myQuery[variable]["$in"] = _in
        if nin != None: self.myQuery[variable]["$lt"] = nin
        if exists != None: self.myQuery[variable]["$exists"] = exists
    
    """ Query filters
    $lt ->Less Than
    $lte -> Less Than Or Equal To
    $gt -> Greater Than
    $gte -> Greater Than Or Equal To
    $ne -> Not Equal To
    $in -> Contained In
    $nin -> Not Contained in
    $exists -> A value is set for the key
    
    NOT SUPPORTED YET
    $select -> This matches a value for a key in the result of a different query
    $dontSelect -> Requires that a key’s value not match a value for a key in the result of a different query
    $all -> Contains all of the given values
    $regex -> Requires that a key’s value match a regular expression
    $text -> Performs a full text search on indexed fields
    """
    
# Parse client
client = parseClient(serverUrl, appId, restKey, mountPath)

# Parse querys examples
"""
# Query with dateTime string needs format
client.addQuery("realDate",
                gte = genDate("2022-04-30 14:00:00", "%Y-%m-%d %H:%M:%S"),
                lte = genDate("2022-05-01 14:00:00", "%Y-%m-%d %H:%M:%S"))
"""
# Same query with dateTime object
client.addQuery("realDate",
                gte = genDate(datetime(2022, 4, 30, 14, 0, 0)),
                lte = genDate(datetime(2022, 5, 1, 14, 0, 0)))

# Query by variable exact value
#client.addQuery("type", value = "INFO")
# Query by variable exact value
client.addQuery("device", value = "esp32front2")

# Use Query
r = client.query("Log", where = client.myQuery, limit = 10000)
print(len(r))

