# Change-Counter-Android

An Android application that uses the Camera API to take a picture which gets sent to a C++ local web server which uses Image processing and bag of words to indentify and recognise coins.

#Dependencies
Requires OpenCV to run the c++ server, android application will run on its own.
A uk coin database is required for the OpenCV

#Known issues
UI for android needs touching up, as the i focused less on the UI and more on the code.
Server can give out false negatives.
Accuracy can vary
Android application camera api can sometimes, camra2 is a fix;
TCP sending from android is not fully been sent at times, this a server issue not a android application issue;

