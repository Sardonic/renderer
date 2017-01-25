#include <stdio.h>
#include <assert.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <X11/keysymdef.h>
#include <X11/Xutil.h>
#include <math.h>

static int x_at_y(int x1, int y1, int x2, int y2, int y);
void graphics_prog();

int main(int argc, char* argv[])
{
  graphics_prog();
  return 0;
}

struct vec2 {
  float x;
  float y;
};

struct vec3 {
  float x;
  float y;
  float z;
};

struct vec2 vec2mul(struct vec2 v, float scalar)
{
  struct vec2 out;
  out.x = v.x * scalar;
  out.y = v.y * scalar;
  return out;
}

int x_at_y(int x1, int y1, int x2, int y2, int y)
{
  struct vec2 v;
  
  if (y1 == y2)
    return -1;
  if (x1 == x2)
    return x1;

  float m = (float)(y2 - y1) / (x2 - x1);
  float b = y1 - m * x1;
  float minv = 1/m;
  return (int)round(minv * (y - b));
}

void graphics_prog()
{
  Display* dsp;
  int blackColor, whiteColor;
  Window wnd;
  GC gc;
  int x1 = 10,
    x2 = 180;

  dsp = XOpenDisplay(NULL);
  assert(dsp);

  blackColor = BlackPixel(dsp, DefaultScreen(dsp));
  whiteColor = WhitePixel(dsp, DefaultScreen(dsp));

  wnd = XCreateSimpleWindow(dsp, DefaultRootWindow(dsp), 0, 0,
			    640, 480, 0, blackColor, blackColor);

  XSelectInput(dsp, wnd, StructureNotifyMask | ButtonPressMask | KeyPressMask | KeyReleaseMask | PointerMotionMask);

  XMapWindow(dsp, wnd);

  gc = XCreateGC(dsp, wnd, 0, (0));

  XSetForeground(dsp, gc, whiteColor);

  for (;;) {
    XEvent e;
    int exit = 0;
    XNextEvent(dsp, &e);
    
    switch (e.type)
      {
      case KeyPress:
	break;
      case KeyRelease:
	{
	  XKeyEvent b = e.xkey;
	  int keysym = (int)XLookupKeysym(&b, 0); // Why 0? I'm not sure...
	  if (keysym == XK_Left)
	    x1 += 10;
	  if (keysym == XK_Right)
	    x2 += 10;
	  XDrawLine(dsp, wnd, gc, x1, 60, x2, 20);
	}
	break;
      case FocusIn:
	printf("Focus in\n");
	break;
      case FocusOut:
	printf("Focus out\n");
	break;
      case ButtonPress:
	printf("Button Press\n");
	break;
      case ButtonRelease:
	printf("Button Release");
	break;
      case MotionNotify:
	{
	  XMotionEvent m = e.xmotion;
	  //printf("{%d, %d}\n", m.x, m.y);
	}
	break;
      case MapNotify:
	XDrawLine(dsp, wnd, gc, x1, 60, x2, 20);
	XFlush(dsp);
	exit = 1;
	break;
      }

    if (exit)
      break;
  }

  {
    XImage* img;
    img = XGetImage(dsp, wnd, 0, 0, 640, 480, 0xFFFFFFFF, ZPixmap);
    assert(img);
    {
      for (int i = 0; i < img->width; ++i)
	{
	  for (int j = 0; j < 20; ++j)
	    {
	      XPutPixel(img, i, j, img->red_mask & 0xa0a0a0a0);
	      XPutPixel(img, i, 20+j, img->red_mask & 0x0f0f0f0f);
	    }
	}
    }
    XDrawLine(dsp, wnd, gc, x2, 60, x1, 20);
    XFlush(dsp);
    XPutImage(dsp, wnd, gc, img, 0, 0, 0, 0, 640, 480);
  }

  // ----- This code is never reached ------
  sleep(3);

  /* Not sure this stuff does anything. I put it in just in case,
   * but I still get an error message about unreleased windows or something.
   */
  XDestroyWindow(dsp, wnd);

  XCloseDisplay(dsp);
}
