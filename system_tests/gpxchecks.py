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
    activity = parseGpxFile(gpxFileName)
    if activity == None:
        return False

    if len(activity.trackPoints) -1 != len(dataPoints):
        return False
    
    if (len(dataPoints) < 1):
        return True
    
    startTime = activity.trackPoints[0].time
    cummulativeAscent = 0
    cummulativeDescent = 0
    for i in range(len(dataPoints)):
        trackPoint = activity.trackPoints[i + 1]
        dp = dataPoints[i]

        altitudeChange = trackPoint.altitude - activity.trackPoints[i].altitude
        if altitudeChange > 0:
            cummulativeAscent += altitudeChange
        elif altitudeChange < 0:
            cummulativeDescent += -altitudeChange

        if (trackPoint.time - startTime) != dp['time']:
            return False
        if abs(trackPoint.altitude - dp['altitude']) >= 0.1:
            return False
        if abs(cummulativeAscent - dp['cummulative_ascent']) >= 0.1:
            return False
        if abs(cummulativeDescent - dp['cummulative_descent']) >= 0.1:
            return False
        if trackPoint.heartRate != dp['heart_rate']:
            return False
        # TODO: cummulative_distance, speed, grade require great circle
        # distance maybe do this later. Maybe not though, because it could be
        # covered in the gpxfile unit test or even comparing the "get" output
        # for cumulative distance

    return True

def validateGpxTrack(gpxFileName, trackPoints):
    activity = parseGpxFile(gpxFileName)
    if activity == None:
        return False

    if len(activity.trackPoints) != len(trackPoints):
        return False
    
    for i in range(len(trackPoints)):
        trackPoint = activity.trackPoints[i]
        tp = trackPoints[i]
        if trackPoint.time != tp['time']:
            return False
        if abs(trackPoint.latitude - tp['latitude']) >= 0.0000001:
            return False
        if abs(trackPoint.longitude - tp['longitude']) >= 0.0000001:
            return False
        if abs(trackPoint.altitude - tp['altitude']) >= 0.1:
            return False

    return True

if __name__ == "__main__":
    gpxFileName = sys.argv[1]
    activity = _parseGpxFile(gpxFileName)
    if activity == None:
        print('Failed to parse GPX file "%s"' % gpxFileName)
    print(json.dumps(activity.map))