//-----------------------------------------------------------------------------
//@file  
//	UserUtils.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief  
//	Utilities global functions implementation
//
//HISTORIC 
//	2013/05/15
//    first release for Hollyhock CPP SDK 6.00.226 
//
//IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs and patch to Usine forum :
//  support@brainmodular.org 
//
// All dependencies are under there own licence.
//
//@LICENCE
// Copyright (C) 2013, 2014, 2015, 2019 BrainModular
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software 
// is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
//     copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE.
//
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------
#include "UserUtils.h"
#include <math.h>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

//-----------------------------------------------------------------------------
// global color utilities functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// calculate HSL from RGB
TColorAhsl sdkColorArgbToColorAhsl(TColorArgb c1)
{
    float themin,themax,delta;
    TColorAhsl c2;

	c2.a = c1.a;

    themin = min( c1.r, min(c1.g,c1.b) );
    themax = max( c1.r, max(c1.g,c1.b) );
    delta = themax - themin;
    c2.l = (themin + themax) / 2;
    c2.s = 0;
    if (c2.l > 0 && c2.l < 1)
        c2.s = delta / (c2.l < 0.5 ? (2*c2.l) : (2-2*c2.l));
    c2.h = 0;
    if (delta > 0) {
        if (themax == c1.r && themax != c1.g)
            c2.h += (c1.g - c1.b) / delta;
        if (themax == c1.g && themax != c1.b)
            c2.h += (2 + (c1.b - c1.r) / delta);
        if (themax == c1.b && themax != c1.r)
            c2.h += (4 + (c1.r - c1.g) / delta);
        c2.h *= 60;
    }
    
    while (c2.h < 0)
        c2.h += 360;
    while (c2.h > 360)
        c2.h -= 360;

    return c2;
}

//-----------------------------------------------------------------------------
// calculate RGB from HSL, reverse of RGB2HSL()
TColorArgb sdkColorAhslToColorArgb(TColorAhsl c1)
{
    TColorArgb c2,sat,ctmp;

	c2.a = c1.a;

    while (c1.h < 0)
        c1.h += 360;
    while (c1.h > 360)
        c1.h -= 360;

    if (c1.h < 120) {
        sat.r = (120 - c1.h) / 60.0f;
        sat.g = c1.h / 60.0f;
        sat.b = 0;
    } else if (c1.h < 240) {
        sat.r = 0;
        sat.g = (240 - c1.h) / 60.0f;
        sat.b = (c1.h - 120) / 60.0f;
    } else {
        sat.r = (c1.h - 240) / 60.0f;
        sat.g = 0;
        sat.b = (360 - c1.h) / 60.0f;
    }
    sat.r = min(sat.r, 1.0f);
    sat.g = min(sat.g, 1.0f);
    sat.b = min(sat.b, 1.0f);

    ctmp.r = 2 * c1.s * sat.r + (1 - c1.s);
    ctmp.g = 2 * c1.s * sat.g + (1 - c1.s);
    ctmp.b = 2 * c1.s * sat.b + (1 - c1.s);

    if (c1.l < 0.5) {
        c2.r = c1.l * ctmp.r;
        c2.g = c1.l * ctmp.g;
        c2.b = c1.l * ctmp.b;
    } else {
        c2.r = (1 - c1.l) * ctmp.r + 2 * c1.l - 1;
        c2.g = (1 - c1.l) * ctmp.g + 2 * c1.l - 1;
        c2.b = (1 - c1.l) * ctmp.b + 2 * c1.l - 1;
    }

    return c2;
}

//-----------------------------------------------------------------------------
// RGB color to Usine color format
TColorUsine sdkColorArgbToAlphaColor( TColorArgb c1 )
{
    TColorUsine c2 = 0;

    c2 += ((uint8_t)(c1.b * 255.0f));
    c2 += ((uint8_t)(c1.g * 255.0f)) << 8;
    c2 += ((uint8_t)(c1.r * 255.0f)) << 16;
    c2 += ((uint8_t)(c1.a * 255.0f)) << 24;

    return c2;
}

//-----------------------------------------------------------------------------
// Usine color format to RGB color
TColorArgb sdkAlphaColorToColorArgb(TColorUsine c1)
{
    TColorArgb c2;

    c2.b = ((float)( c1        & 0x000000FF)) / 255.0f;
    c2.g = ((float)((c1 >> 8)  & 0x000000FF)) / 255.0f;
    c2.r = ((float)((c1 >> 16) & 0x000000FF)) / 255.0f;
    c2.a = ((float)((c1 >> 24) & 0x000000FF)) / 255.0f;

    return c2;
}

//-----------------------------------------------------------------------------
// HSL color to Usine color format
TColorUsine sdkColorAhslToAlphaColor(TColorAhsl c1)
{
    TColorUsine c2 = sdkColorArgbToAlphaColor( sdkColorAhslToColorArgb( c1 ) );
    return c2;
}

//-----------------------------------------------------------------------------
// Usine color format to HSL color
TColorAhsl sdkAlphaColorToColorAhsl(TColorUsine c1)
{
    TColorAhsl c2 = sdkColorArgbToColorAhsl( sdkAlphaColorToColorArgb( c1 ) );
    return c2;
}

//-----------------------------------------------------------------------------
// RGB color constructor
TColorArgb sdkColorAgrb(float r, float g, float b, float a)
{
    TColorArgb c2;
    
    c2.a = a;
    c2.r = r;
    c2.g = g;
    c2.b = b;

    return c2;
}

//-----------------------------------------------------------------------------
// HSL color constructor
TColorAhsl sdkColorAhsl(float h, float s, float l, float a)
{
    TColorAhsl c2;

    c2.a = a;
    c2.h = h;
    c2.s = s;
    c2.l = l;

    return c2;
}

//-----------------------------------------------------------------------------
// Usine color constructor
TColorUsine sdkAlphaColor(  float r, float g, float b, float a)
{
    TColorUsine c2 = 0;

    c2 = sdkColorArgbToAlphaColor(sdkColorAgrb(r, g, b, a));

    return c2;
}

//-----------------------------------------------------------------------------
// Usine color constructor
TColorUsine sdkAlphaColor( int r, int g, int b, int a)
{
    TColorUsine c2 = 0;

    c2 = sdkColorArgbToAlphaColor(sdkColorAgrb(r / 255.f, g / 255.f, b / 255.f, a / 255.f));

    return c2;
}

//-----------------------------------------------------------------------------
// global geometric utilities functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TPointF constructor
TPointF sdkPointF(float x, float y)
{
    TPointF p1;

    p1.x = x;
    p1.y = y;

    return p1;
}

//-----------------------------------------------------------------------------
// Test if coordinate is in a rect bound
bool sdkPointInRect(int X, int Y, TPointI LeftTop, TPointI RightBottom)
{
    TPointI Coords;
    Coords.x = X;	
    Coords.y = Y;	
    return sdkPointInRect(Coords, LeftTop, RightBottom);
}

//-----------------------------------------------------------------------------
// Test if coordinate is in a rect bound
bool sdkPointInRect(TPointI Coords, TPointI LeftTop, TPointI RightBottom)
{
    return ((Coords.x >= LeftTop.x) && (Coords.y >= LeftTop.y)
        && (Coords.x < RightBottom.x)
        && (Coords.y < RightBottom.y));
}

//-----------------------------------------------------------------------------
// Test if coordinate is in a rect bound
bool sdkPointInRect(float X, float Y, TRectF rect)
{
    return ((X >= rect.left) && (Y >= rect.top) && (X < rect.right) && (Y < rect.bottom));
}

//-----------------------------------------------------------------------------
// Four control point Bezier interpolation
TPointF sdkBezier4(TPointF p1, TPointF p2, TPointF p3, TPointF p4, float mu)
{
    float mum1,mum13,mu3;
    TPointF p;

    mum1 = 1 - mu;
    mum13 = mum1 * mum1 * mum1;
    mu3 = mu * mu * mu;

    p.x = mum13*p1.x + 3*mu*mum1*mum1*p2.x + 3*mu*mu*mum1*p3.x + mu3*p4.x;
    p.y = mum13*p1.y + 3*mu*mum1*mum1*p2.y + 3*mu*mu*mum1*p3.y + mu3*p4.y;

    return(p);
}

//
float sdkBezier4(float p1, float p2, float p3, float p4, float mu)
{
    float mum1,mum13,mu3;
    float p;

    mum1 = 1 - mu;
    mum13 = mum1 * mum1 * mum1;
    mu3 = mu * mu * mu;

    p= mum13*p1 + 3*mu*mum1*mum1*p2 + 3*mu*mu*mum1*p3 + mu3*p4;

    return(p);
}

//
float sdkCoeffToDb(const float Coeff)
{
    float result = COEFF_TO_DB_MIN;

    if (Coeff > 0.0f)
        result = log10(Coeff) / LN10S20;

    return result;
}

float sdkDbToCoeff(const float Gain)
{
    float result = 0.0f;
    if (Gain == 0.0f)
        result = 1.0f;
    else if (Gain > GAIN_MIN)
        result = exp(Gain * LN10S20);
    return result;
}

//
std::string sdkFloatToString(const float Number, const int Prec, const int Width)
{
    std::ostringstream os;
    os << std::setiosflags(std::ios::fixed) << std::setprecision(Prec) << Number; // << std::setw(Width) 
    return os.str();
}

//
std::string sdkIntToString(const int Number)
{
    std::ostringstream os;
    os << Number; // << std::setw(Width) 
    return os.str();
}

//-----------------------------------------------------------------------------
#if (defined (USINE_OSX32) || defined (USINE_OSX64))
//----------------------------------------------------------------------------
//
//#include <mach/exc.defs>
//#include <defs.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <mach/mach.h>
#include <mach/mach_error.h>
#include <mach/thread_status.h>
#include <mach/exception.h>
#include <mach/task.h>
#include <pthread.h>
#include <sys/mman.h>

#define DIE(x) do { fprintf(stderr,"%s failed at %d\n",x,__LINE__); exit(1); } while(0)
#define ABORT(x) do { fprintf(stderr,"%s at %d\n",x,__LINE__); } while(0)

/* this is not specific to mach exception handling, its just here to separate required mach code from YOUR code */
static int my_handle_exn(char *addr, integer_t code);


/* These are not defined in any header, although they are documented */
extern "C"
{
    boolean_t exc_server(mach_msg_header_t *,mach_msg_header_t *);
    kern_return_t exception_raise(
                                  mach_port_t,mach_port_t,mach_port_t,
                                  exception_type_t,exception_data_t,mach_msg_type_number_t);
    kern_return_t exception_raise_state(
                                        mach_port_t,mach_port_t,mach_port_t,
                                        exception_type_t,exception_data_t,mach_msg_type_number_t,
                                        thread_state_flavor_t*,thread_state_t,mach_msg_type_number_t,
                                        thread_state_t,mach_msg_type_number_t*);
    kern_return_t exception_raise_state_identity(
                                                 mach_port_t,mach_port_t,mach_port_t,
                                                 exception_type_t,exception_data_t,mach_msg_type_number_t,
                                                 thread_state_flavor_t*,thread_state_t,mach_msg_type_number_t,
                                                 thread_state_t,mach_msg_type_number_t*);
}

#define MAX_EXCEPTION_PORTS 16

static struct {
    mach_msg_type_number_t count;
    exception_mask_t      masks[MAX_EXCEPTION_PORTS];
    exception_handler_t   ports[MAX_EXCEPTION_PORTS];
    exception_behavior_t  behaviors[MAX_EXCEPTION_PORTS];
    thread_state_flavor_t flavors[MAX_EXCEPTION_PORTS];
} old_exc_ports;

static mach_port_t exception_port;

static void *exc_thread(void *junk) {
    mach_msg_return_t r;
    /* These two structures contain some private kernel data. We don't need to
     access any of it so we don't bother defining a proper struct. The
     correct definitions are in the xnu source code. */
    struct {
        mach_msg_header_t head;
        char data[256];
    } reply;
    struct {
        mach_msg_header_t head;
        mach_msg_body_t msgh_body;
        char data[1024];
    } msg;
    
    for(;;) {
        r = mach_msg(
                     &msg.head,
                     MACH_RCV_MSG|MACH_RCV_LARGE,
                     0,
                     sizeof(msg),
                     exception_port,
                     MACH_MSG_TIMEOUT_NONE,
                     MACH_PORT_NULL);
        if(r != MACH_MSG_SUCCESS) DIE("mach_msg");
        
        /* Handle the message (calls catch_exception_raise) */
        if(!exc_server(&msg.head,&reply.head)) DIE("exc_server");
        
        /* Send the reply */
        r = mach_msg(
                     &reply.head,
                     MACH_SEND_MSG,
                     reply.head.msgh_size,
                     0,
                     MACH_PORT_NULL,
                     MACH_MSG_TIMEOUT_NONE,
                     MACH_PORT_NULL);
        if(r != MACH_MSG_SUCCESS) DIE("mach_msg");
    }
    /* not reached */
}

void initExceptionHandler ()
{
    kern_return_t r;
    mach_port_t me;
    pthread_t thread;
    pthread_attr_t attr;
    exception_mask_t mask;
    
    me = mach_task_self();
    r = mach_port_allocate(me,MACH_PORT_RIGHT_RECEIVE,&exception_port);
    if(r != MACH_MSG_SUCCESS) DIE("mach_port_allocate");
    
    r = mach_port_insert_right(me,exception_port,exception_port,
                               MACH_MSG_TYPE_MAKE_SEND);
    if(r != MACH_MSG_SUCCESS) DIE("mach_port_insert_right");
    
    /* for others see mach/exception_types.h */
    //mask = EXC_MASK_BAD_ACCESS;
    //mask = EXC_MASK_ALL;
    mask = EXC_MASK_BAD_ACCESS | EXC_MASK_ARITHMETIC | EXC_MASK_BAD_INSTRUCTION;
    
    /* get the old exception ports */
    r = task_get_exception_ports(
                                 me,
                                 mask,
                                 old_exc_ports.masks,
                                 &old_exc_ports.count,
                                 old_exc_ports.ports,
                                 old_exc_ports.behaviors,
                                 old_exc_ports.flavors
                                 );
    if(r != MACH_MSG_SUCCESS) DIE("task_get_exception_ports");
    
    /* set the new exception ports */
    r = task_set_exception_ports(
                                 me,
                                 mask,
                                 exception_port,
                                 //EXCEPTION_DEFAULT,
                                 EXCEPTION_STATE_IDENTITY,
                                 MACHINE_THREAD_STATE
                                 );
    if(r != MACH_MSG_SUCCESS) DIE("task_set_exception_ports");
    
    if(pthread_attr_init(&attr) != 0) DIE("pthread_attr_init");
    if(pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED) != 0)
        DIE("pthread_attr_setdetachedstate");
    
    if(pthread_create(&thread,&attr,exc_thread,NULL) != 0)
        DIE("pthread_create");
    pthread_attr_destroy(&attr);
}

/* The source code for Apple's GDB was used as a reference for the exception
 forwarding code. This code is similar to be GDB code only because there is
 only one way to do it. */
static kern_return_t forward_exception(
                                       mach_port_t thread,
                                       mach_port_t task,
                                       exception_type_t exception,
                                       exception_data_t data,
                                       mach_msg_type_number_t data_count
                                       ) {
    int i;
    kern_return_t r;
    mach_port_t port;
    exception_behavior_t behavior;
    thread_state_flavor_t flavor;
    
    thread_state_data_t thread_state;
    mach_msg_type_number_t thread_state_count = THREAD_STATE_MAX;
    
    for(i=0;i<old_exc_ports.count;i++)
        if(old_exc_ports.masks[i] & (1 << exception))
            break;
    if(i==old_exc_ports.count) ABORT("No handler for exception!");
    
    port = old_exc_ports.ports[i];
    behavior = old_exc_ports.behaviors[i];
    flavor = old_exc_ports.flavors[i];
    
    if(behavior != EXCEPTION_DEFAULT) {
        r = thread_get_state(thread,flavor,thread_state,&thread_state_count);
        if(r != KERN_SUCCESS)
            ABORT("thread_get_state failed in forward_exception");
    }
    
    switch(behavior) {
        case EXCEPTION_DEFAULT:
            r = exception_raise(port,thread,task,exception,data,data_count);
            break;
        case EXCEPTION_STATE:
            r = exception_raise_state(port,thread,task,exception,data,
                                      data_count,&flavor,thread_state,thread_state_count,
                                      thread_state,&thread_state_count);
            break;
        case EXCEPTION_STATE_IDENTITY:
            r = exception_raise_state_identity(port,thread,task,exception,data,
                                               data_count,&flavor,thread_state,thread_state_count,
                                               thread_state,&thread_state_count);
            break;
        default:
            r = KERN_FAILURE; /* make gcc happy */
            ABORT("forward_exception: unknown behavior");
            break;
    }
    
    if(behavior != EXCEPTION_DEFAULT) {
        r = thread_set_state(thread,flavor,thread_state,thread_state_count);
        if(r != KERN_SUCCESS)
            ABORT("thread_set_state failed in forward_exception");
    }
    
    return r;
}

#define FWD() forward_exception(thread,task,exception,code,code_count)

kern_return_t
catch_exception_raise(
                      mach_port_t exception_port,mach_port_t thread,mach_port_t task,
                      exception_type_t exception,exception_data_t code,
                      mach_msg_type_number_t code_count
                      ) {
    kern_return_t r;
    char *addr;
#ifdef __POWERPC__
    thread_state_flavor_t flavor = PPC_EXCEPTION_STATE;
    mach_msg_type_number_t exc_state_count = PPC_EXCEPTION_STATE_COUNT;
    ppc_exception_state_t exc_state;
#elif defined(__x86_64__)
    thread_state_flavor_t flavor = x86_EXCEPTION_STATE;
    mach_msg_type_number_t exc_state_count = x86_EXCEPTION_STATE_COUNT;
    x86_exception_state_t exc_state;
#elif defined(__arm64__)
    thread_state_flavor_t flavor = ARM_EXCEPTION_STATE64;
    mach_msg_type_number_t exc_state_count = ARM_EXCEPTION_STATE64_COUNT;
    arm_exception_state_t exc_state;
#endif

    
    
    /* we should never get anything that isn't EXC_BAD_ACCESS, but just in case */
    if(exception != EXC_BAD_ACCESS) {
        /* We aren't interested, pass it on to the old handler */
        fprintf(stderr,"Exception: 0x%x Code: 0x%x 0x%x in catch....\n",
                exception,
                code_count > 0 ? code[0] : -1,
                code_count > 1 ? code[1] : -1);
        return FWD();
    }
    
    r = thread_get_state(thread,flavor,
                         (natural_t*)&exc_state,&exc_state_count);
    if(r != KERN_SUCCESS) DIE("thread_get_state");
    
    /* This is the address that caused the fault */
    addr = (char*)
#ifdef __POWERPC__
    exc_state.dar;
#elif defined(__x86_64__)
    exc_state.ues.es64.__faultvaddr;
#elif defined(__arm64__)
    exc_state.__far;
#endif
    
    /* you could just as easily put your code in here, I'm just doing this to
     point out the required code */
    if(!my_handle_exn(addr, code[0])) return FWD();
    
    return KERN_SUCCESS;
}
#undef FWD

/* These should never be called, but just in case...  */
kern_return_t catch_exception_raise_state(mach_port_name_t exception_port,
                                          int exception, exception_data_t code, mach_msg_type_number_t codeCnt,
                                          int flavor, thread_state_t old_state, int old_stateCnt,
                                          thread_state_t new_state, int new_stateCnt)
{
    ABORT("catch_exception_raise_state");
    return(KERN_INVALID_ARGUMENT);
}
kern_return_t catch_exception_raise_state_identity(
                                                   mach_port_name_t exception_port, mach_port_t thread, mach_port_t task,
                                                   int exception, exception_data_t code, mach_msg_type_number_t codeCnt,
                                                   int flavor, thread_state_t old_state, int old_stateCnt,
                                                   thread_state_t new_state, int new_stateCnt)
{
    ABORT("catch_exception_raise_state_identity");
    return(KERN_INVALID_ARGUMENT);
}

static char *data;

static int my_handle_exn(char *addr, integer_t code)
{
    if(code == KERN_INVALID_ADDRESS)
    {
        fprintf(stderr,"Got KERN_INVALID_ADDRESS at %p\n",addr);
        exit(1);
    }
    if(code == KERN_PROTECTION_FAILURE)
    {
        fprintf(stderr,"Got KERN_PROTECTION_FAILURE at %p\n",addr);
        if(addr == NULL)
        {
            fprintf(stderr,"Tried to dereference NULL");
            exit(1);
        }
        else //if(addr == data)
        {
            fprintf(stderr,"Making data (%p) writeable\n",addr);
            if(mprotect(addr,4096,PROT_READ|PROT_WRITE) < 0) DIE("mprotect");
            return 1; // we handled it
        }
        //fprintf(stderr,"Got KERN_PROTECTION_FAILURE at %p\n",addr);
        //return 0; // forward it
    }
    
    /* You should filter out anything you don't want in the catch_exception_raise... above
     and forward it */
    fprintf(stderr,"Got unknown code %d at %p\n",(int)code,addr);
    return 0;
}

//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------
