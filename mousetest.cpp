#include <X11/Xlib.h>
#include <iostream>
#include <unistd.h>

// https://gist.github.com/aliva/3372331
// sudo apt-get install libx11-dev
// g++ -o mousetest  mousetest.cpp -lX11

int main(void) {
    // Find display
    Display* dpy = XOpenDisplay(0);
    int scr = XDefaultScreen(dpy);
    Window root_window = XRootWindow(dpy, scr);

    // Determine display dimensions
    int height = DisplayHeight(dpy, scr);
    int width  = DisplayWidth(dpy, scr);
    std::cout << "Screen size : " << width << "x" << height << std::endl;

    // Ratio of height to width
    float m = (float)height/(float)width;

    int j;
    // Create coordinates from (0,0) to (width,height)
    for(int i=0; i<width; i++){
        j = m*i;
        // Move pointer to coords (i,j)
        XWarpPointer(dpy, None, root_window, 0, 0, 0, 0, i, j);
        XFlush(dpy);
        //pause some milliseconds
        usleep(500);
    }
}