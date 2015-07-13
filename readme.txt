07.02.15,

Website: http://sergiosanchoasensio.atspace.cc/
Mail: sergiosanchoasensio@gmail.com

Hello!,

First of all I would like to thank you for taking your time to nose about my thesis 'Beyond Traditional Frameworks For Modelling Learning Domains'!

As you can see in the root there are two folders:

1) 'Thesis' where you can read my own thesis and presentation slides (in spanish language). In the slides you can see step by step how Decision Trees and Gradient Boosting technique works.

2) 'Core' where you can find the implementation and documentation of Gradient Boosting in C++ (GBM), the data sets used in the thesis, the GUI in Python used to make the experimentation process faster (PGUI) and the script in Python that translates the output of GBM to DOT language in order to print the trees (ModelToGpv).

Folders in 'Core':
	
	2.1) DataSet
	2.2) Documentation
	2.3) Executable*
	2.4) ModelToGpv**
	2.4) SourceCode

(*)
In 'Executable' folder you can find GradientBoosting.exe and PGUI.py.

GradientBoosting.exe works on command line:

Training example:

GradientBoosting.exe "--train=../DataSet/irs/irs-10-0tra.dat --minimumnumberofinstancesperleaf=4 --maximumnumberofleaves=0 --maximumdepth=0 --minimumvariance=0.003 --numberoftrees=100 --learningrate=0.1 --verbose=F --multipleexperiments=F"

Test example:

GradientBoosting.exe "--test=../DataSet/irs/irs-10-0tst.dat --minimumnumberofinstancesperleaf=4 --maximumnumberofleaves=0 --maximumdepth=0 --minimumvariance=0.003 --numberoftrees=100 --learningrate=0.1 --verbose=F --multipleexperiments=F"

PGUI.py it is a GUI that makes the experimentation more user friendly. It allows you to configure, call GBM and use 10 fold cross validation.

(**)

ModelToGpv tranlates the GBM output (input.txt) to DOT language (output.txt). You can use Graphviz to read output.txt content and print the model.

For more information see ‘TFG_GBM.pdf’.

If you have any further questions do not hesitate to contact me,

Sergio Sancho Asensio.