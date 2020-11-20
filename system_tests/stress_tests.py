#!/usr/bin/env python3
import os
import re
import threading
import unittest
import sys
from activitylog import ActivityLog
from gpxchecks import compareGpxFiles, validateGpxPlot, validateGpxTrack


def listGpxFiles(path):
    return [ os.path.join(path, fileName) for fileName in os.listdir(path) if fileName.lower().endswith('.gpx') ]

class Uploader:
    TEST_FILE_PATH = '../../test_files/'
    def __init__(self, activityLog):
        self.activityLog = activityLog
    def countTestGpxFiles(self):
        return len(listGpxFiles(Uploader.TEST_FILE_PATH))
    def run(self):
        print('Uploader: uploading files ...')
        for gpxFileName in listGpxFiles(Uploader.TEST_FILE_PATH):
            # print('Uploader: uploading \"%s\"' % gpxFileName)
            self.activityLog.uploadActivity(gpxFileName)

class Downloader:
    def __init__(self, activityLog, filesToExpect):
        self.activityLog = activityLog
        self.filesToExpect = filesToExpect
    def run(self):
        while True:
            # print('Downloader: downloading GPX files ...')
            self.activityLog.downloadAllActivities()
            activitiesDownloaded = len(listGpxFiles('.'))
            print('Downloader: downloaded %u activities' % activitiesDownloaded)
            if activitiesDownloaded == self.filesToExpect:
                break

class Plotter:
    def __init__(self, activityLog, filesToExpect):
        self.activityLog = activityLog
        self.filesToExpect = filesToExpect
    def run(self):
        while True:
            #print('Plotter: listing activities')
            activities = self.activityLog.listActivities()
            activityCount = len(activities)
            # print('Plotter: plotting %u activities ...' % activityCount)
            for activity in activities:
                #print('Plotter: plotting activity %s' % activity.id)
                trackPoints = activity.getTrack()
            print('Plotter: plotted %u activities' % activityCount)
            if activityCount == self.filesToExpect:
                break

class Tracker:
    def __init__(self, activityLog, filesToExpect):
        self.activityLog = activityLog
        self.filesToExpect = filesToExpect
    def run(self):
        while True:
            #print('Tracker: listing activities')
            activities = self.activityLog.listActivities()
            activityCount = len(activities)
            # print('Tracker: tracking %u activities ...' % activityCount)
            for activity in activities:
                #print('Tracker: getting track points for activity %s' % activity.id)
                dataPoints = activity.plot()
            print('Tracker: tracked %u activities' % activityCount)
            if activityCount == self.filesToExpect:
                break

class ActivityLogSystemTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        super(ActivityLogSystemTest, cls).setUpClass()
        os.system('pkill -f activity_server')

    @classmethod
    def tearDownClass(cls):
        os.system('pkill -f activity_server')

    def setUp(self):
        os.system('rm -f *.gpx')
        self.activityLog = ActivityLog()
        self.activityLog.start()
        self.activityLog.deleteAllActivities()
    
    def tearDown(self):
        self.activityLog.deleteAllActivities()
        self.activityLog.stop()
        del self.activityLog
        os.system('rm -f *.gpx')

    def test_stress(self):
        uploader = Uploader(self.activityLog)
        filesToExpect = uploader.countTestGpxFiles()

        downloader = Downloader(self.activityLog, filesToExpect)
        downloaderThread = threading.Thread(target=downloader.run)
        downloaderThread.start()

        plotter = Plotter(self.activityLog, filesToExpect)
        plotterThread = threading.Thread(target=plotter.run)
        plotterThread.start()

        tracker = Tracker(self.activityLog, filesToExpect)
        trackerThread = threading.Thread(target=tracker.run)
        trackerThread.start()

        uploader.run()

        downloaderThread.join()
        plotterThread.join()
        trackerThread.join()

        self._validateDownloadedGpxFiles(filesToExpect)

    ''' Hopefully get more concurrency without disk activity for downloads '''
    def test_stressWithoutDownloader(self):
        uploader = Uploader(self.activityLog)
        filesToExpect = uploader.countTestGpxFiles()

        plotter = Plotter(self.activityLog, filesToExpect)
        plotterThread = threading.Thread(target=plotter.run)
        plotterThread.start()

        tracker = Tracker(self.activityLog, filesToExpect)
        trackerThread = threading.Thread(target=tracker.run)
        trackerThread.start()

        uploader.run()

        plotterThread.join()
        trackerThread.join()

        # Now download the activities to validate them
        print('Downloading activities')
        self.activityLog.downloadAllActivities()

        self._validateDownloadedGpxFiles(filesToExpect)


    def _validateDownloadedGpxFiles(self, filesToExpect):
        print('Vaildating downloaded files ...')
        DOWNLOADED_GPX_FILE_RE = re.compile(r'[\./\w\_]+\_(\w+).gpx')
        fileNumber = 1
        for gpxFileName in listGpxFiles('.'):
            print('.', end = '')
            sys.stdout.flush()
            # print('Validating file %u of %u \"%s\"' % (fileNumber, filesToExpect, gpxFileName))
            m = DOWNLOADED_GPX_FILE_RE.match(gpxFileName)
            self.assertTrue(m != None)
            self.assertEqual(len(m.groups()), 1)
            activityId = m.groups()[0]

            activity = self.activityLog.getActivity(activityId)
            self.assertTrue(activity != None)

            dataPoints = activity.plot()
            self.assertTrue(validateGpxPlot(gpxFileName, dataPoints))

            trackPoints = activity.getTrack()
            self.assertTrue(validateGpxTrack(gpxFileName, trackPoints))

            fileNumber += 1
        print('')

if __name__ == '__main__':
    unittest.main()