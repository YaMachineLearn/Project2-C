import parse
import labelUtil
import numpy
import time
from operator import add

TRAIN_FEATURE_FILENAME  	= "MLDS_HW2_RELEASE_v1/fbank/train.ark"
TRAIN_LABEL_FILENAME    	= "MLDS_HW2_RELEASE_v1/label/train.lab"
OUTPUT_UTTERANCE_FILENAME	= "utterance_count.ark"
OUTPUT_FEATURE_FILENAME		= "label_feature.ark"


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

parse.outputParse(numList, OUTPUT_UTTERANCE_FILENAME)

print 'Writing file...'
t2 = time.time()
parse.outputFeatureParse(trainLabelIndices, trainFeats, OUTPUT_FEATURE_FILENAME)
t3 = time.time()
print '...costs ', t3 - t2, ' seconds\n'