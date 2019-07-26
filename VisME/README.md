VisME - Visual Microsaccades Explorer
=====================================

**VisME** (Visual Microsaccades Explorer) is a visual analytics system to explore high frequency eye tracking data with a special focus on **microsaccades**. 
The exploration can take place on different levels, for fixations, participants, trials, and test conditions using data of individuals as well as groups of participants.
The system provides multiple views, eye movement filters, visibility filters and further options to support users in their exploration.

![Screen shot of application](application.png?raw=true)

Implementation
--------------

The system is implemented in C++ with Qt 5 using the Graphics View Framework for interactive visualizations. We reimplemented the R code by [Engbert et al.](http://read.psych.uni-potsdam.de/attachments/article/140/MS_Toolbox_R.zip) to detect microsaccades and added additional conditions for their detection. A project file for Qt Creator is available.

Usage
-----

The .pro file can be opened in Qt Creator and compiled with Qt 5 (tested with Windows 10 with Qt 5.11 and Kubuntu with Qt 5.9).
No additional libraries are required.

Additionally we provide a precompiled version for Windows.
A test data set can also be found in `../eyeTrackingData` to try the system.


Data Preparation
----------------

If you have eye tracking data from an EyeLink Eye Tracker you can convert them in a preprocessing step from .edf files to .asc files using a converter which is available by SR Research: the EyeLink EDF2ASC Converter (included in [Data Viewer](http://www.sr-research.com/accessories_EL1000_dv.html)). Then, you can adapt our script written in Python (`convertFromASC.py`) to create an input file in the format, expected by our application, containing eye tracking positions and fixations as determined by the eye tracking software. The script was written for our data, you will have to change it according to your data.
See inputFormats.txt for more information on the required input files and check the example files.


Last Words
----------

This system was created in a research project; it may contain bugs and behave in unexpected ways.
