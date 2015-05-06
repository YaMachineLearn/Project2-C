"""map_a"""
#DICT_LABEL_INDEX = {'aa': 0, 'el': 14, 'ch': 8, 'ae': 1, 'eh': 13, 'cl': 9, 'ah': 2, 'ao': 3, 'ih': 22, 'en': 15, 'ey': 18, 'aw': 4, 'ay': 6, 'ax': 5, 'er': 17, 'vcl': 42, 'ng': 29, 'iy': 24, 'sh': 35, 'th': 38, 'sil': 36, 'zh': 46, 'w': 44, 'dh': 11, 'v': 43, 'ix': 23, 'y': 45, 'hh': 21, 'jh': 25, 'dx': 12, 'b': 7, 'd': 10, 'g': 20, 'f': 19, 'k': 26, 'm': 28, 'l': 27, 'n': 30, 'uh': 40, 'p': 33, 's': 37, 'r': 34, 't': 39, 'oy': 32, 'epi': 16, 'ow': 31, 'z': 47, 'uw': 41}
#DICT_INDEX_CHAR = {0: 'a', 1: 'b', 2: 'c', 3: 'd', 4: 'e', 5: 'f', 6: 'g', 7: 'h', 8: 'i', 9: 'j', 10: 'k', 11: 'l', 12: 'm', 13: 'n', 14: 'o', 15: 'p', 16: 'q', 17: 'r', 18: 's', 19: 't', 20: 'u', 21: 'v', 22: 'w', 23: 'x', 24: 'y', 25: 'z', 26: 'A', 27: 'B', 28: 'C', 29: 'D', 30: 'E', 31: 'F', 32: 'G', 33: 'H', 34: 'I', 35: 'J', 36: 'K', 37: 'L', 38: 'M', 39: 'N', 40: 'O', 41: 'P', 42: 'Q', 43: 'R', 44: 'S', 45: 'T', 46: 'U', 47: 'V'}

"""map_b"""
DICT_LABEL_INDEX = {'aa': 0, 'el': 14, 'ch': 8, 'ae': 1, 'eh': 13, 'cl': 9, 'ah': 2, 'ao': 3, 'ih': 22, 'en': 15, 'ey': 18, 'aw': 4, 'ay': 6, 'ax': 5, 'er': 17, 'vcl': 43, 'ng': 30, 'iy': 24, 'sh': 36, 'th': 39, 'sil': 37, 'zh': 47, 'w': 44, 'dh': 11, 'v': 42, 'ix': 23, 'y': 45, 'hh': 21, 'jh': 25, 'dx': 12, 'b': 7, 'd': 10, 'g': 20, 'f': 19, 'k': 26, 'm': 28, 'l': 27, 'n': 29, 'uh': 40, 'p': 33, 's': 35, 'r': 34, 't': 38, 'oy': 32, 'epi': 16, 'ow': 31, 'z': 46, 'uw': 41}
DICT_INDEX_CHAR = {0: 'a', 1: 'b', 2: 'c', 3: 'd', 4: 'e', 5: 'f', 6: 'g', 7: 'h', 8: 'i', 9: 'j', 10: 'k', 11: 'l', 12: 'm', 13: 'n', 14: 'o', 15: 'p', 16: 'q', 17: 'r', 18: 's', 19: 't', 20: 'u', 21: 'v', 22: 'w', 23: 'x', 24: 'y', 25: 'z', 26: 'A', 27: 'B', 28: 'C', 29: 'D', 30: 'E', 31: 'F', 32: 'G', 33: 'H', 34: 'I', 35: 'J', 36: 'K', 37: 'L', 38: 'M', 39: 'N', 40: 'O', 41: 'P', 42: 'Q', 43: 'R', 44: 'S', 45: 'T', 46: 'U', 47: 'V'}
DICT_INDEX48_INDEX39 = {0: 0, 1: 1, 2: 2, 3: 0, 4: 4, 5: 2, 6: 6, 7: 7, 8: 8, 9: 37, 10: 10, 11: 11, 12: 12, 13: 13, 14: 27, 15: 29, 16: 37, 17: 17, 18: 18, 19: 19, 20: 20, 21: 21, 22: 22, 23: 22, 24: 24, 25: 25, 26: 26, 27: 27, 28: 28, 29: 29, 30: 30, 31: 31, 32: 32, 33: 33, 34: 34, 35: 35, 36: 36, 37: 37, 38: 38, 39: 39, 40: 40, 41: 41, 42: 42, 43: 37, 44: 44, 45: 45, 46: 46, 47: 36}

LABEL_COUNT = 48
SIL_INDEX = DICT_LABEL_INDEX['sil']

def labelsToIndices(labelList):
    #labelList ex: ['aa', 'sil', 'ch']
    #output ex: [0, 37, 8]
    indices = [ DICT_LABEL_INDEX[label] for label in labelList ]
    return indices

def indicesToChars(indexList):
    #indexList ex: [0, 37, 8]
    #output ex: ['a', 'L', 'i']
    chars = [ DICT_INDEX_CHAR[index] for index in indexList ]
    return chars

def indices48Toindices39(indexList):
    #indexList ex: [0, 1, 2, 3, 4, 5]
    #output ex: [0, 1, 2, 0, 4, 2]
    indices39 = [ DICT_INDEX48_INDEX39[index] for index in indexList ]
    return indices39

def indicesToCharString(indexList):
    #input: [0, 37, 8]
    #output: 'aLi'
    charList = [ DICT_INDEX_CHAR[index] for index in indexList ]
    charString = ''.join(charList)
    return charString

def trimIndices(indexList):
    #input: [37, 37, 1, 1, 1, 37, 37, 8, 8, 37, 37]
    #output: [1, 37, 8]
    #input will not be modified.
    trimmedIndexList = list()

    if not indexList:   #if empty
        return trimmedIndexList

    """ Remove the repeated elements """
    prevElement = indexList[0]
    trimmedIndexList.append(prevElement)
    for element in indexList:
        if element != prevElement:
            trimmedIndexList.append(element)
            prevElement = element

    """ Remove <sil> in the endpoints """
    if trimmedIndexList[0] == SIL_INDEX:
        trimmedIndexList.pop(0)
    if trimmedIndexList[-1] == SIL_INDEX:
        trimmedIndexList.pop(-1)

    return trimmedIndexList

def smoothLabels(indexList, windowSize=5):
    if windowSize < 1:
        print "smoothLabels ereror: windowSize is smaller than 1."
        return None
    if len(indexList) <= windowSize:
        print "smoothLabels ereror: length of indexList is smaller than or equal to windowSize."
        return None

    smoothedLabelList = list()

    for i in xrange( len(indexList) - windowSize + 1 ):
        smoothedLabelList.append( most_common( indexList[i:i+windowSize] ) )

    return smoothedLabelList


def most_common(lst):
    #find most frequently appeared element
    return max(lst, key=lst.count)

if __name__ == '__main__':
    print labelsToIndices(['aa', 'sil', 'ch'])
    print indicesToChars([0, 37, 8])
    print indices48Toindices39([0, 1, 2, 3, 4, 5])
    print indicesToCharString([0, 37, 8])

    print
    labels = ['sil', 'sil', 'sil', 'sil', 'ch', 'ch', 'sil', 'sil', 'sil', 'aa', 'ao', 'aa', 'sil', 'sil']
    print labels
    indices48 = labelsToIndices(labels)
    print indices48
    indices39 = indices48Toindices39(indices48)
    print indices39
    trimmedIndices = trimIndices(indices39)
    print trimmedIndices
    charString = indicesToCharString(trimmedIndices)
    print charString

    print smoothLabels( [1,1,2,2,2,5,2,2,3,3,3,6,6,3,3,3,8,8,8], 3 )