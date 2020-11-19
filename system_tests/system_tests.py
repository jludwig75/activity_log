#!/usr/bin/env python3
import os
import unittest
from activitylog import ActivityLog
from gpxchecks import compareGpxFiles, validateGpxPlot, validateGpxTrack


class ActivityLogSystemTest(unittest.TestCase):
    def setUp(self):
        os.system('pkill -f activity_server')
        self.activityLog = ActivityLog()
        self.activityLog.start()
        self.activityLog.deleteAllActivities()

    def tearDown(self):
        self.activityLog.deleteAllActivities()
        self.activityLog.stop()
        del self.activityLog
        os.system('pkill -f activity_server')

    def test_upload(self):
        activityLog = self.activityLog

        # Upload the activity
        activity = activityLog.uploadActivity("../../Big_CC_Loop.gpx")

        # Validate that activity is a valid activity
        self.assertTrue(len(activity.id) > 0)

    def test_list(self):
        activityLog = self.activityLog

        # No activities at start of test
        activityList = activityLog.listActivities()
        self.assertEqual(len(activityList), 0)

        # Upload one activity
        activity1 = activityLog.uploadActivity("../../Big_CC_Loop.gpx")
        activityList = activityLog.listActivities()
        self.assertEqual(len(activityList), 1)
        self.assertEqual(activityList[0].name, "Big CC Loop")
        activity1ID = activityList[0].id

        # Upload a second activity
        activity2 = activityLog.uploadActivity("../../Fun_Long_View_Three_Falls_Forbidden_Loop.gpx")
        activityList = activityLog.listActivities()
        self.assertEqual(len(activityList), 2)
        activity1Found = False
        activity2Found = False
        for activity in activityList:
            if activity.id == activity1ID:
                activity1Found = True
                self.assertEqual(activity.name, "Big CC Loop")
            else:
                activity2Found = True
                self.assertEqual(activity.name, "Fun Long View, Three Falls, Forbidden Loop")
        self.assertTrue(activity1Found and activity2Found)

    def test_get(self):
        activityLog = self.activityLog

        # Uplaod an activity
        uploadedActivity = activityLog.uploadActivity("../../Big_CC_Loop.gpx")

        # Re-get the activity
        activity = activityLog.getActivity(uploadedActivity.id)

        # Validate that activity is a valid activity
        self.assertTrue(len(activity.id) > 0)
        self.assertEqual(activity.name, "Big CC Loop")
        self.assertEqual(activity.startTime, 'October 31, 2020 at 10:35: AM')
        self.assertEqual(activity.duration, '3:42:18')
        self.assertEqual(activity.movingTime, '3:23:38')
        self.assertEqual(activity.totalDistance, 24.2352)
        self.assertEqual(activity.totalAscent, 3992.12)
        self.assertEqual(activity.totalDescent, 3992.45)
        self.assertEqual(activity.averageSpeed, 6.54121)
        self.assertEqual(activity.movingAverageSpeed, 7.14082)
        self.assertEqual(activity.maxSpeed, 21.1191)
        self.assertEqual(activity.averageHeartRate, 0)
        self.assertEqual(activity.maxHeartRate, 0)
        self.assertEqual(activity.averageClimbingGrade, r'7.2984%')
        self.assertEqual(activity.averageDescendingGrade, r'-6.37192%')

    def test_delete(self):
        activityLog = self.activityLog

        activity = activityLog.uploadActivity("../../Big_CC_Loop.gpx")
        activityList = activityLog.listActivities()
        self.assertEqual(len(activityList), 1)
        self.assertEqual(activityList[0].name, "Big CC Loop")

        activity.delete()
        activityList = activityLog.listActivities()
        self.assertEqual(len(activityList), 0)

    def test_download(self):
        activityLog = self.activityLog

        GPX_FILE_NAME = "../../Finding_My_Inner_Mountain_Goat.gpx"
        activity = activityLog.uploadActivity(GPX_FILE_NAME)
        OUTPUT_GPX_FILE_NAME = 'delme_py1.gpx'
        activity.download(OUTPUT_GPX_FILE_NAME)

        # Compare the downloaded GPX file to the original
        self.assertTrue(compareGpxFiles(GPX_FILE_NAME, OUTPUT_GPX_FILE_NAME))

    def test_plot(self):
        activityLog = self.activityLog

        GPX_FILE_NAME = "../../Big_CC_Loop.gpx"
        activity = activityLog.uploadActivity(GPX_FILE_NAME)
        dataPoints = activity.plot()

        # Validate dataPoints
        self.assertTrue(validateGpxPlot(GPX_FILE_NAME, dataPoints))

    def test_track(self):
        activityLog = self.activityLog

        GPX_FILE_NAME = "../../Big_CC_Loop.gpx"
        activity = activityLog.uploadActivity(GPX_FILE_NAME)
        trackPoints = activity.getTrack()

        # Validate trackPoints
        self.assertTrue(validateGpxTrack(GPX_FILE_NAME, trackPoints))

if __name__ == '__main__':
    unittest.main()