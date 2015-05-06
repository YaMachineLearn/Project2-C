import parse
import labelUtil
import numpy
import time
from operator import add

TRAIN_FEATURE_FILENAME  	= "Project2-C/MLDS_HW2_RELEASE_v1/fbank/train.ark"
TRAIN_LABEL_FILENAME    	= "Project2-C/MLDS_HW2_RELEASE_v1/label/train.lab"
OUTPUT_FRAMENAME_FILE_NAME	= "parse/frame_name.ark"
OUTPUT_FEATURE_FILE_NAME	= "parse/feature.ark"
OUTPUT_LABEL_FILE_NAME		= "parse/label.lab"


print 'Parsing data...'
t0 = time.time()
trainFeats, trainLabels, trainFrameNames = parse.parseTrainData(TRAIN_FEATURE_FILENAME, TRAIN_LABEL_FILENAME)
trainLabelIndices = labelUtil.labelsToIndices(trainLabels)
t1 = time.time()
print '...costs ', t1 - t0, ' seconds\n'

num = 0
numList = []
frameName = trainFrameNames[0].split('_')[0] + trainFrameNames[0].split('_')[1]

for i in xrange(len(trainFrameNames)):
	currentName = trainFrameNames[i].split('_')[0] + trainFrameNames[i].split('_')[1]
	if currentName == frameName:
		num += 1
	else:
		frameName = currentName
		numList.append(num)
		num = 1
numList.append(num)

parse.outputParse(numList, OUTPUT_FRAMENAME_FILE_NAME)

print 'Writing file...'
t2 = time.time()
parse.outputFeatureParse(trainLabelIndices, trainFeats, OUTPUT_FEATURE_FILE_NAME)
t3 = time.time()
print '...costs ', t3 - t2, ' seconds\n'