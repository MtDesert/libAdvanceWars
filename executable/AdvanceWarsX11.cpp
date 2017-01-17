#include <X11/Xlib.h>
#include <stdio.h>

void printDisplay(const Display &display){
	printf("Display:\n");
	printf("ConnectionNumber: %d\n",ConnectionNumber(&display));
	printf("DefaultScreen: %d\n",DefaultScreen(&display));
	printf("DefaultRootWindow: %d\n",DefaultRootWindow(&display));
	printf("QLength: %d\n",QLength(&display));
	printf("ScreenCount: %d\n",ScreenCount(&display));
	printf("ServerVender: %s\n",ServerVendor(&display));
	printf("ProtocolVersion %d\n",ProtocolVersion(&display));
	printf("ProtocolRevision %d\n",ProtocolRevision(&display));
	printf("VenderRelease: %d\n",VendorRelease(&display));
	printf("DisplayString: %s\n",DisplayString(&display));
	printf("BitmapUnit: %d\n",BitmapUnit(&display));
	printf("BitmapBitOrder: %d\n",BitmapBitOrder(&display));
	printf("BitmapPad: %d\n",BitmapPad(&display));
	printf("ImageByteOrder: %d\n",ImageByteOrder(&display));
}
void printScreen(const Display &display,int screen){
	printf("Screen:\n");
	printf("RootWindow: %d\n",RootWindow(&display,screen));
	printf("BlackPixel: %.8X\n",BlackPixel(&display,screen));
	printf("WhitePixel: %.8X\n",WhitePixel(&display,screen));
	printf("DisplayWidth: %d\n",DisplayWidth(&display,screen));
	printf("DisplayHeight: %d\n",DisplayHeight(&display,screen));
	printf("DisplayWidthMM: %d\n",DisplayWidthMM(&display,screen));
	printf("DisplayHeightMM: %d\n",DisplayHeightMM(&display,screen));
	printf("DisplayPlanes: %d\n",DisplayPlanes(&display,screen));
	printf("DisplayCells: %d\n",DisplayCells(&display,screen));
	printf("DefaultDepth: %d\n",DefaultDepth(&display,screen));
	printf("DefaultColormap: %d\n",DefaultColormap(&display,screen));
}
void printVisual(const Visual &visual){
	printf("Visual:\n");
	auto extData=visual.ext_data;
	if(extData){
		printf("ext_data:");
		for(;extData;extData=extData->next){printf(" %d",extData->number);}
		printf("\n");
	}
	printf("ID: %u\n",visual.visualid);
	printf("Mask(r,g,b): %.8X,%.8X,%.8X\n",visual.red_mask,visual.green_mask,visual.blue_mask);
	printf("Bit per RGB: %d\n",visual.bits_per_rgb);
	printf("Map entries: %d\n",visual.map_entries);
}

void keyPressEvent(const XKeyPressedEvent &event){}
void keyReleaseEvent(const XKeyReleasedEvent &event){}
void buttonPressEvent(const XButtonPressedEvent &event){}
void buttonReleaseEvent(const XButtonReleasedEvent &event){}
void motionNotifyEvent(const XMotionEvent &event){}
void enterNotifyEvent(const XEnterWindowEvent &event){}
void leaveNotifyEvent(const XLeaveWindowEvent &event){}
void focusInEvent(const XFocusInEvent &event){}
void focusOutEvent(const XFocusOutEvent &event){}
void keymapNotifyEvent(const XKeymapEvent &event){}
void exposeEvent(const XExposeEvent &event){}

int main(int argc, char **argv)
{
	char *display_name = NULL;
	printf("Opening display: \"%s\"....",XDisplayName(display_name));
	Display *display=XOpenDisplay(display_name);
	printf(display?"success!\n":"failed!\n");
	if(!display)return 0;

	//print...
	printDisplay(*display);
	int screen = DefaultScreen(display);
	printScreen(*display,screen);
	Visual *visual=DefaultVisual(display,screen);
	if(visual){
		printVisual(*visual);
	}

	unsigned width=200,height=200;
	unsigned int border_width = 4;
	Window win = XCreateSimpleWindow(display, //display
		RootWindow(display,screen),
		0, 0, width, height,border_width,
		WhitePixel(display,screen),
		BlackPixel(display,screen));

	XSelectInput(display, win,
		KeyPressMask|KeyReleaseMask|
		ButtonPressMask|ButtonReleaseMask|
		EnterWindowMask|LeaveWindowMask|
		PointerMotionMask|PointerMotionHintMask|
		Button1MotionMask|Button2MotionMask|Button3MotionMask|Button4MotionMask|Button5MotionMask|
		ButtonMotionMask|
		KeymapStateMask|
		ExposureMask|
		VisibilityChangeMask|
		StructureNotifyMask|
		ResizeRedirectMask|
		SubstructureNotifyMask|
		SubstructureRedirectMask|
		FocusChangeMask|
		PropertyChangeMask|
		ColormapChangeMask|
		OwnerGrabButtonMask);

	XGCValues values;
	GC gc = XCreateGC(display,win,0,&values);
	XMapWindow(display, win);

	//event loop
	XEvent event;
	while(true){
		XNextEvent(display,&event);
		switch (event.type) {
			case KeyPress:keyPressEvent(event.xkey);break;
			case KeyRelease:keyReleaseEvent(event.xkey);break;
			case ButtonPress:buttonPressEvent(event.xbutton);break;
			case ButtonRelease:buttonReleaseEvent(event.xbutton);break;
			case MotionNotify:motionNotifyEvent(event.xmotion);break;
			case EnterNotify:enterNotifyEvent(event.xcrossing);break;
			case LeaveNotify:leaveNotifyEvent(event.xcrossing);break;
			case FocusIn:focusInEvent(event.xfocus);break;
			case FocusOut:focusOutEvent(event.xfocus);break;
			case KeymapNotify:keymapNotifyEvent(event.xkeymap);break;
			case Expose:exposeEvent(event.xexpose);break;
			case ConfigureNotify:
				width = event.xconfigure.width;
				height = event.xconfigure.height;
			break;
			default:break;
		}
	}
	return 0;
}