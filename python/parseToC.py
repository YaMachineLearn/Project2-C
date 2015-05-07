import parse
import labelUtil
import numpy
import time
from operator import add

TRAIN_FEATURE_FILENAME  	= "MLDS_HW2_RELEASE_v1/fbank/train.ark"
TRAIN_LABEL_FILENAME    	= "MLDS_HW2_RELEASE_v1/label/train.lab"
TEST_FEATURE_FILENAME   	= "MLDS_HW2_RELEASE_v1/fbank/test.ark"

OUTPUT_TRAIN_UTTER_COUNT_FILENAME	= "parse_data/train_utterance_count.ark"
OUTPUT_TRAIN_LABEL_FEAT_FILENAME		= "parse_data/train_label_feature.ark"
OUTPUT_TEST_UTTER_COUNT_FILENAME   = "parse_data/test_utterance_count.ark"
OUTPUT_TEST_LABEL_FEAT_FILENAME		= "parse_data/test_n1_feature.ark"


print 'Parsing data...'
t0 = time.time()
trainFeats, trainLabels, trainFrameNames = parse.parseTrainData(TRAIN_FEATURE_FILENAME, TRAIN_LABEL_FILENAME)
trainLabelIndices = labelUtil.labelsToIndices(trainLabels)
testFeats, testFrameNames = parse.parseTestData(TEST_FEATURE_FILENAME)
t1 = time.time()
print '...costs ', t1 - t0, ' seconds\n'

print 'Writing files...'
t2 = time.time()
parse.outputUtterCount(parse.countUtterLength(trainFrameNames), OUTPUT_TRAIN_UTTER_COUNT_FILENAME)
parse.outputUtterCount(parse.countUtterLength(testFrameNames), OUTPUT_TEST_UTTER_COUNT_FILENAME)
parse.outputFeatureParseToC(trainLabelIndices, trainFeats, OUTPUT_TRAIN_LABEL_FEAT_FILENAME)
parse.outputTestParseToC(testFeats, OUTPUT_TEST_LABEL_FEAT_FILENAME)
t3 = time.time()
print '...costs ', t3 - t2, ' seconds\n'