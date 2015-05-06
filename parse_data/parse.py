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
            if line.rstrip():
                lineList = line.rstrip().split(" ")
                testFrameNames.append( lineList.pop(0) )
                testFeats.append( [ float(ele) for ele in lineList ] )
    
    return (testFeats, testFrameNames)

def outputPartAAsCsv(obser, transition, TEST_CSV_FILE_NAME):
    with open(TEST_CSV_FILE_NAME, 'w') as testCsvFile:
        testCsvFile.write("id,feature\n")
        for i in xrange(len(obser)):
            testCsvFile.write('faem0_si1392_' + str(i) + ',' + str(obser[i]) + '\n')
        for i in xrange(len(transition)):
            testCsvFile.write('faem0_si1392_' + str(i + len(obser)) + ',' + str(transition[i]) + '\n')

def trimFrameNames(frameNameList):
    #input: ['fadg0_si1279_1', 'fadg0_si1279_2', ..., 'fadg0_si1909_1', 'fadg0_si1909_2', ...]
    #output: ['fadg0_si1279', 'fadg0_si1909', ...]
    #need to make sure the input has no empty string inside(in the end)
    trimmedNameList = list()

    prevSplitName = frameNameList[0].split('_') #should .strip()?
    splitName = prevSplitName   #point to same list, not copy. doesn't matter in this case
    trimmedNameList.append(splitName[0] + '_' + splitName[1])
    
    for frameName in frameNameList:
        splitName = frameName.split('_')
        if splitName[0] != prevSplitName[0] or splitName[1] != prevSplitName[1]:
            trimmedNameList.append(splitName[0] + '_' + splitName[1])
            prevSplitName = splitName
            # prevSplitName is updated when the first two elements
            # of prevSplitName and splitName are different.
            # so the third element is always '1'
    return trimmedNameList

def getFrameNameAndNumber(frameName):
    regex = re.compile(ur'(.+_.+)_(\d+)')
    result = re.search(regex, frameName)
    return (result.group(1), result.group(2))

def outputPartB(trimmedTestFrameNames, testLabelStrings, OUTPUT_CSV_FILE_NAME):
    with open(OUTPUT_CSV_FILE_NAME, 'w') as testCsvFile:
        testCsvFile.write("id,phone_sequence")
        for i in xrange(len(trimmedTestFrameNames)):
            testCsvFile.write('\n' + trimmedTestFrameNames[i] + ',' + testLabelStrings[i])

def outputUtteranceCount(trainParameter, OUTPUT_FILE_NAME):
    with open(OUTPUT_FILE_NAME, 'w') as testFile:
        for i in xrange(len(trainParameter)):
            testFile.write(str(trainParameter[i]))
            if i != len(trainParameter) - 1:
                testFile.write('\n')

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