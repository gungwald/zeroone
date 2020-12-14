#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <X11/Xlib.h>

#include "vroot.h"

long int delta();

int main(int argc, char *argv[]) {
	Display *dpy;
	Window root, win;
	XWindowAttributes winAttributes;
	GC graphicsContext;
	Font sonyFont;
	XColor redx, reds;
	long startX, startY, x, y;
	XEvent e;
	long blackPixel, whitePixel;

	dpy = XOpenDisplay(getenv("DISPLAY"));
	root = DefaultRootWindow(dpy);
	blackPixel = BlackPixel(dpy, DefaultScreen (dpy));
	whitePixel = WhitePixel(dpy, DefaultScreen (dpy));

	if (argc > 1 && strcmp(argv[1], "-root")) {
		win = root;
	} else {
		/* create, name, and map window */
		win = XCreateSimpleWindow(dpy, root, 200, 200, 400, 400, 1, 
				whitePixel, blackPixel);
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
		printf("x=%ld y=%ld\n", x, y);
		XDrawString(dpy, win, graphicsContext, x, y, " ", 1);
		char digit = random() % 2 + '0';
		XDrawString(dpy, win, graphicsContext, x, y, &digit, 1);
		x += delta();
		y += delta();

		if (x < 0 || y < 0 || x >= winAttributes.width || y >= winAttributes.height) {
			puts("clear");
			XClearWindow(dpy, win);
			x = random() % winAttributes.width;
			y = random() % winAttributes.height;
		}

		/* flush changes and sleep */
		XFlush(dpy);
		usleep(1000000);
	}

	XCloseDisplay(dpy);
	return EXIT_SUCCESS;
}

long int delta()
{
	long change = random() % 3;
	switch(change) {
	case 0:
		change = -15;
		break;
	case 1:
		change = 0;
		break;
	case 2:
		change = 15;
		break;
	default:
		change = 0;
		break;
	}
	return change;
}
