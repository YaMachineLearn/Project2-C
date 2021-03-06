import re

def parseTrainData(TRAIN_FEATURE_FILENAME, TRAIN_LABEL_FILENAME):
    trainFeats = []
    trainLabels = []
    trainFrameNames = []

    #parse training features
    with open(TRAIN_FEATURE_FILENAME) as trainFeatFile:
        for line in trainFeatFile:
            strippedLine = line.rstrip()
            if strippedLine:   #not empty after strip
                lineList = strippedLine.split(' ')
                trainFrameNames.append( lineList.pop(0) )
                trainFeats.append( [ float(ele) for ele in lineList ] )

    #parse training labels
    with open(TRAIN_LABEL_FILENAME) as trainLabelFile:
        for line in trainLabelFile:
            strippedLine = line.rstrip()
            if strippedLine:
                lineList = strippedLine.split(",")
                trainLabels.append(lineList[1])
    
    return (trainFeats, trainLabels, trainFrameNames)

def parseTestData(TEST_FEATURE_FILENAME):
    testFeats = []
    testFrameNames = []

    #parse testing features
    with open(TEST_FEATURE_FILENAME) as testFeatFile:
        for line in testFeatFile:
            strippedLine = line.rstrip()
            if strippedLine:
                lineList = strippedLine.split(" ")
                testFrameNames.append( lineList.pop(0) )
                testFeats.append( [ float(ele) for ele in lineList ] )
    
    return (testFeats, testFrameNames)

def parseHw1Csv(CSV_FILENAME):
    frameNames = list()
    labels = list()

    with open(CSV_FILENAME) as csvFile:
        csvFile.readline()
        for line in csvFile:
            strippedLine = line.rstrip()
            if strippedLine:
                lineList = strippedLine.split(",")
                frameNames.append(lineList[0])
                labels.append(lineList[1])

    return (frameNames, labels)

def outputPartAAsCsv(obser, transition, TEST_CSV_FILE_NAME):
    with open(TEST_CSV_FILE_NAME, 'w') as testCsvFile:
        testCsvFile.write("id,feature\n")
        for i in xrange(len(obser)):
            testCsvFile.write('faem0_si1392_' + str(i) + ',' + str(obser[i]) + '\n')
        for i in xrange(len(transition)):
            testCsvFile.write('faem0_si1392_' + str(i + len(obser)) + ',' + str(transition[i]) + '\n')

def getUtterNamesFromFrameNames(frameNameList):
    #input: ['fadg0_si1279_1', 'fadg0_si1279_2', ..., 'fadg0_si1909_1', 'fadg0_si1909_2', ...]
    #output: ['fadg0_si1279', 'fadg0_si1909', ...]
    #need to make sure the input has no empty string inside(in the end)
    utterNameList = list()

    prevUtterName, frameNum = getFrameNameAndNumber(frameNameList[0])
    utterNameList.append(prevUtterName)
    
    for frameName in frameNameList:
        utterName, frameNum = getFrameNameAndNumber(frameName)
        if utterName != prevUtterName:
            utterNameList.append(utterName)
            prevUtterName = utterName

    return utterNameList

def getFrameNameAndNumber(frameName):   #should be getUtterNameAndNumber
    regex = re.compile(ur'(.+_.+)_(\d+)')
    result = re.search(regex, frameName)
    return (result.group(1), result.group(2))

def parseTestLabelsHw2(TEST_LABEL_FILENAME):
    #output: [ [37,37,37,...], [37,37,37,...], ... ]
    testLabelList = list()
    with open(TEST_LABEL_FILENAME) as labelFile:
        for line in labelFile:
            strippedLine = line.rstrip()
            if strippedLine:
                lineList = strippedLine.split(' ')
                testLabelList.append( [ int(ele) for ele in lineList ] )
    return testLabelList


def outputPartB(utterNames, utterCharStrings, OUTPUT_CSV_FILENAME):
    #utterNames: ['fadg0_si1279', 'fadg0_si1909', ...]
    #utterCharStrings: ['HIrwLA...', 'vLJLtH...', ...]
    with open(OUTPUT_CSV_FILENAME, 'w') as testCsvFile:
        testCsvFile.write("id,phone_sequence")
        for i in xrange(len(utterNames)):
            testCsvFile.write('\n' + utterNames[i] + ',' + utterCharStrings[i])

def countUtterLength(frameNameList):
    frameCounter = 0
    utterCountList = list()
    prevUtterName, useless = getFrameNameAndNumber(frameNameList[0])

    for frameName in frameNameList:
        utterName, useless = getFrameNameAndNumber(frameName)
        if utterName != prevUtterName:
            utterCountList.append(frameCounter)
            prevUtterName = utterName
            frameCounter = 1
        else:
            frameCounter += 1
    utterCountList.append(frameCounter)
    return utterCountList

def outputUtterCount(utterCounts, OUTPUT_FILE_NAME):
    #utterCounts: [202, 153, 345, ...]
    with open(OUTPUT_FILE_NAME, 'w') as outputFile:
        outputFile.write(str(len(utterCounts)))     #first line: total number of utterances
        for utterLen in utterCounts:
            outputFile.write('\n' + str(utterLen))
        #for i in xrange(len(trainParameter)):
        #    testFile.write(str(trainParameter[i]))
        #    if i != len(trainParameter) - 1:
        #        testFile.write('\n')

def outputFeatureParseToC(trainParameter1, trainParameter2, OUTPUT_FILE_NAME):
    with open(OUTPUT_FILE_NAME, 'w') as testFile:
        for i in xrange(len(trainParameter1)):
            testFile.write(str(trainParameter1[i]))
            testFile.write('\n')
            for j in xrange(len(trainParameter2[i])):
                if j == len(trainParameter2[i]) - 1:
                    testFile.write(str(trainParameter2[i][j]))
                else:
                    testFile.write(str(trainParameter2[i][j]) + ' ')
            if i != len(trainParameter2) - 1:
                testFile.write('\n')

def outputTestParseToC(trainParameter, OUTPUT_FILE_NAME):
    with open(OUTPUT_FILE_NAME, 'w') as testFile:
        for i in xrange(len(trainParameter)):
            testFile.write(str(-1))
            testFile.write('\n')
            for j in xrange(len(trainParameter[i])):
                if j == len(trainParameter[i]) - 1:
                    testFile.write(str(trainParameter[i][j]))
                else:
                    testFile.write(str(trainParameter[i][j]) + ' ')
            if i != len(trainParameter) - 1:
                testFile.write('\n')

if __name__ == '__main__':
    print getFrameNameAndNumber('adsf_dfs_123')
    print getUtterNamesFromFrameNames(['fadsf_asdfads_1', 'fadsf_asdfads_12', 'fadsf_a_1', 'fadsf_a_1'])
