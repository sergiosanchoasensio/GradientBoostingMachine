__author__ = 'Sergi'

import subprocess
from subprocess import Popen, PIPE
import os
import statistics

from tkinter import *

class GbmStart:
    def __init__(self):
        window = Tk()
        window.title("PGUI GBM")
        #window.geometry("720x480")

        frame = Frame(window)
        frame.pack(side=LEFT)

        frame2 = Frame(window)
        frame2.pack(side=RIGHT)

        defaultVariableColumn = 1
        defaultVariableRow = 1
        
        fontAndthings = dict(font=('Arial', 10))
        options = dict(padx=4, pady=4, sticky="NSW")

        #Data set [1,1;1,2]
        Label(frame, text="Data set", **fontAndthings).grid(row=defaultVariableRow, column=defaultVariableColumn, **options)
        self.dataset = StringVar()

        self.entryDataset = Entry(frame, textvariable=self.dataset, justify=RIGHT)
        self.entryDataset.grid(row=defaultVariableRow, column=defaultVariableColumn+1, **options)

        #Experiment of data set
        Label(frame, text="Experiment of data set", **fontAndthings).grid(row=defaultVariableRow+1,column=defaultVariableColumn, **options)
        self.experimentofdataset = StringVar()

        self.entryExperimentofdataset = Entry(frame, textvariable=self.experimentofdataset, justify=RIGHT)
        self.entryExperimentofdataset.grid(row=defaultVariableRow+1, column=defaultVariableColumn+1, **options)

        #minimumnumberofinstancesperleaf
        Label(frame, text="Min. Number of instances per leaf", **fontAndthings).grid(row=defaultVariableRow+2, column=defaultVariableColumn, **options)
        self.minimumnumberofinstancesperleaf = StringVar()

        self.entryMinimumnumberofinstancesperleaf = Entry(frame, textvariable=self.minimumnumberofinstancesperleaf, justify=RIGHT)
        self.entryMinimumnumberofinstancesperleaf.grid(row=defaultVariableRow+2, column=defaultVariableColumn+1, **options)

        #maximumnumberofleaves
        Label(frame, text="Max. Number of leaves", **fontAndthings).grid(row=defaultVariableRow+3, column=defaultVariableColumn, **options)
        self.maximumnumberofleaves = StringVar()

        self.entryMaximumnumberofleaves = Entry(frame, textvariable=self.maximumnumberofleaves, justify=RIGHT)
        self.entryMaximumnumberofleaves.grid(row=defaultVariableRow+3, column=defaultVariableColumn+1, **options)

        #maximumdepth
        Label(frame, text="Max. Depth", **fontAndthings).grid(row=defaultVariableRow+4, column=defaultVariableColumn, **options)
        self.maximumdepth = StringVar()

        self.entryMaximumdepth = Entry(frame, textvariable=self.maximumdepth, justify=RIGHT)
        self.entryMaximumdepth.grid(row=defaultVariableRow+4, column=defaultVariableColumn+1, **options)

        #minimumvariance
        Label(frame, text="Min. Variance", **fontAndthings).grid(row=defaultVariableRow+5, column=defaultVariableColumn, **options)
        self.minimumvariance = StringVar()

        self.entryMinimumvariance = Entry(frame, textvariable=self.minimumvariance, justify=RIGHT)
        self.entryMinimumvariance.grid(row=defaultVariableRow+5, column=defaultVariableColumn+1, **options)

        #numberoftrees
        Label(frame, text="Number of trees", **fontAndthings).grid(row=defaultVariableRow+6, column=defaultVariableColumn, **options)
        self.numberoftrees = StringVar()

        self.entryNumberoftrees = Entry(frame, textvariable=self.numberoftrees, justify=RIGHT)
        self.entryNumberoftrees.grid(row=defaultVariableRow+6, column=defaultVariableColumn+1, **options)

        #learningrate
        Label(frame, text="Learning rate", **fontAndthings).grid(row=defaultVariableRow+7, column=defaultVariableColumn,**options)
        self.learningrate = StringVar()

        self.entryLearningrate = Entry(frame, textvariable=self.learningrate, justify=RIGHT)
        self.entryLearningrate.grid(row=defaultVariableRow+7, column=defaultVariableColumn+1, **options)

        #RadioButton, Options
        self.tOption = StringVar(frame, '1')
        self.set_RadioButton1 = Radiobutton(frame, text="Train", variable=self.tOption, value=1, **fontAndthings)
        self.set_RadioButton2 = Radiobutton(frame, text="Test", variable=self.tOption, value=2, **fontAndthings)
        self.set_RadioButton3 = Radiobutton(frame, text="10CV", variable=self.tOption, value=3, **fontAndthings)
        self.set_RadioButton1.grid(column=defaultVariableColumn, row=defaultVariableRow+8, **options)
        self.set_RadioButton2.grid(column=defaultVariableColumn, row=defaultVariableRow+9, **options)
        self.set_RadioButton3.grid(column=defaultVariableColumn, row=defaultVariableRow+10, **options)

        #CheckButton, Show trees (verbose)
        self.tApplyweight = IntVar(frame,0)
        self.set_CBApplyWeight = Checkbutton(frame, text="Apply Weight", variable=self.tApplyweight, **fontAndthings)
        self.set_CBApplyWeight.grid(column=defaultVariableColumn+1, row=defaultVariableRow+8, **options)

        #CheckButton, Show trees (verbose)
        self.tRBVerbose = IntVar(frame,0)
        self.set_CBVerbose1 = Checkbutton(frame, text="Show model", variable=self.tRBVerbose, **fontAndthings)
        self.set_CBVerbose1.grid(column=defaultVariableColumn+1, row=defaultVariableRow+9, **options)

        #Button Accept
        btCalc = Button(frame, text="Accept", command=self.GbmStart, **fontAndthings)
        btCalc.grid(column=defaultVariableColumn+1, row=defaultVariableRow+10, columnspan=2, rowspan=2, **options)

        #Load parameters
        btCalc = Button(frame, text="Load Param.", command=self.Parameters, **fontAndthings)
        btCalc.grid(column=defaultVariableColumn+2, row=defaultVariableRow+10, columnspan=2, rowspan=2, **options)

        #Output
        scrollbar = Scrollbar(frame2)
        self.text = Text(frame2, width=40, height=20,wrap=WORD, yscrollcommand=scrollbar.set)
        scrollbar.config(command=self.text.yview)

        Label(frame2, text="Output", **fontAndthings).grid(column=defaultVariableColumn+2, row=defaultVariableRow, **options)
        self.text.grid(column=defaultVariableColumn+2, row=defaultVariableRow+1, **options)
        scrollbar.grid(column=defaultVariableColumn+7, row=defaultVariableRow+1, **options)

        #Version
        Label(frame2, text="version 17/03/2015 18:48").grid(column=defaultVariableColumn+2, row=defaultVariableRow+2, **options)
        
        window.mainloop()

    def Parameters(self):
        self.dataset.set('ExamenFebreroDummy')
        self.experimentofdataset.set(0)
        self.minimumnumberofinstancesperleaf.set(4)
        self.maximumnumberofleaves.set(0)
        self.maximumdepth.set(1)
        self.minimumvariance.set(0.000001)
        self.numberoftrees.set(10)
        self.learningrate.set(0.1)

    def GbmStart(self):
        params = ''
        params = ' --minimumnumberofinstancesperleaf=' + str(self.minimumnumberofinstancesperleaf.get())
        params += ' --maximumnumberofleaves=' + str(self.maximumnumberofleaves.get())
        params += ' --maximumdepth=' + str(self.maximumdepth.get())
        params += ' --minimumvariance=' + str(self.minimumvariance.get())
        params += ' --numberoftrees=' + str(self.numberoftrees.get())
        params += ' --learningrate=' + str(self.learningrate.get())

        if str(self.tApplyweight.get()) == '0':
            params += ' --applyweight=F'
        else:
            params += ' --applyweight=T'
        if str(self.tRBVerbose.get()) == '0':
            params += ' --verbose=F'
        else:
            params += ' --verbose=T'
        dataset = self.dataset.get()

        #variables
        exeName = 'GradientBoosting.exe'
        #Static variables
        NUMBEROFCHARSINTESTACCURACY = 13
        NUMBEROFCHARSINFMEASURE = 10
        train = ''
        test = ''
        resultant = ''
        
        #ROC
        gAccuracy = []
        gFMeasure = []
        gAvgAccuracy = 0
        gAvgFMeasure = 0
        gStdAccuracy = []
        gStdFMeasure = []

        self.text.yview(END)

        #CV
        if (self.tOption.get() == str(3)):
            self.text.insert(END,"Straight 10 Fold Cross Validation: \n")
            iNumFile = 0
            for iNumFile in range(10):
                    train = ''
                    test = ''
                    resultant = ''
                    train = ' --train=../DataSet/' + dataset + '/' + dataset
                    train += '-10-'
                    train += str(iNumFile)
                    train += 'tra.dat'
                    train += params
                    resultant = exeName + train

                    #GBM
                    output = subprocess.Popen(resultant, shell=True, stdout=PIPE).communicate()[0]

                    resultant = ''
                    test = ' --test=../DataSet/' + dataset + '/' + dataset
                    test += '-10-'
                    test += str(iNumFile)
                    test += 'tst.dat'
                    test += params
                    resultant = exeName + test

                    output = subprocess.Popen(resultant, shell=True, stdout=PIPE).communicate()[0]
                    output = (output.decode("UTF-8"))

                    #Print tree
                    if str(self.tRBVerbose.get()) == '1':
                        index = output.find("unkown values.\r")
                        limit = output.find(str("Confusion"))
                        gbmModel = output[index+len("unkown values.\r"):limit]
                        self.text.insert(END, "Model:" + gbmModel)
                    
                    output = output.replace(' ', '')

                    #Confusion matrix
                    index = output.find("Confusionmatrix")
                    limit = output.find("Testaccuracy")
                    confusionMatrix = output[index+len('Confusionmatrix:\n'):limit]

                    index = output.find("Testaccuracy:")
                    limit = output.find("(",index+NUMBEROFCHARSINTESTACCURACY)

                    testAccuracy = output[index+NUMBEROFCHARSINTESTACCURACY:limit]

                    testAcc = float(testAccuracy)

                    index = output.find("F-Measure:")
                    limit = output.find("\n",index+NUMBEROFCHARSINFMEASURE)

                    fMeasure = output[index+NUMBEROFCHARSINFMEASURE:limit]
                    testFm = float(fMeasure)

                    gAccuracy.append(testAcc)
                    gFMeasure.append(testFm)

            indexMeasure = 0
            for indexMeasure in range(10):
                gAvgAccuracy += gAccuracy[indexMeasure]
                gAvgFMeasure += gFMeasure[indexMeasure]

            gAvgAccuracy = gAvgAccuracy / float(10)
            gAvgFMeasure = gAvgFMeasure / float(10)


            gStdAccuracy = statistics.stdev(gAccuracy)
            gStdFMeasure = statistics.stdev(gFMeasure)

            self.text.insert(END, "Avg. Accuracy (%): "+str(float(round(gAvgAccuracy*100,3))) + '\n')
            self.text.insert(END, "Avg. FMeasure (%): "+str(float(round(gAvgFMeasure*100,3))) + '\n')
            self.text.insert(END, "Standard devation of Accuracy (%): "+str(float(round(gStdAccuracy*100,3))) + '\n')
            self.text.insert(END, "Standard devation of FMeasure (%): "+str(float(round(gStdFMeasure*100,3))) + '\n')
        #Test
        else:
            if (self.tOption.get() == str(2)):
                test = ''
                resultant = ''
                test = ' --test=../DataSet/' + dataset + '/' + dataset
                test += '-10-'
                test += str(self.experimentofdataset.get())
                test += 'tst.dat'

                test += params
                resultant = exeName + test

                print(resultant)

                #GBM
                output = subprocess.Popen(resultant, shell=True, stdout=PIPE).communicate()[0]
                output = (output.decode("UTF-8"))

                #Print tree
                if str(self.tRBVerbose.get()) == '1':
                    index = output.find("unkown values.\r")
                    limit = output.find(str("Confusion"))
                    gbmModel = output[index+len("unkown values.\r"):limit]
                    self.text.insert(END, "Model:" + gbmModel)

                output = output.replace(' ', '')

                #Confusion matrix
                index = output.find("Confusionmatrix")
                limit = output.find("Testaccuracy")
                confusionMatrix = output[index+len('Confusionmatrix:\n'):limit]

                #Accuracy
                index = output.find("Testaccuracy:")
                limit = output.find("(",index+NUMBEROFCHARSINTESTACCURACY)


                testAccuracy = output[index+NUMBEROFCHARSINTESTACCURACY:limit]
                testAcc = float(testAccuracy)

                #F Measure
                index = output.find("F-Measure:")
                limit = output.find("\n",index+NUMBEROFCHARSINFMEASURE)

                fMeasure = output[index+NUMBEROFCHARSINFMEASURE:limit]
                testFm = float(fMeasure)

                gAccuracy.append(testAcc)
                gFMeasure.append(testFm)

                self.text.insert(END, "Confusion Matrix:" + confusionMatrix + "\n")
                self.text.insert(END, "Accuracy (%): "+str(float(round(gAccuracy[0]*100,3))) + '\n')
                self.text.insert(END, "FMeasure (%): "+str(float(round(gFMeasure[0]*100,3))) + '\n')
            #Train
            else:
                train = ''
                resultant = ''
                train = ' --train=../DataSet/' + dataset + '/' + dataset
                train += '-10-'
                train += str(self.experimentofdataset.get())
                train += 'tra.dat'
                train += params
                resultant = exeName + train

                #GBM
                output = subprocess.Popen(resultant, shell=True, stdout=PIPE).communicate()[0]
                output = (output.decode("UTF-8"))

                #Print tree
                if str(self.tRBVerbose.get()) == '1':
                    index = output.find("unkown values.\r")
                    limit = output.find(str("\r\n\r\n[Press enter to exit]"))
                    gbmModel = output[index+len("unkown values.\r"):limit]
                    self.text.insert(END, "Model:" + gbmModel + "\n \n")

                self.text.insert(END, "Train done! \n")
        self.text.insert(END,'\n')

GbmStart()
