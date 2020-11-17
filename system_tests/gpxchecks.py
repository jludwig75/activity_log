#!/usr/bin/env python3
from gpxparser import parseGpxFile
import sys


def compareGpxFiles(file1Name, file2Name):
    activity1 = parseGpxFile(file1Name)
    if activity1 == None:
        return False
    activity2 = parseGpxFile(file2Name)
    if activity2 == None:
        return False
    if activity1.name != activity2.name or activity1.startTime != activity2.startTime:
        return False
    if len(activity1.trackPoints) != len(activity2.trackPoints):
        return False
    for i in range(len(activity1.trackPoints)):
        trackPoint1 = activity1.trackPoints[i]
        trackPoint2 = activity2.trackPoints[i]
        if trackPoint1.time != trackPoint2.time or trackPoint1.latitude != trackPoint2.latitude or trackPoint1.altitude != trackPoint2.altitude or trackPoint1.heartRate != trackPoint2.heartRate:
            return False
    return True

def validateGpxPlot(gpxFileName, dataPoints):
    return True    # TODO: Implement this

def validateGpxTrack(gpxFileName, trackPoints):
    return True    # TODO: Implement this

if __name__ == "__main__":
    gpxFileName = sys.argv[1]
    activity = _parseGpxFile(gpxFileName)
    if activity == None:
        print('Failed to parse GPX file "%s"' % gpxFileName)
    print(json.dumps(activity.map))