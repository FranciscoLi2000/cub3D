#ifndef X11_COMPAT_H
# define X11_COMPAT_H

# include <stdint.h>
# include <stddef.h>

typedef struct s_xdisplay	Display;
typedef unsigned long		XID;
typedef XID					Window;
typedef XID					Drawable;
typedef XID					Pixmap;
typedef XID					Colormap;
typedef XID					Atom;
typedef XID					KeySym;
typedef unsigned long		Time;
typedef int					Bool;
typedef int					Status;
typedef char				*XPointer;
typedef struct s_xvisual	Visual;
typedef struct s_xgc		*GC;

# define False 0
# define True 1
# define None 0
# define Success 0
# define ZPixmap 2

# define KeyPress 2
# define KeyRelease 3
# define ButtonPress 4
# define ButtonRelease 5
# define MotionNotify 6
# define Expose 12
# define DestroyNotify 17
# define ConfigureNotify 22
# define ClientMessage 33

# define KeyPressMask (1L << 0)
# define KeyReleaseMask (1L << 1)
# define ButtonPressMask (1L << 2)
# define ButtonReleaseMask (1L << 3)
# define PointerMotionMask (1L << 6)
# define ExposureMask (1L << 15)
# define StructureNotifyMask (1L << 17)

typedef struct s_xgcvalues
{
	int	dummy;
}	t_xgcvalues;

typedef struct s_xcompose_status
{
	int	dummy;
}	t_xcompose_status;

typedef struct s_ximage
{
	int				width;
	int				height;
	int				xoffset;
	int				format;
	char			*data;
	int				byte_order;
	int				bitmap_unit;
	int				bitmap_bit_order;
	int				bitmap_pad;
	int				depth;
	int				bytes_per_line;
	int				bits_per_pixel;
	unsigned long	red_mask;
	unsigned long	green_mask;
	unsigned long	blue_mask;
	XPointer		obdata;
	struct s_ximage	*(*create_image)(struct s_ximage *, unsigned int, int,
			int, char *, int, int, int, int, int);
	int				(*destroy_image)(struct s_ximage *);
	unsigned long	(*get_pixel)(struct s_ximage *, int, int);
	int				(*put_pixel)(struct s_ximage *, int, int, unsigned long);
	struct s_ximage	*(*sub_image)(struct s_ximage *, int, int,
			unsigned int, unsigned int);
	int				(*add_pixel)(struct s_ximage *, long);
}	t_ximage;

typedef struct s_xkeyevent
{
	int				type;
	unsigned long	serial;
	Bool			send_event;
	Display			*display;
	Window			window;
	Window			root;
	Window			subwindow;
	Time			time;
	int				x;
	int				y;
	int				x_root;
	int				y_root;
	unsigned int	state;
	unsigned int	keycode;
	Bool			same_screen;
}	t_xkeyevent;

typedef struct s_xbuttonevent
{
	int				type;
	unsigned long	serial;
	Bool			send_event;
	Display			*display;
	Window			window;
	Window			root;
	Window			subwindow;
	Time			time;
	int				x;
	int				y;
	int				x_root;
	int				y_root;
	unsigned int	state;
	unsigned int	button;
	Bool			same_screen;
}	t_xbuttonevent;

typedef struct s_xmotionevent
{
	int				type;
	unsigned long	serial;
	Bool			send_event;
	Display			*display;
	Window			window;
	Window			root;
	Window			subwindow;
	Time			time;
	int				x;
	int				y;
	int				x_root;
	int				y_root;
	unsigned int	state;
	char			is_hint;
	Bool			same_screen;
}	t_xmotionevent;

typedef struct s_xconfigevent
{
	int				type;
	unsigned long	serial;
	Bool			send_event;
	Display			*display;
	Window			event;
	Window			window;
	int				x;
	int				y;
	int				width;
	int				height;
	int				border_width;
	Window			above;
	Bool			override_redirect;
}	t_xconfigevent;

typedef struct s_xclientmessageevent
{
	int				type;
	unsigned long	serial;
	Bool			send_event;
	Display			*display;
	Window			window;
	Atom			message_type;
	int				format;
	union
	{
		char	b[20];
		short	s[10];
		long	l[5];
	}				data;
}	t_xclientmessageevent;

typedef struct s_xexposeevent
{
	int				type;
	unsigned long	serial;
	Bool			send_event;
	Display			*display;
	Window			window;
	int				x;
	int				y;
	int				width;
	int				height;
	int				count;
}	t_xexposeevent;

typedef union u_xevent
{
	int						type;
	t_xkeyevent				xkey;
	t_xbuttonevent			xbutton;
	t_xmotionevent			xmotion;
	t_xconfigevent			xconfigure;
	t_xclientmessageevent	xclient;
	t_xexposeevent			xexpose;
}	t_xevent;

Display		*XOpenDisplay(char *display_name);
int			XCloseDisplay(Display *display);
int			XDefaultScreen(Display *display);
Window		XRootWindow(Display *display, int screen_number);
Colormap	XDefaultColormap(Display *display, int screen_number);
int			XDefaultDepth(Display *display, int screen_number);
Visual		*XDefaultVisual(Display *display, int screen_number);
Window		XCreateSimpleWindow(Display *display, Window parent, int x, int y,
			unsigned int width, unsigned int height, unsigned int border_width,
			unsigned long border, unsigned long background);
int			XStoreName(Display *display, Window w, char *name);
int			XMapWindow(Display *display, Window w);
int			XDestroyWindow(Display *display, Window w);
int			XSelectInput(Display *display, Window w, long event_mask);
GC			XCreateGC(Display *display, Drawable d, unsigned long valuemask,
			t_xgcvalues *values);
int			XFreeGC(Display *display, GC gc);
int			XSetForeground(Display *display, GC gc, unsigned long foreground);
int			XDrawPoint(Display *display, Drawable d, GC gc, int x, int y);
int			XClearWindow(Display *display, Window w);
int			XFlush(Display *display);
int			XSync(Display *display, Bool discard);
int			XPending(Display *display);
int			XNextEvent(Display *display, t_xevent *event_return);
KeySym		XLookupKeysym(t_xkeyevent *key_event, int index);
t_ximage	*XCreateImage(Display *display, Visual *visual, unsigned int depth,
			int format, int offset, char *data, unsigned int width,
			unsigned int height, int bitmap_pad, int bytes_per_line);
int			XPutImage(Display *display, Drawable d, GC gc, t_ximage *image,
			int src_x, int src_y, int dest_x, int dest_y, unsigned int width,
			unsigned int height);
int			XDestroyImage(t_ximage *image);
Atom		XInternAtom(Display *display, char *atom_name, Bool only_if_exists);
int			XSetWMProtocols(Display *display, Window w, Atom *protocols,
			int count);
int			XWarpPointer(Display *display, Window src_w, Window dest_w,
			int src_x, int src_y, unsigned int src_width,
			unsigned int src_height, int dest_x, int dest_y);
int			XQueryPointer(Display *display, Window w, Window *root_return,
			Window *child_return, int *root_x_return, int *root_y_return,
			int *win_x_return, int *win_y_return, unsigned int *mask_return);
int			XMoveWindow(Display *display, Window w, int x, int y);
int			XResizeWindow(Display *display, Window w, unsigned int width,
			unsigned int height);
int			XAutoRepeatOff(Display *display);
int			XAutoRepeatOn(Display *display);
int			XDefineCursor(Display *display, Window w, void *cursor);
void		*XCreateFontCursor(Display *display, unsigned int shape);
int			XFreeCursor(Display *display, void *cursor);
int			XGetGeometry(Display *display, Drawable d, Window *root_return,
			int *x_return, int *y_return, unsigned int *width_return,
			unsigned int *height_return, unsigned int *border_width_return,
			unsigned int *depth_return);

# define XK_Escape 0xff1b
# define XK_Left 0xff51
# define XK_Up 0xff52
# define XK_Right 0xff53
# define XK_Down 0xff54
# define XK_w 0x0077
# define XK_a 0x0061
# define XK_s 0x0073
# define XK_d 0x0064

#endif
