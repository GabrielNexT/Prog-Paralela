#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

#define X_RESN 80000 /* x resolution */
#define Y_RESN 80000 /* y resolution */

typedef struct complextype
{
	float real, imag;
} Compl;

typedef unsigned int ui;

void main()
{
	double start_time = omp_get_wtime();

	/* initialization for a window */
	Window win;
	ui width, height;									/* window size */
	ui x, y;													/* window position */
	ui border_width;									/*border width in pixels */
	ui display_width, display_height; /* size of screen */
	ui screen;												/* which screen */
	char *window_name = "Mandelbrot Set", *display_name = NULL;
	GC gc;
	unsigned long valuemask = 0;
	XGCValues values;
	Display *display;
	XSizeHints size_hints;
	Pixmap bitmap;
	XPoint points[800];
	FILE *fp, *fopen();
	char str[100];

	XSetWindowAttributes attr[1];

	/* connect to Xserver */

	if ((display = XOpenDisplay(display_name)) == NULL)
	{
		fprintf(stderr, "drawon: cannot connect to X server %s\n", XDisplayName(display_name));
		exit(-1);
	}

	/* get screen size */
	screen = DefaultScreen(display);
	display_width = DisplayWidth(display, screen);
	display_height = DisplayHeight(display, screen);

	/* set window size */
	width = X_RESN;
	height = Y_RESN;

	/* set window position */
	x = 0;
	y = 0;

	/* create opaque window */
	border_width = 4;
	win = XCreateSimpleWindow(display, RootWindow(display, screen),
														x, y, width, height, border_width,
														BlackPixel(display, screen), WhitePixel(display, screen));

	size_hints.flags = USPosition | USSize;
	size_hints.x = x;
	size_hints.y = y;
	size_hints.width = width;
	size_hints.height = height;
	size_hints.min_width = 300;
	size_hints.min_height = 300;

	XSetNormalHints(display, win, &size_hints);
	XStoreName(display, win, window_name);

	/* create graphics context */

	gc = XCreateGC(display, win, valuemask, &values);

	XSetBackground(display, gc, WhitePixel(display, screen));
	XSetForeground(display, gc, BlackPixel(display, screen));
	XSetLineAttributes(display, gc, 1, LineSolid, CapRound, JoinRound);

	attr[0].backing_store = Always;
	attr[0].backing_planes = 1;
	attr[0].backing_pixel = BlackPixel(display, screen);

	XChangeWindowAttributes(display, win, CWBackingStore | CWBackingPlanes | CWBackingPixel, attr);

	XMapWindow(display, win);
	XSync(display, 0);

	/* Calculate and draw points */

#pragma omp parallel for shared(display, win, gc)
	for (int i = 0; i < X_RESN; i++)
	{
#pragma omp parallel for shared(display, win, gc)
		for (int j = 0; j < Y_RESN; j++)
		{
			Compl z, c;
			float lengthsq, temp;

			z.real = z.imag = 0.0;
			c.real = ((float)j - 400.0) / 200.0; /* scale factors for 800 x 800 window */
			c.imag = ((float)i - 400.0) / 200.0;
			int k = 0;

			do
			{ /* iterate for pixel color */

				temp = z.real * z.real - z.imag * z.imag + c.real;
				z.imag = 2.0 * z.real * z.imag + c.imag;
				z.real = temp;
				lengthsq = z.real * z.real + z.imag * z.imag;
				k++;
			} while (lengthsq < 4.0 && k < 100);

			if (k == 100)
			{
#pragma omp critical
				{
					XDrawPoint(display, win, gc, j, i);
				}
			}
		}
	}

	double end_time = omp_get_wtime();

	printf("Total runtime: %.15lf\n", end_time - start_time);

	/* Program Finished */
}