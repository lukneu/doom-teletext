//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	DOOM graphics stuff for SDL.
//


#include "SDL/SDL.h"
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/keyboard.h>
#include <linux/kd.h>

#include "config.h"
#include "deh_str.h"
#include "doomtype.h"
#include "doomkeys.h"
#include "i_joystick.h"
#include "i_system.h"
#include "i_swap.h"
#include "i_timer.h"
#include "i_video.h"
#include "i_scale.h"
#include "m_argv.h"
#include "m_config.h"
#include "m_misc.h"
#include "tables.h"
#include "v_video.h"
#include "w_wad.h"
#include "z_zone.h"

int vanilla_keyboard_mapping = 1;

// Is the shift key currently down?

static int shiftdown = 0;
// Lookup table for mapping ASCII characters to their equivalent when
// shift is pressed on an American layout keyboard:

static const char shiftxform[] =
{
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
    31, ' ', '!', '"', '#', '$', '%', '&',
    '"', // shift-'
    '(', ')', '*', '+',
    '<', // shift-,
    '_', // shift--
    '>', // shift-.
    '?', // shift-/
    ')', // shift-0
    '!', // shift-1
    '@', // shift-2
    '#', // shift-3
    '$', // shift-4
    '%', // shift-5
    '^', // shift-6
    '&', // shift-7
    '*', // shift-8
    '(', // shift-9
    ':',
    ':', // shift-;
    '<',
    '+', // shift-=
    '>', '?', '@',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '[', // shift-[
    '!', // shift-backslash - OH MY GOD DOES WATCOM SUCK
    ']', // shift-]
    '"', '_',
    '\'', // shift-`
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '{', '|', '}', '~', 127
};

/* Checks whether or not the given file descriptor is associated
   with a local keyboard.
   Returns 1 if it is, 0 if not (or if something prevented us from
   checking). */

int tty_is_kbd(int fd)
{
    int data = 0;

    if (ioctl(fd, KDGKBTYPE, &data) != 0)
        return 0;

    if (data == KB_84) {
        printf("84-key keyboard found.\n");
        return 1;
    } else if (data == KB_101) {
        printf("101-key keyboard found.\n");
        return 1;
    } else {
        printf("KDGKBTYPE = 0x%x.\n", data);
        return 0;
    }
}

static int old_mode = -1;
static struct termios old_term;
static int kb = -1; /* keyboard file descriptor */

void kbd_shutdown(void)
{
    /* Shut down nicely. */

    printf("Cleaning up.\n");
    fflush(stdout);

    printf("Exiting normally.\n");
    if (old_mode != -1) {
        ioctl(kb, KDSKBMODE, old_mode);
        tcsetattr(kb, 0, &old_term);
    }

    if (kb > 3)
        close(kb);

    exit(0);
}

static int kbd_init(void)
{
    struct termios new_term;
    char *files_to_try[] = {"/dev/tty", "/dev/console", NULL};
    int i;
    int flags;

    printf(">>>>>>>>>>>>>> %s\r\n", __func__);

    /* First we need to find a file descriptor that represents the
       system's keyboard. This should be /dev/tty, /dev/console,
       stdin, stdout, or stderr. We'll try them in that order.
       If none are acceptable, we're probably not being run
       from a VT. */
    for (i = 0; files_to_try[i] != NULL; i++) {
        /* Try to open the file. */
        kb = open(files_to_try[i], O_RDONLY);
        if (kb < 0) continue;
        /* See if this is valid for our purposes. */
        if (tty_is_kbd(kb)) {
            printf("Using keyboard on %s.\n", files_to_try[i]);
            break;
        }
        close(kb);
    }

    /* If those didn't work, not all is lost. We can try the
       3 standard file descriptors, in hopes that one of them
       might point to a console. This is not especially likely. */
    if (files_to_try[i] == NULL) {
        for (kb = 0; kb < 3; kb++) {
            if (tty_is_kbd(i)) break;
        }
        printf("Unable to find a file descriptor associated with "\
                "the keyboard.\n" \
                "Perhaps you're not using a virtual terminal?\n");
        return 1;
    }

    /* Find the keyboard's mode so we can restore it later. */
    if (ioctl(kb, KDGKBMODE, &old_mode) != 0) {
        printf("Unable to query keyboard mode.\n");
        kbd_shutdown();
    }

    /* Adjust the terminal's settings. In particular, disable
       echoing, signal generation, and line buffering. Any of
       these could cause trouble. Save the old settings first. */
    if (tcgetattr(kb, &old_term) != 0) {
        printf("Unable to query terminal settings.\n");
        kbd_shutdown();
    }

    new_term = old_term;
    new_term.c_iflag = 0;
    new_term.c_lflag &= ~(ECHO | ICANON | ISIG);

    /* TCSAFLUSH discards unread input before making the change.
       A good idea. */
    if (tcsetattr(kb, TCSAFLUSH, &new_term) != 0) {
        printf("Unable to change terminal settings.\n");
    }
    
    /* Put the keyboard in mediumraw mode. */
    if (ioctl(kb, KDSKBMODE, K_MEDIUMRAW) != 0) {
        printf("Unable to set mediumraw mode.\n");
        kbd_shutdown();
    }

    /* Put in non-blocking mode */
    flags = fcntl(kb, F_GETFL, 0);
    fcntl(kb, F_SETFL, flags | O_NONBLOCK);

    printf("Ready to read keycodes. Press Backspace to exit.\n");

    return 0;
}

int kbd_read(int *pressed, unsigned char *key)
{
    unsigned char data;

    if (read(kb, &data, 1) < 1) {
        return 0;
    }

    *pressed = (data & 0x80) == 0x80;
    *key = data & 0x7F;

    /* Print the keycode. The top bit is the pressed/released
       flag, and the lower seven are the keycode. */
    printf("%s: 0x%2X (%i)\n", *pressed ? "Released" : " Pressed", (unsigned int)*key, (unsigned int)*key);
    //if (*key == 0x0E) {
    //    printf("Backspace pressed.\n");
    //    kbd_shutdown();
    //}

    return 1;
}


//
// Translates the SDL key
//

#define TTY_ESC   (0x01)
#define TTY_ENTER (0x1C)
#define TTY_W     (0x11)
#define TTY_A     (0x1E)
#define TTY_S     (0x1F)
#define TTY_D     (0x20)
#define TTY_LCTRL (0x1D)
#define TTY_RCTRL (0x61)
#define TTY_UP    (0x67)
#define TTY_LEFT  (0x69)
#define TTY_DOWN  (0x6C)
#define TTY_RIGHT (0x6A)
#define TTY_Y     (0x15)
#define TTY_N     (0x31)

#if 0
enum sdl_keys {
	SDLK_a			= 97,
	SDLK_b			= 98,
	SDLK_c			= 99,
	SDLK_d			= 100,
	SDLK_e			= 101,
	SDLK_f			= 102,
	SDLK_g			= 103,
	SDLK_h			= 104,
	SDLK_i			= 105,
	SDLK_j			= 106,
	SDLK_k			= 107,
	SDLK_l			= 108,
	SDLK_m			= 109,
	SDLK_n			= 110,
	SDLK_o			= 111,
	SDLK_p			= 112,
	SDLK_q			= 113,
	SDLK_r			= 114,
	SDLK_s			= 115,
	SDLK_t			= 116,
	SDLK_u			= 117,
	SDLK_v			= 118,
	SDLK_w			= 119,
	SDLK_x			= 120,
	SDLK_y			= 121,
	SDLK_z			= 122,
};
#endif

static unsigned char TranslateKey(unsigned char key)
{
    switch(key)
    {
        case TTY_ENTER:     return KEY_ENTER;
        case TTY_ESC:       return KEY_ESCAPE;
        case TTY_W:         return 'w';
        case TTY_A:         return 'a';
        case TTY_S:         return 's';
        case TTY_D:         return 'd';
        case TTY_LEFT:	    return KEY_LEFTARROW;
        case TTY_RIGHT:	    return KEY_RIGHTARROW;
        case TTY_DOWN:	    return KEY_DOWNARROW;
        case TTY_UP:	    return KEY_UPARROW;
        case TTY_Y:         printf("Y\r\n"); return SDLK_y;
        case TTY_N:         printf("N\r\n"); return SDLK_n;

        case TTY_LCTRL:
        case TTY_RCTRL:
                            return KEY_RCTRL;

//      case SDLK_TAB:	return KEY_TAB;
//      case SDLK_F1:	return KEY_F1;
//      case SDLK_F2:	return KEY_F2;
//      case SDLK_F3:	return KEY_F3;
//      case SDLK_F4:	return KEY_F4;
//      case SDLK_F5:	return KEY_F5;
//      case SDLK_F6:	return KEY_F6;
//      case SDLK_F7:	return KEY_F7;
//      case SDLK_F8:	return KEY_F8;
//      case SDLK_F9:	return KEY_F9;
//      case SDLK_F10:	return KEY_F10;
//      case SDLK_F11:	return KEY_F11;
//      case SDLK_F12:	return KEY_F12;
//      case SDLK_PRINT:  return KEY_PRTSCR;
//
//      case SDLK_BACKSPACE: return KEY_BACKSPACE;
//      case SDLK_DELETE:	return KEY_DEL;
//
//      case SDLK_PAUSE:	return KEY_PAUSE;
//
//      case SDLK_MINUS:  return KEY_MINUS;
//
//      case SDLK_LSHIFT:
//      case SDLK_RSHIFT:
//	return KEY_RSHIFT;
//	
//      case SDLK_LCTRL:
//      case SDLK_RCTRL:
//	return KEY_RCTRL;
//	
//      case SDLK_LALT:
//      case SDLK_RALT:
//        return KEY_RALT;
//
//      case SDLK_CAPSLOCK: return KEY_CAPSLOCK;
//      case SDLK_SCROLLOCK: return KEY_SCRLCK;
//      case SDLK_NUMLOCK: return KEY_NUMLOCK;
//
//      case SDLK_KP0: return KEYP_0;
//      case SDLK_KP1: return KEYP_1;
//      case SDLK_KP2: return KEYP_2;
//      case SDLK_KP3: return KEYP_3;
//      case SDLK_KP4: return KEYP_4;
//      case SDLK_KP5: return KEYP_5;
//      case SDLK_KP6: return KEYP_6;
//      case SDLK_KP7: return KEYP_7;
//      case SDLK_KP8: return KEYP_8;
//      case SDLK_KP9: return KEYP_9;
//
//      case SDLK_KP_PERIOD:   return KEYP_PERIOD;
//      case SDLK_KP_MULTIPLY: return KEYP_MULTIPLY;
//      case SDLK_KP_PLUS:     return KEYP_PLUS;
//      case SDLK_KP_MINUS:    return KEYP_MINUS;
//      case SDLK_KP_DIVIDE:   return KEYP_DIVIDE;
//      case SDLK_KP_EQUALS:   return KEYP_EQUALS;
//      case SDLK_KP_ENTER:    return KEYP_ENTER;
//
//      case SDLK_HOME: return KEY_HOME;
//      case SDLK_INSERT: return KEY_INS;
//      case SDLK_END: return KEY_END;
//      case SDLK_PAGEUP: return KEY_PGUP;
//      case SDLK_PAGEDOWN: return KEY_PGDN;
//
//#ifdef SDL_HAVE_APP_KEYS
//        case SDLK_APP1:        return KEY_F1;
//        case SDLK_APP2:        return KEY_F2;
//        case SDLK_APP3:        return KEY_F3;
//        case SDLK_APP4:        return KEY_F4;
//        case SDLK_APP5:        return KEY_F5;
//        case SDLK_APP6:        return KEY_F6;
//#endif

      //default:
      //  return tolower(key);
    }
}

// Get the equivalent ASCII (Unicode?) character for a keypress.

static unsigned char GetTypedChar(unsigned char key)
{
    key = TranslateKey(key);

    // Is shift held down?  If so, perform a translation.

    //if (shiftdown > 0)
    //{
    //    if (key >= 0 && key < arrlen(shiftxform))
    //    {
    //        key = shiftxform[key];
    //    }
    //    else
    //    {
    //        key = 0;
    //    }
    //}

    return key;
}

static void UpdateShiftStatus(int pressed, unsigned char key)
{
    int change;

    if (pressed) {
        change = 1;
    } else {
        change = -1;
    }

    if (key == SDLK_LSHIFT || key == SDLK_RSHIFT) {
        shiftdown += change;
    }
}


void I_GetEvent(void)
{
    event_t event;
    int pressed;
    unsigned char key;

    // put event-grabbing stuff in here
    
    while (kbd_read(&pressed, &key))
    {
        if (key == 0x0E) {
            kbd_shutdown();
            I_Quit();
        }

        //UpdateShiftStatus(pressed, key);

        // process event
        
        if (!pressed)
        {
            // data1 has the key pressed, data2 has the character
            // (shift-translated, etc)
            event.type = ev_keydown;
            event.data1 = TranslateKey(key);
            event.data2 = GetTypedChar(key);

            if (event.data1 != 0)
            {
                printf("Post keydown event.data1 = 0x%x\r\n", event.data1);
                D_PostEvent(&event);
            }
        }
        else
        {
            event.type = ev_keyup;
            event.data1 = TranslateKey(key);

            // data2 is just initialized to zero for ev_keyup.
            // For ev_keydown it's the shifted Unicode character
            // that was typed, but if something wants to detect
            // key releases it should do so based on data1
            // (key ID), not the printable char.

            event.data2 = 0;

            if (event.data1 != 0)
            {
                printf("Post keyup event.data1 = 0x%x\r\n", event.data1);
                D_PostEvent(&event);
            }
            break;
        }
    }


                /*
            case SDL_MOUSEMOTION:
                event.type = ev_mouse;
                event.data1 = mouse_button_state;
                event.data2 = AccelerateMouse(sdlevent.motion.xrel);
                event.data3 = -AccelerateMouse(sdlevent.motion.yrel);
                D_PostEvent(&event);
                break;
                */
}


//void I_ShutdownGraphics(void)
//{
//    //SDL_QuitSubSystem(SDL_INIT_VIDEO);
//}

//
// I_StartTic
//
//void I_StartTic (void)

void I_InitInput(void)
{
    SDL_Event dummy;

    kbd_init();

    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        I_Error("Failed to initialize video: %s", SDL_GetError());
    }

    //UpdateFocus();

    // We need SDL to give us translated versions of keys as well

    SDL_EnableUNICODE(1);

    // Repeat key presses - this is what Vanilla Doom does
    // Not sure about repeat rate - probably dependent on which DOS
    // driver is used.  This is good enough though.

    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    // clear out any events waiting at the start and center the mouse
  
    while (SDL_PollEvent(&dummy));
}

