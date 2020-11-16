# TODO: Implement this
class Activity:
    def __init__(self):
        self.id = ''
        self.name = ''
        self.startTime = 0

    def delete(self):
        pass

    def download(self, activityFileName):
        pass

    def getTrack(self):
        return []

    def plot(self):
        return []

# TODO: Implement this
class ActivityLog:
    def start(self):
        pass

    def stop(self):
        pass

    def deleteAllActivities(self):
        pass

    def uploadActivity(self, activityFileName):
        return Activity()

    def listActivities(self):
        return []

    def getActivity(self, activityId):
        return Activity()
