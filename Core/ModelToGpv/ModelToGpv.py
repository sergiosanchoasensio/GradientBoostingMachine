print ("Version: 23-10-2014 23.13h")

#Open BinaryTree.txt
fo = open( "Input.txt", "r" )
array = []
#Split it line by line
for line in fo:
    array.append( line )
fo.close()

#Data variables
feature = "NA"
splitPoint = -999
sumOfWeightsAndErrors = -999
expectedValue = -999
isLeaf = 1;

#Structure
dataStructure = {}

#Items
fLine = 0;
lLine = len(array);

print("Blank", array.count('\n'))

#Current line
#petallength <= 2.45 : 0.00 (w:50.00 / e:0.00)

for index in range(fLine,lLine):
    print("The current array: ", array[index])

    #Control variables
    numberOfBlankSpaces = 0
    fCheckpoint = 0
    lCheckpoint = 0

    #Data variables
    feature = "NA"
    splitPoint = -999
    sumOfWeightsAndErrors = ''
    isLeaf = 1;

    #1. Delete blank spaces from the line of text
    array[index] = array[index].replace(' ', '')

    #2. Look for the depth
    depth = array[index].count("|")
    #print ("depth:", depth)

    if depth > 0:
        lCheckpoint = array[index].rfind('|')
        lCheckpoint += 1

    #3. Look for the first character '<=' or '>' in order to copy the feature name
    fCheckpoint = array[index].find(">")
    if fCheckpoint < 0: #If it doesn't exist, return -1
        fCheckpoint = array[index].find("<=")
        numberOfBlankSpaces = 2
    else:
        numberOfBlankSpaces = 1

    feature = array[index][lCheckpoint:fCheckpoint]
    #print("feature:",feature)

    #4. Only the non leaf nodes have ':' 
    lCheckpoint = array[index].find(":")
    if lCheckpoint > 0:
        #4.1. Get the split point
        splitPoint = array[index][fCheckpoint+numberOfBlankSpaces:lCheckpoint]
        #print("splitPoint:",splitPoint)

        #4.2. Get the expected value
        fCheckpoint = array[index].find(":")
        lCheckpoint = array[index].find("(")
        expectedValue = array[index][fCheckpoint+1:lCheckpoint]
        #print("expectedValue:",expectedValue)
        
        #4.3. Get the sum of weights and errors
        fCheckpoint = array[index].find("(")
        lCheckpoint = array[index].find(")")
        sumOfWeightsAndErrors = array[index][fCheckpoint:lCheckpoint+1]
        #print("sumOfWeightsAndErrors:",sumOfWeightsAndErrors)
        isLeaf = 1
    else:
        #4.4. Get the split point
        splitPoint = array[index][fCheckpoint+numberOfBlankSpaces:lCheckpoint]
        #print("splitPointLeaf:",splitPoint)
        isLeaf = 0
    #print ("isLeaf:", isLeaf)

    dataStructure[index,0] = depth;
    dataStructure[index,1] = feature;
    dataStructure[index,2] = splitPoint;
    dataStructure[index,3] = expectedValue;
    dataStructure[index,4] = sumOfWeightsAndErrors;
    dataStructure[index,5] = isLeaf;
    
    dataStructure[index,6] = 0; #repetitions Feature

    fCheckpoint = array[index].rfind("<=")
    if fCheckpoint <= 0:
        dataStructure[index,7] = ">"
    else:
        dataStructure[index,7] = "<="

    dataStructure[index,8] = -1; #Repetitions expected value
    
#6. Control repeated Expected values
lIndex = 0
auxValue = 0
for index in range(fLine,lLine):
    if (dataStructure[index,5] == 1): # Para todos los nodos hoja
        auxValue = dataStructure[index,3]
        for j in range(fLine,lLine):
            if (auxValue == dataStructure[j,3]):
                dataStructure[j,8] += 1
        dataStructure[index,3] = str(dataStructure[index,3]) + "(" + str(dataStructure[index,8]) + ")"

#7. Rename when necessary (in case of having the same name in that level)
#We want to transform, petallength to 0-0-petallength, where the first 0 is the depth and the second the number of repetitions of that feature in these level
lIndex = 0
repetition = 0
#Guardo la nova feature
for index in range(fLine,lLine):
    for lIndex in range(fLine,lLine):
        #if =depth =feature
        if (dataStructure[index,0] == dataStructure[lIndex,0] and dataStructure[index,1] == dataStructure[lIndex,1]):
            repetition += 1
            if (repetition == 2):
                repetition = 0
                dataStructure[index,6] += 1
    if (dataStructure[index,6] > 0 ):
        dataStructure[index,6] -= 1    

#Guardem en un array les posicions on són > 0
auxIndex = []
casilla = 0
for index in range(fLine,lLine):
    if (dataStructure[index,6] > 0):
        auxIndex.append(index)

numLRTree = 0
#Ajuntem de 2 en 2 e incrementem per a etiquetar repetits!
for index in range(0,len(auxIndex),2):
    dataStructure[auxIndex[index],6] += numLRTree
    dataStructure[auxIndex[index+1],6] += numLRTree
    numLRTree += 1

#Per a totes les features
    #str(dataStructure[index,6]) = -1 si no E
for index in range(fLine,lLine):
    dataStructure[index,1] = str(dataStructure[index,0]) + '-' + dataStructure[index,1] + '(' + str(dataStructure[index,6]) + ')'

#5. Solve when is not leaf >>> WARNIIIIIING
lIndex = 0
 
for index in range(fLine,lLine):
    #Not leaf
    if (dataStructure[index,5] == 0):        
        #Desde index fins abaix
        for lIndex in range(index,lLine):
            #Si =depth+1 =sign, veiem fills
            if (dataStructure[index,0] + 1 == dataStructure[lIndex,0]):
                #Copy the 'feature' to the 'expected value'
                    dataStructure[index,3] = dataStructure[lIndex,1]
                    break

#6. Build the output string
oString = "digraph G { \n"
oString += "graph [splines=line, nodesep=1] \n"
for index in range(0,lLine): 
    oString += '\"' + dataStructure[index,1] + '\"'
    oString += "-> \"" + str(dataStructure[index,3]) + '\"' + ' ' + "[label=\"" + dataStructure[index,7] + str(dataStructure[index,2]) + ' ' + str(dataStructure[index,4]) + "\"] \n"
oString += '}'
print (oString)

#Save to output file
foutput = open( "Output.txt", "w" )
foutput.write(oString);
foutput.close()

    
