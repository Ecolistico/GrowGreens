#!/usr/bin/env python3

# Import modules
import base64

def encode(string2Encode):
    encodedBytes = base64.b64encode(string2Encode.encode("utf-8"))
    encodedString = str(encodedBytes, "utf-8")
    return encodedString

def decode(string2Decode):
    decodedBytes = base64.b64decode(string2Decode.encode("utf-8"))
    decodedString = str(decodedBytes, "utf-8")
    return decodedString
