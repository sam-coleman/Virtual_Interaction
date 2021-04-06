# Computer Vision Interation Project

Sam Coleman and Kate Mackowiak

## Project Goals:

Our project goal is to use C++ and OpenCV to get input from a user, and use that input to perform a task. Our minimum viable goal is to track a color target to allow the user to paint in the air. There are multiple extenstions we want to explore, such as various painting options (such as allowing the user to select different colors and line thicknesses by pointing at different options for some amount of time along with other features) in addition to tracking a hand or pointer finger as opposed to a color target. These extensions can be pursued simultaneously and merged once working separately. Our stretch goal is to use the input from a pointer finger (as well as potentionally the whole hand) to create a virtual mouse controlled by hand gestures. The mouse should, at minimum, have the ability to school on a page, for maximum ergonimics. 

## Learning Goals:

Our primary goal is to gain familiarity with OpenCV and C++. Our second goal is to learn how to research a subject that we have minimal familiarity with and take a project on it from brainstorming to working prototype. Finally, we want to work on a portfolio worthy project. 

## What We've Done So Far:
While working, we have two components of our project being developed simultaneously: virtual painting and hand tracking.

### Virtual Painting:
With regards to the virtual painting component, we have a past MVP prototype completed. The general structure and functions were based on this article by [Analytics India Mag](https://analyticsindiamag.com/how-to-create-a-virtual-painting-app-using-opencv/) of a similar application written in Python. Our current iteration works by trackinga a red piece of paper the user is holding to paint with (as opposed to a finger). The current features supported are:
* Changing color from preset of 4 options
* Changing pen width from preset of 5 options
* Displaying the current color and current thickness on screen
* Clearing all output
* Duplicating the painting overlayed on user's video on a blank screen to make it easier to tell where the "paintbrush" is

Some of the next features we want to implement include:
* Increasing canvas window size to give more real estate for controls and painting room
* Save canvas output
* Determine and implement a method to allow the user more choice over the colors they can use

#### Current Draft Output Virtual Painting
![Current Draft Output](/saved_outputs/flower_draft.jpg)

### Hand Tracking:
With regards to hand tracking, we have attempted the implementation of four different methods. Our first implementation was using a neural net however the processing time was too slow for our use case. We need as close to real time feedback as possible and were getting about a frame per second, which is extremely limiting when you are trying to paint (with a one second delay). We then attempted implementing a few different algorithms which all worked pretty similarly - first identifying the users skin tone and then looking for a blob with that color and identifying it as a hand. These implementations also aim to identify the number of fingers the user has held up by counting the number of contours in the blob. We are currently attempting to convert a pointer finger tracker by [Amar Prakash Pandey](https://github.com/amarlearning/Finger-Detection-and-Tracking/blob/master/Finger%20Detection%20and%20Tracking/FingerDetection.py) from Python to C++. However, due to the drastic difference between python and C++ this is going slowly. One possible direction we can attempt is instead using a haar cascade to detect a hand in one unique position. While this would require the user to be at a specific angle to the camera and hold their hand in a specific way, it would simplify many of our problems separating the hand from the background.

#### Test Implementation of Hand Tracking
![Test Implementation of Hand Tracking](/saved_outputs/handfailure.png)

### Current Tasks
1. Convert pointer finger tracking from Python to C++. Definition of done: all functions are converted and functionality deemed feasible or not. Sam and Kate
2. Research haar cascade. Definition of done: Deemed feasible or not; if yes model found to base our implementation of off. Kate
3. Add features from above to virtual painting. Definition of done: features are added. Sam
4. Consider if we want to start developing mouse input by using color targets on fingers. Something we will be thinking about (whether we want to continue down hand tracking route or would rather try to get mouse input working). Sam and Kate
