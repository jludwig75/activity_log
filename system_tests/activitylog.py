import signal
import os
import re
import subprocess

LIST_CMD_HEADER_RE = re.compile(r'Retrieved (\d+) activities\:')

#   5fb330eca7180fe8947a8c63 - "Big CC Loop": 24.2352 miles, 3992.12 feet ascent
LIST_CMD_EMTRY_RE = re.compile(r'\s+(\w+) \- (\d+) "(.*)"')

UPLOAD_CMD_RE = re.compile(r'Successfully uploaded activity file ".*" as activity (\w+)')

def runCmd(cmd):
    cmd = '../bin/activities ' + cmd
    return subprocess.check_output(cmd.split()).decode("utf-8")

class Activity:
    def __init__(self, id, name):
        self.id = id
        self.name = name
        self.startTime = 0

    def delete(self):
        runCmd('delete ' + self.id)

    def download(self, activityFileName):
        runCmd('download %s %s ' % (self.id, activityFileName))

    def getTrack(self):
        output = runCmd('track %s' % self.id)
        if output == None or len(output) == 0:
            return []
        lines = output.split('\n')
        fields = lines[0].split(',')
        entries = []
        for line in lines[1:]:
            if len(line) == 0:
                continue
            parts = line.split(',')
            parts = [ float(part) for part in parts ]
            if len(parts) != len(fields):
                continue
            entry = {}
            for i in range(len(fields)):
                entry[fields[i]] = parts[i] if fields[i] != 'time' else int(parts[i])
            entries.append(entry)
        return entries

    def plot(self):
        output = runCmd('plot %s' % self.id)
        if output == None or len(output) == 0:
            return []
        lines = output.split('\n')
        fields = lines[0].split(',')
        entries = []
        for line in lines[1:]:
            if len(line) == 0:
                continue
            parts = line.split(',')
            parts = [ float(part) for part in parts ]
            if len(parts) != len(fields):
                continue
            entry = {}
            for i in range(len(fields)):
                entry[fields[i]] = parts[i] if fields[i] != 'time' else int(parts[i])
            entries.append(entry)
        return entries

class ActivityLog:
    def start(self):
        self.process = subprocess.Popen(['../bin/activity_server'], stdout=subprocess.PIPE, preexec_fn=os.setsid) 

    def stop(self):
        os.killpg(self.process.pid, signal.SIGTERM)
        self.process.wait()
        self.process.stdout.close()

    def deleteAllActivities(self):
        runCmd('deleteall')

    def uploadActivity(self, activityFileName):
        output = runCmd('upload ' + activityFileName)
        m = UPLOAD_CMD_RE.match(output)
        if m == None:
            return None
        return Activity(m.groups()[0], 'NO_NAME_RETRIEVED')

    def listActivities(self):
        output = runCmd('list')
        lines = output.split('\n')
        if (len(lines) == 0 or len(lines[0]) == 0):
            return []
        m = LIST_CMD_HEADER_RE.match(lines[0])
        if m == None:
            return []
        if len(m.groups()) != 1:
            return []
        activityCount = int(m.groups()[0])
        if len(lines) - 1 < activityCount:
            activityCount = len(lines) - 1
        activities = []
        for line in lines[1:]:
            m = LIST_CMD_EMTRY_RE.match(line)
            if m != None:
                id, startTime, name = m.groups()
                activity = Activity(id, name)
                activity.startTime = startTime
                activities.append(activity)
        return activities

    def getActivity(self, activityId):
        output = runCmd('get ' + activityId)
        if output == None:
            return None
        lines = output.split('\n')
        if len(lines) < 12:  # Currently 12 known fields
            return None
        fields = lines[0].strip().split()
        id = fields[1]
        activity = Activity(id, 'NOT_YET_SET')
        for line in lines[1:]:
            if len(line) == 0:
                continue
            parts = line.strip().split(':')
            key = parts[0]
            value = parts[1].strip()
            setattr(activity, key, self._strToValue(value))
        return activity
    def _strToValue(self, value):
        number = _strToNumber(value)
        if number != None:
            return number
        return value
    
def _isStringNumeric(s):
    for c in s:
        if not c in "0123456789.-":
            return False
    parts = s.split('.')
    return len(parts) == 1 or len(parts) == 2   # Only 1 .

def _isStringInteger(s):
    for c in s:
        if not c in "0123456789-":
            return False
    return True

def _strToNumber(s):
    s = s.strip()
    if _isStringInteger(s):
        return int(s)
    if _isStringNumeric(s):
        return float(s)
    parts = s.split()
    if len(parts) == 2 and _isStringNumeric(parts[0]) and not _isStringNumeric(parts[1]):
        return _strToNumber(parts[0])   # yes recursion, but it will only ever be one leve
    return None

