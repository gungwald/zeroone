#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <X11/Xlib.h>

#include "vroot.h"

int main(int argc, char *argv[]) {
	Display *dpy;
	Window w;
	XWindowAttributes winAttributes;
	GC graphicsContext;
	Font sonyFont;
	XColor redx, reds;
	long startX, startY;
	XEvent e;

	/* open the display (connect to the X server) */
	dpy = XOpenDisplay(getenv("DISPLAY"));

	if (argc > 1 && strcmp(argv[1], "-root")) {
		w = DefaultRootWindow(dpy);
	} else {
		/* create, name, and map window */
		w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 200, 200, 400, 400,
				1, BlackPixel(dpy, DefaultScreen (dpy)),
				WhitePixel(dpy, DefaultScreen (dpy)));
		XStoreName(dpy, w, "ZeroOne");
		XSelectInput(dpy, w, StructureNotifyMask);
		XMapRaised(dpy, w);
		do {
			XWindowEvent(dpy, w, StructureNotifyMask, &e);
		} while (e.type != MapNotify);
	}

	/* get attributes of the root window */
	XGetWindowAttributes(dpy, w, &winAttributes);

	/* create a GC for drawing in the window */
	graphicsContext = XCreateGC(dpy, w, 0, NULL);

	sonyFont = XLoadFont(dpy, "-sony-*-*-*-*-*-*-*-*-*-*-*-*-*");
	XSetFont(dpy, graphicsContext, sonyFont);

	/* allocate the red color */
	XAllocNamedColor(dpy, DefaultColormapOfScreen(DefaultScreenOfDisplay(dpy)),
			"red", &reds, &redx);

	/* set foreground color */
	XSetForeground(dpy, graphicsContext, reds.pixel);

	/* draw something */
	while (1) {
		/* draw a string */
		XDrawString(dpy, w, graphicsContext, random() % winAttributes.width,
				random() % winAttributes.height, "Ooops!", strlen("Ooops!"));

		/* once in a while, clear all */
		if (random() % 500 < 1)
			XClearWindow(dpy, w);

		/* flush changes and sleep */
		XFlush(dpy);
		usleep(10);
	}

	XCloseDisplay(dpy);
	return EXIT_SUCCESS;
}
