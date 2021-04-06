# Computer Vision Interation Project

Sam Coleman and Kate Mackowiak

## Project Goals:

Our project goal is to use C++ and OpenCV to get input from a user, and use that input to perform a task. Our minimum viable goal is to track a color target to allow the user to paint in the air. There are multiple extenstions we want to explore, such as various painting options (such as allowing the user to select different colors and line thicknesses by pointing at different options for some amount of time along with other features) in addition to tracking a hand or pointer finger as opposed to a color target. These extensions can be pursued simultaneously and merged once working separately. Our stretch goal is to use the input from a pointer finger (as well as potentionally the whole hand) to create a virtual mouse controlled by hand gestures. The mouse should, at minimum, have the ability to scroll on a page, for maximum ergonimics. 

## Learning Goals:

Our primary goal is to gain familiarity with OpenCV and C++. Our second goal is to learn how to research a subject that we have minimal familiarity with and take a project on it from brainstorming to working prototype. Finally, we want to work on a portfolio worthy project. 

## What We Did:
While working, we three components of our project being developed simultaneously: virtual painting, mouse interaction, and hand tracking.

### Virtual Painting:
With regards to the virtual painting component, we have a past MVP prototype completed. The general structure and functions were based on this article by [Analytics India Mag](https://analyticsindiamag.com/how-to-create-a-virtual-painting-app-using-opencv/) of a similar application written in Python. Our program works by tracking a color target the user is holding to paint with (as opposed to a finger). The current features supported are:
* Change the color target by passing in the lower and upper HSV bounds (default is red)
* Changing color from preset of 5 options
* Changing pen width from preset of 4 options
* Displaying the current color and current thickness on screen
* Save output with user inputted name and file extension
* Clearing all output
* Duplicating the painting overlayed on user's video on a blank screen to make it easier to tell where the "paintbrush" is

The Virtual Painting implementation is in [VirtualPainting.cpp](/VirtualPainting.cpp) and the helper functions are declared in [painting.cpp](/include/painting.cpp) and [shared.cpp](include/shared.cpp). The broad walkthrough of the implementation is:
1. Define and draw controls (for clear, color, and thickness
(The following operations are in a while loop)
2. Update the video capture and the indicators for current color and thickness
3. Perform morphing techniques to make color target more clear
4. Find contours and determine if color target is present (area of largest contour must be greater than noise threshold)
5. If color target detected, perform action based on location
    * If in change color control box, update color
    * If in change thickness control box, update thickness
    * If in clear control box, clear canvas
    * If none of the above, draw line connecting from previous coordinates to current coordinates using current color and thickness
6. Update coordinates
7. Update windows and display on screen
8. Wait 10ms to see if key is pressed
     * If 'esc', exit program
     * If 's', have user input file_name.extension and save current canvas to folder

### Implementation Key Snippets

To help with identifying a color target, we performed the below morphing techniques on the video capture
```c
//Perform morphing techniques
Mat kernal = Mat(5, 5, CV_8U, Scalar(1,1,1));
Mat mask;
inRange(hsv, lowerBound, upperBound, mask);
erode(mask, mask, kernal);
erode(mask, mask, kernal);
morphologyEx(mask, mask, MORPH_OPEN, kernal);
dilate(mask, mask, kernal);
```

Below is how we identify the contours present in the capture to determine if the color target is identified. GetMaxContourId is a helper function defined in [shared.cpp](include/shared.cpp) that takes a vector of contours (represneted as points) as an argument and returns the position of the maximum contour.
```c 
//Find all contours and determine if color target is seen (if yes, get location)
//find contours
vector <vector<Point> > contours;
findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

//if color target is seen
if (contours.size() >= 1 && contourArea(contours.at(GetMaxAreaContourId(contours))) > noiseThreshold) { //if sees color target
    vector<Point> c = contours.at(GetMaxAreaContourId(contours)); //location of color target
    boundBox = boundingRect(c); //create bounding box
    //(code continues from step 5 above)
}
```

GetColorFromTuple is a helper function we created to convert the tuple representing a color that is stored in our color vector to a Scalar type that OpenCV can use.
```c
//Convert color represented as BGR tuple to a Scalar type
Scalar GetColorFromTuple (tuple<int, int, int> color) {
    int blue, green, red;
    blue = get<0>(color);
    green = get<1>(color);
    red = get<2>(color);
    return Scalar(blue, green, red);
}
```
#### Example Output Virtual Painting
[Video of Virtual Painting in action!](saved_outputs/PaintingTest.mp4)  
**TODO: INSERT PHOTOS OF OUTPUTS**  
To view more sample outputs, see [saved_outputs](/saved_outputs).  

### Mouse Movement:
We were able to use the countour input from the color tracker to do minimal control of the mouse using [this resource](https://gist.github.com/aliva/3372331). Using the X11 library, we are able to move the user's mouse around the screen, regardless of what window is being used, however that is the limit of our functionality. We decided not to implement clicking or scrolling functionality due to difficulties with implementation as well as difficulty identifying what a "click" or "scroll" using a color target was.

The key code in allowing the mouse motion is below, and moves the user's pointer to the coordinates mouseX and mouseY - which are calculated based on the target coordinates.
```c
// Move pointer to coords (mouseX,mouseY)
XWarpPointer(display, None, rootWindow, 0, 0, 0, 0, mouseX, mouseY);
XFlush(display);
```
[Video of Mouse Movement in action!](/saved_outputs/MouseTest.mp4)

### Hand Tracking:
With regards to hand tracking, we attempted the implementation of several different methods (see our [test files folder](/testFiles)). Our first implementation was using a neural net however the processing time was too slow for our use case. We need as close to real time feedback as possible and were getting about a frame per second, which is extremely limiting when you are trying to paint (with a one second delay). We then attempted implementing a few different algorithms which all worked pretty similarly - first identifying the users skin tone and then looking for a blob with that color and identifying it as a hand. These implementations also aim to identify the number of fingers the user has held up by counting the number of contours in the blob. We also attempting to convert a pointer finger tracker by [Amar Prakash Pandey](https://github.com/amarlearning/Finger-Detection-and-Tracking/blob/master/Finger%20Detection%20and%20Tracking/FingerDetection.py) from Python to C++. However, due to the drastic difference between python and C++ this went pretty slowly. After unsucessfully attempting to implement a Haar Cascade model, we decided that this path was not worth continuing on. Ultimately, hand tracking was not worth pursuing further since we had a working test using a color target. 

#### Test Implementation of Hand Tracking
![Test Implementation of Hand Tracking](/saved_outputs/handfailure.png). As the image shows, this implementation was not accurate.

## Design Decisions:
One design decision we made was to use a color target instead of hand tracking. This was a multi-part decision - deciding that each implementation method was not feasible for our project. The first version that we decided against - using a neural net - was far to slow for our purposes (with a full one second processing delay), and this resulted in a pretty easy decision. However, the next few implementations we tested were not so cut and dry. Many of the methods we tested involved using a similar method to our color tracking of identifying a color range that the hand was in and then finding the largest contour in that color range. We had many issues with this implementation however, because despite being able to set the color range that the hand was in, it often was too similar to most backgrounds to find a hand in frame, even when we made efforts to reduce background noise.  The end decision to use a color target came from the fact that our tracking code for the target was very similar to our hands - without an extra step to find what fingers were pointed. The color target was able to meet our goal of tracking something with OpenCV and a camera in a similar method to our initial plan, while also removing many of the issues we had of similarity to the background by being a bright color. This design decision allowed us to move forward with our other learning goals and continue our project. Despite it not making the final cut, we learned a lot about hand tracking through these various attempts.

## Reflection

We are really proud of this project and what we were able to accomplish and learn. We went well above our lower bound in some aspect, but also had to pivot many of our original goals. We successfully implemented our painting app, and were able to start down the road of using a camera input as a mouse. While we didn't meet our upper bound of having scrolling capability, we were able to move our user's mouse around the screen using a color target. One large pivot we made was from doing hand tracking to color target tracking - which was still able to meet our learning goals despite being a different means to the end. Despite not getting hand tracking to work, we both still learned a lot about the process and implementation (and the difficulties thereof), which meets our learning goals.

One of our other learning goals was to explore the world of C++ and OpenCV, which we defintiely did. We are proud of our final outputs and what we were able to accomplish - and definitely would consider this portfolio worthy.
