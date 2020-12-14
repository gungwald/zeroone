#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <X11/Xlib.h>

#include "vroot.h"

int main(int argc, char *argv[]) {
	Display *dpy;
	Window win;
	XWindowAttributes winAttributes;
	GC graphicsContext;
	Font sonyFont;
	XColor redx, reds;
	long startX, startY, x, y;
	XEvent e;

	/* open the display (connect to the X server) */
	dpy = XOpenDisplay(getenv("DISPLAY"));

	if (argc > 1 && strcmp(argv[1], "-root")) {
		win = DefaultRootWindow(dpy);
	} else {
		/* create, name, and map window */
		win = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 200, 200, 400, 400,
				1, BlackPixel(dpy, DefaultScreen (dpy)),
				WhitePixel(dpy, DefaultScreen (dpy)));
		XStoreName(dpy, win, "ZeroOne");
		XSelectInput(dpy, win, StructureNotifyMask);
		XMapRaised(dpy, win);
		do {
			XWindowEvent(dpy, win, StructureNotifyMask, &e);
		} while (e.type != MapNotify);
	}

	/* get attributes of the root window */
	XGetWindowAttributes(dpy, win, &winAttributes);

	/* create a GC for drawing in the window */
	graphicsContext = XCreateGC(dpy, win, 0, NULL);

	sonyFont = XLoadFont(dpy, "-sony-*-*-*-*-*-*-*-*-*-*-*-*-*");
	XSetFont(dpy, graphicsContext, sonyFont);

	/* allocate the red color */
	XAllocNamedColor(dpy, DefaultColormapOfScreen(DefaultScreenOfDisplay(dpy)),
			"red", &reds, &redx);

	/* set foreground color */
	XSetForeground(dpy, graphicsContext, reds.pixel);

	x = random() % winAttributes.width;
	y = random() % winAttributes.height;

	while (1) {
		printf("x=%d y=%d\n", x, y);
		XDrawString(dpy, win, graphicsContext, x, y, "0", 1);
		x += 10;
		y += 10;

		if (x >= winAttributes.width || y >= winAttributes.height) {
			puts("clear");
			XClearWindow(dpy, win);
			x = random() % winAttributes.width;
			y = random() % winAttributes.height;
		}

		/* flush changes and sleep */
		XFlush(dpy);
		usleep(1000);
	}

	XCloseDisplay(dpy);
	return EXIT_SUCCESS;
}
