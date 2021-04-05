# SoftSys_Virtual_Interaction
Software Systems Open CV HCI Project

Identifies a color target in webcam frame and uses that input to either paint or move a mouse.  

## Requirements: 
    - Opencv >= 3.0 
    - X11

## Installation

Create a build directory, run cmake .., and then run make.

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

### Painting

Run the Painting file in the build directory and use a red color target to virtually "paint".

```bash
cd build
./Painting
```

To change the the color target, add lower and upper bound HSV values when running the file
```bash
./Painting lower_H lower_S lower_V upper_H upper_S upper_V
```

Press the 's' key to save the image, and enter a file name including extension in the command line. The images will be saved in the folder saved_outputs.
Press the 'esc' key to exit the painting canvas. 

Run the Determine Color file to determine the lower and upper bound HSV values of a color target with a GUI. Adjust the sliders until only your color target is displayed, and press the 's' key to output the bounds to the terminal.
```bash
python3 determineColor.py
```

### Mouse Movement

Run the Mouse file in the build directory and use a red color target to move the mouse around.

```bash
cd build
./Mouse
```

Press the 'esc' key to exit the program. 

## Contributors

Written by [Sam Coleman](https://github.com/sam-coleman) and [Kate Mackowiak](https://github.com/kviiim) for Software Systems at Olin College of Engineering 
