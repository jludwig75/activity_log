import json
import sys
import time
import xml.etree.ElementTree as ET

GPX_TIME_STR = r'%Y-%m-%dT%H:%M:%SZ'

def _parseGpxTimeString(timeString):
    return int(time.mktime(time.strptime(timeString, GPX_TIME_STR)))

class ParsedActivity:
    def __init__(self, name, startTime):
        self.name = name
        self.startTime = startTime
        self.trackPoints = []
    @property
    def map(self):
        activityMap = { 'name': self.name, 'startTime': self.startTime }
        activityMap['trackPoints'] = [ trackPoint.map for trackPoint in self.trackPoints ]
        return activityMap

class ParsedTrackPoint:
    def __init__(self, time, latitude, longitude, altitude, heartRate):
        self.time = time
        self.latitude = latitude
        self.longitude = longitude
        self.altitude = altitude
        self.heartRate = heartRate
    @property
    def map(self):
        return {'time': self.time,
                'latitude': self.latitude,
                'longitude': self.longitude,
                'altitude': self.altitude,
                'heartRate': self.heartRate } 

def _elementName(name):
    return '{http://www.topografix.com/GPX/1/1}' + name

def _getChild(element, childName):
    return element.find('./' + _elementName(childName))

def _getChildren(element, childName):
    return element.findall('./' + _elementName(childName))

def parseGpxFile(gpxFileName):
    tree = ET.parse(gpxFileName)
    gpx = tree.getroot()
    metadata = _getChild(gpx, 'metadata')
    if metadata == None:
        return None
    time = _getChild(metadata, 'time')
    if time == None:
        return None
    startTime = time.text
    trk = _getChild(gpx, 'trk')
    if trk == None:
        return None
    name = _getChild(trk, 'name')
    if name == None:
        return None
    activity = ParsedActivity(name.text, _parseGpxTimeString(startTime))
    for trkseg in _getChildren(trk, 'trkseg'):
        for trkpt in _getChildren(trkseg, 'trkpt'):
            latitude = float(trkpt.get('lat'))
            longitude = float(trkpt.get('lon'))
            ele = _getChild(trkpt, 'ele')
            if ele == None:
                return None
            altitude = float(ele.text)
            time = _getChild(trkpt, 'time')
            if time == None:
                return None
            # TODO: parse heart rate
            activity.trackPoints.append(ParsedTrackPoint(_parseGpxTimeString(time.text), latitude, longitude, altitude, 0))
    return activity

