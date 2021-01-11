#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <X11/Xlib.h>

#include "vroot.h"

static const long int MICROSECONDS_PER_SECOND = 1000000;

static char *describeDisplay(char *display);
static long int delta();
static Window getWindow(Display *display, int screenNum, Window root, int argc, char *argv[]);
static struct Rectangle getCharCell(Display *display, GC graphicsContext);

struct Rectangle
{
	int width;
	int height;
};

int main(int argc, char *argv[]) {
	Display *display;
	Window root, win;
	XWindowAttributes winAttributes;
	GC graphicsContext;
	Font sonyFont;
	XColor redx, reds;
	long startX, startY, x, y;
	XEvent event;
	char *displayEnv;
	int screenNumber;
	Screen *screen;
	struct Rectangle charCell;

	displayEnv = getenv("DISPLAY");
	display = XOpenDisplay(displayEnv);
	if (display == NULL) {
		fprintf(stderr, "Failed to open display: %s\n", describeDisplay(displayEnv));
		exit(EXIT_FAILURE);
	}

	root = DefaultRootWindow(display);
	screenNumber = DefaultScreen(display);
	screen = DefaultScreenOfDisplay(display);
	win = getWindow(display, screenNumber, root, argc, argv);

	XGetWindowAttributes(display, win, &winAttributes);
	graphicsContext = XCreateGC(display, win, 0, NULL);
	sonyFont = XLoadFont(display, "-sony-*-*-*-*-*-*-*-*-*-*-*-*-*");
	XSetFont(display, graphicsContext, sonyFont);
	XAllocNamedColor(display, DefaultColormapOfScreen(screen), "red", &reds, &redx);
	XSetForeground(display, graphicsContext, reds.pixel);

	charCell = getCharCell(display, graphicsContext);

	x = random() % winAttributes.width;
	y = random() % winAttributes.height;

	XSelectInput(display, win, KeyPressMask);

	/* Loop until a key is pressed. */
	while (! XCheckTypedEvent(display, KeyPress, &event)) {
		printf("x=%ld y=%ld\n", x, y);
		char digit = random() % 2 + '0';
		XDrawString(display, win, graphicsContext, x, y, &digit, 1);
		x += delta();
		y += delta();

		if (x < 0 || y < 0 || x >= winAttributes.width || y >= winAttributes.height) {
			puts("clear");
			XClearWindow(display, win);
			x = random() % winAttributes.width;
			y = random() % winAttributes.height;
		}
		XFlush(display);
		usleep(MICROSECONDS_PER_SECOND);
	}
	XCloseDisplay(display);
	return EXIT_SUCCESS;
}

char *describeDisplay(char *display)
{
	return display==NULL?"Default":display;
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

Window getWindow(Display *display, int screenNumber, Window root, int argc, char *argv[])
{
	Window win;
	XEvent event;
	long blackPixel, whitePixel;

	blackPixel = BlackPixel(display, screenNumber);
	whitePixel = WhitePixel(display, screenNumber);

	if (argc > 1 && strcmp(argv[1], "-root")) {
		win = root;
	} else {
		win = XCreateSimpleWindow(display, root, 200, 200, 400, 400, 1, whitePixel, blackPixel);
		XStoreName(display, win, "ZeroOne");
		XSelectInput(display, win, StructureNotifyMask);
		XMapRaised(display, win);
		do {
			XWindowEvent(display, win, StructureNotifyMask, &event);
		} while (event.type != MapNotify);
	}
}

struct Rectangle getCharCell(Display *display, GC graphicsContext)
{
	XGCValues gcValues;
	XFontStruct *fontStruct;
	struct Rectangle cell;

	XGetGCValues(display, graphicsContext, GCFont, &gcValues);
	fontStruct = XQueryFont(display, gcValues.font);
	cell.width = XTextWidth(fontStruct, "A", 1);
	cell.height = fontStruct->ascent + fontStruct->descent;
	return cell;
}

