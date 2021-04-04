# SoftSys_Virtual_Interaction
Software Systems Open CV HCI Project

Identifies a red color target in webcam frame and uses that input to either paint or move a mouse.  

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

Press the 's' key to save the image, and enter a file name in the command line.
Press the 'esc' key to exit the painting canvas. 

### Mouse Movement

Run the Mouse file in the build directory and use a red color target to move the mouse around.

```bash
cd build
./Mouse
```

Press the 'esc' key to exit the program. 

## Contributors

Written by Sam Coleman and Kate Mackowiak for Software Systems at Olin College of Engineering 
