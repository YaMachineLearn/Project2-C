import parse
import labelUtil
import time

DATA_FOLDER = "MLDS_HW2_RELEASE_v1/"

TEST_FEATURE_FILENAME = DATA_FOLDER + "fbank/test.ark"
TEST_LABEL_FILENAME = "output/predict.dat"
OUTPUT_CSV_FILENAME = "output/output.kaggle" #predict.csv

SMOOTH_TIMES = 4
SMOOTH_WINDOW_SIZE = 5

print '\nParsing testing utterance names...'
t0 = time.time()
testFeats, testFrameNames = parse.parseTestData(TEST_FEATURE_FILENAME)
utterNames = parse.getUtterNamesFromFrameNames(testFrameNames)
t1 = time.time()
print '...costs ', t1 - t0, ' seconds'

print '\nParsing testing labels...'
t0 = time.time()
utterLabels48 = parse.parseTestLabelsHw2(TEST_LABEL_FILENAME)

print 'Converting and trimming labels(48) into character strings(39)...'
utterLabels39 = [ labelUtil.indices48Toindices39(labels48) for labels48 in utterLabels48]
#smoothing labels
for i in xrange(SMOOTH_TIMES):     #smooth n times
    utterLabels39 = [ labelUtil.smoothLabels(labels, SMOOTH_WINDOW_SIZE) for labels in utterLabels39 ]
utterLabels39Trim = [ labelUtil.trimIndices(labels39) for labels39 in utterLabels39 ]
utterCharStrings = [ labelUtil.indicesToCharString(labels39) for labels39 in utterLabels39Trim ]

print 'Writing CSV file...'
parse.outputPartB(utterNames, utterCharStrings, OUTPUT_CSV_FILENAME)
t1 = time.time()
print '...costs ', t1 - t0, ' seconds'