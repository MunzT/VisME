#!/usr/bin/python
# -*- coding: <UTF-8> -*-

"""
This script converts asc files to maf files. It is possible to convert multiple files at once.
asc files can be created from edf files (e.g. from an Eye Link 1000 Eye-tracker) using the EDF2ASC converter available from SR Research.

maf files are used as input file to VisualMicrosaccadesExplorer. They contain general eye tracking information and can contain, amongst others, fixations and microsaccades. In VisME the data can be visually explored.

For the correct format of a .maf see the inputFormat file which can also be opend in VisME.
"""

import sys
import os
import operator
from operator import itemgetter

###############################################################################
# set following value according to the experiment:
pixelsPerDegree = 26.48
###############################################################################
        
        
def writeHeader(outFile, participant, pixelsPerDegree):
    outFile.write('PARTICIPANT %s \n' % participant)
    outFile.write('PIXELSPERDEGREE %s \n' % pixelsPerDegree)


def writeTrial(outFile, trialId,minX, minY, maxX, maxY, frequency, imagePath, fixations, gazeData, events):

    # end reached: write data to file...
    outFile.write('TRIAL %s \n' % trialId)
    outFile.write('COORDS %s %s %s %s \n' % (minX, minY, maxX, maxY)) # COORDS 0 0 1279 959
    outFile.write('FREQ %s \n' % frequency) # FREQ 1000

    if imagePath:
        # STIMULUS stimulus file
        outFile.write('STIMULUS %s \n' % imagePath)

    # write fixations
    for fixation in fixations:
        outFile.write('F %s %s %s %s %s \n' % (fixation[0], fixation[1], fixation[2], fixation[3], fixation[4]))

    # write raw data
    for gaze in gazeData:
        outFile.write('%s %s %s %s \n' % (gaze[0], gaze[1], gaze[2], gaze[3]))
    
    # write event informations
    for event in events:
        outFile.write('E %s %s %s \n' % (event[0], event[1], event[2])) # T name start duration

    outFile.write('ENDTRIAL\n')


def isInt(astring):
    try:
        int(astring)
    except ValueError:
        return 0
    else:
        return 1
        
def addInterpolatedFixation(gazeData, start, end, newFixations):
    # average position
    samples = [[e[2], e[3]] for e in gazeData if ((e[0] == "R" or e[0] == "L") and start <= e[1] <= end)]
    if len(samples) > 0:
        x = sum([e[0] for e in samples]) / float(len(samples))
        y = sum([e[1] for e in samples]) / float(len(samples))
    
        # average y position
        newFixations.append(["B", start, end - start, x, y])
        
        
# Function to convert .asc files to .maf files.
def convertAscToMaf(files):

    messages = []
            
    # for each file...
    for fileName in files:
        if not fileName.lower().endswith(".asc"):
            continue

        participantFileName = os.path.splitext(fileName)[0] + ".maf"
        print("Converting %s to %s" % (fileName, participantFileName))

        with open(participantFileName, 'w') as outFile:
        
            # PARTICIPANT participant name or file name of original file
            participant = os.path.basename(fileName)[3:6] #exp002.asc 
            writeHeader(outFile, participant, pixelsPerDegree)

            with open(fileName, 'r') as inFile:
                trialId = 0
                imagePath = ""
                
                trialStarted = False
                fixations = []
                microsaccades = []
                gazeData = []
                
                useTrial = False
                firstLeftIndex = -1
                firstRightIndex = -1

                # for each line...
                for line in inFile:
                    # split on whitespace
                    parts = line.split()

                    if len(parts) < 2:
                        continue

                    # right or left eye
                    if parts[0] == "START":
                        rightSecond = (parts[3] == "RIGHT")
                        leftFirst = (parts[2] == "LEFT")
                        if (not rightSecond and  not leftFirst):
                            print ("Wrong order")
                            return
                        
                    # frequency
                    elif parts[0] == "SAMPLES":
                        found = False
                        for el in parts:
                            if found:
                                frequency = float(el)
                                break

                            if el == "RATE":
                                found = True

                    # fixations
                    elif trialStarted and parts[0] == "EFIX" and (parts[1] == "R" or parts[1] == "L"):
                        # EFIX R   18872272    18872628    357      641.2      482.5       1203
                        durMinimize = 0
                        firstIndex = 0
                        if (parts[1] == "R" and float(parts[2]) < firstRightIndex):
                            durMinimize = firstRightIndex - float(parts[2])
                            firstIndex = firstRightIndex
                        if (parts[1] == "L" and float(parts[2]) < firstLeftIndex):
                            durMinimize = firstLeftIndex - float(parts[2])
                            firstIndex = firstLeftIndex
                        
                        time = int(max(float(firstIndex), float(parts[2])) * frequency / 1000) # timestamp parts[2] in milliseconds -> index
                        duration = int((float(parts[4]) - durMinimize) * frequency / 1000)
                        x = float(parts[5])
                        y = float(parts[6])
                        fixations.append([parts[1], time, duration, x, y])

                    # raw data
                    elif trialStarted and  parts[0].isdigit():
                        # 19073307      715.2      412.9     1013.0    ...
                        time = int(float(parts[0]) * frequency / 1000) # timestamp parts[0] in milliseconds -> index
                        left = False
                        right = False
                        try:
                            x1 = float(parts[1])
                            y1 = float(parts[2])
                            #pupilSize1 = float(parts[3])
                            
                            gazeData.append(["L", time, x1, y1])
                            left = True
                            if (firstLeftIndex == -1):
                                firstLeftIndex = time
                        except:
                            # ignore lines starting with e.g. "."
                            pass

                        try:
                            x2 = float(parts[4])
                            y2 = float(parts[5])
                            #pupilSize2 = float(parts[6])

                            gazeData.append(["R", time, x2, y2])
                            right = True
                            if (firstRightIndex == -1):
                                firstRightIndex = time
                        except:
                            # ignore lines starting with e.g. "."
                            pass
                            
                        if left and right:
                            gazeData.append(["B", time, (x1 + x2) / 2, (y1 + y2) / 2])

                    elif parts[0] == "MSG":
                        # screen coordinates
                        if len(parts) >= 7 and parts[2] == "GAZE_COORDS":
                            # MSG    18872264 GAZE_COORDS 0.00 0.00 1279.00 959.00
                            minX = float(parts[3])
                            minY = float(parts[4])
                            maxX = float(parts[5])
                            maxY = float(parts[6])
                            
                        elif len(parts) > 3:
                            # trial id
                            if parts[2] == "TRIALID":
                                trialId = parts[3]
                                
                            # event showbild for image path, event and start trial signal
                            if parts[3] == "showbild":
                                imagePath = parts[4]
                                trialStarted = True 
                                
                            # only trials with showdisaku, showdisvis and showdisboth are used
                            elif (parts[3] == "showdisaku" or parts[3] == "showdisvis" or parts[3] == "showdisboth"):
                                useTrial = True
                                
                            # msg hidebild as signal to end of trial
                            elif parts[3] == "hidebild":
                                # use only trials with defined events (fix found; showdisaku; showdisvis; showdiboth)
                                if useTrial:
                                    # sort fixations
                                    fixations = sorted(fixations, key=itemgetter(1))
                                    
                                    # R/L time duration x y
                                    start = -1
                                    end = -1
                                    # fixations are already sorted by start time
                                    newFixations = []
                                    for f in fixations:
                                        # trim fixations at beginning of trial
                                        startNew = f[1]
                                        endNew = startNew + f[2] - 1
                                        
                                        if (start != -1 or end != -1):
                                            if (startNew <= end):
                                                end = max(end, endNew)
                                            else:
                                                addInterpolatedFixation(gazeData, max(max(firstLeftIndex, firstLeftIndex), start), end, newFixations)
                                                start = -1
                                                end = -1
                                        else:
                                            start = startNew
                                            end = endNew
                                            
                                    if (start != -1 and end != -1):
                                        addInterpolatedFixation(gazeData, max(max(firstLeftIndex, firstLeftIndex), start), end, newFixations)
                                        
                                    fixations = fixations + newFixations
                                
                                    writeTrial(outFile, trialId, minX, minY, maxX, maxY, frequency, imagePath, fixations, gazeData, [])

                                # reset values
                                fixations.clear()
                                gazeData.clear()
                                imagePath = ""
                                trialStarted = False
                                leftFirst = False
                                leftSecond = False
                                rightFirst = False
                                rightSecond = False
                                useTrial = False
                                firstLeftIndex = -1
                                firstRightIndex = -1

if __name__ == '__main__':

    if (len(sys.argv) < 2):
        print("You did not specify files for conversion to .maf")
    else:
        if (len(sys.argv) == 2 and os.path.exists(os.path.dirname(sys.argv[1]))):
            print(os.listdir(sys.argv[1]))
            list = [os.path.join(sys.argv[1], x) for x in os.listdir(sys.argv[1])]
            convertAscToMaf(list)
        else:
            convertAscToMaf(sys.argv[1:])
