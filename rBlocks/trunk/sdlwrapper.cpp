/*******************************************************************************
*
*   Title:		SDLWrapper
*   Author:	    Rob Loach (http://robloach.net)
*   Date:		November 18th, 2004
*
*   Description:
*				A simple SDL wrapper class.
*
*   Usage:
*               CSDLWrapper SDLWrapper(640, 480, false, "Title");
*               while(SDLWrapper.StillGoing()){
*                   // Code
*               }
*
*******************************************************************************/


/*******************************************************************************
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*******************************************************************************/
 
 
// Includes
#include "sdlwrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

// Initiate
// Loads SDL as well as gets the window ready for painting
bool CSDLWrapper::Initiate(int screenWidth, int screenHeight, bool fullScreen, char windowTitle[]){

    // Initiate SDL
    if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 )
        return false;
        
    atexit(SDL_Quit); // At quit, destroy SDL
    
    // Set up the window and video mode
    if(fullScreen)
        screen = SDL_SetVideoMode(screenWidth,screenHeight,32,SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN);
    else
        screen = SDL_SetVideoMode(screenWidth,screenHeight,32,SDL_HWSURFACE|SDL_DOUBLEBUF);
    if ( screen == NULL )
        return false;
    
    // Setup the caption of the window
    SDL_WM_SetCaption(windowTitle, NULL);
    
    // Default values
    m_going = true;
    m_TickCount = 20;
    m_ignoreESC = false;
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
    
    return true;
}
CSDLWrapper::CSDLWrapper(int screenWidth, int screenHeight, bool fullScreen, char windowTitle[]){
    Initiate(screenWidth,screenHeight, fullScreen, windowTitle);
}
CSDLWrapper::CSDLWrapper(int screenWidth, int screenHeight, bool fullScreen){
    Initiate(screenWidth,screenHeight, fullScreen, "SDL Application");
}
CSDLWrapper::CSDLWrapper(int screenWidth, int screenHeight){
    Initiate(screenWidth,screenHeight, false, "SDL Application");
}       
CSDLWrapper::CSDLWrapper(){
    //Initiate(640, 480, false, "SDL Application");
}      

// Still Going
// Called during the main game while loop.
// Flips the screen as well as provides some timing functions
bool CSDLWrapper::StillGoing(){

    if(m_ignoreESC == false){
    	SDL_Event event;
    	while ( SDL_PollEvent(&event) ){
        	switch(event.type){
            case SDL_QUIT:
                return false;
                break;
            case SDL_KEYDOWN:
               	if(event.key.keysym.sym == SDLK_ESCAPE)
                   	return false;
                break;
            case SDL_MOUSEMOTION:
                m_mousex = event.motion.x;
                m_mousey = event.motion.y;
                break;
            default:
                break;
			}
        }
    }
    
    // Paint the buffer to the monitor
    SDL_Flip(screen);
	SDL_FillRect(screen, NULL, 0); // Clear the buffer black

	// Provide time delay
	Uint32 now;
	static Uint32 next_time;
    now = SDL_GetTicks();
    if(next_time > now)
        SDL_Delay(next_time - now);
    next_time += m_TickCount;
	
    return m_going;
}

// Put Pixel
// Use the MakeColour function to draw a pixel to the screen at (x,y)
void CSDLWrapper::PutPixel(int x, int y, Uint32 pixel)
{
    int bpp = screen->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

// Draw Line
// Draws a line from (x1,y1) to (x2,y2).
// Use the Make Colour function to create the colour
void CSDLWrapper::DrawLine(int x1, int y1, int x2, int y2, Uint32 color){
	double m = (double)(y2 - y1) / (double)(x2 - x1); // define the slope
	int y; // this will change as we vary x
	for (int x = x1; x <= x2; x++)
	{
		y = (int)(m * (x - x1)) + y1;
		PutPixel(x, y, color);
	}
}

// Make Colour
// Create a SDL colour from the R GB values.
Uint32 CSDLWrapper::MakeColour(int r, int g, int b){
    return SDL_MapRGB(screen->format, r, g, b);
}

char* CSDLWrapper::GetKeyName(int key){
	switch(key){
	case SDLK_BACKSPACE: return "Backspace";
	case SDLK_TAB: return "Tab";
	case SDLK_CLEAR: return "Clear";
	case SDLK_RETURN: return "Enter";
	case SDLK_PAUSE: return "Pause";
	case SDLK_ESCAPE: return "Escape";
	case SDLK_SPACE: return "Space";
	case SDLK_EXCLAIM: return "!";
	case SDLK_QUOTEDBL: return "\"";
	case SDLK_HASH: return "#";
	case SDLK_DOLLAR: return "$";
	case SDLK_AMPERSAND: return "&";
	case SDLK_QUOTE: return "'";
	case SDLK_LEFTPAREN: return "(";
	case SDLK_RIGHTPAREN: return ")";
	case SDLK_ASTERISK: return "*";
	case SDLK_PLUS: return "+";
	case SDLK_COMMA: return ",";
	case SDLK_MINUS: return "-";
	case SDLK_PERIOD: return ".";
	case SDLK_SLASH: return "/";
	case SDLK_0: return "0";
	case SDLK_1: return "1";
	case SDLK_2: return "2";
	case SDLK_3: return "3";
	case SDLK_4: return "4";
	case SDLK_5: return "5";
	case SDLK_6: return "6";
	case SDLK_7: return "7";
	case SDLK_8: return "8";
	case SDLK_9: return "9";
	case SDLK_COLON: return ":";
	case SDLK_SEMICOLON: return ";";
	case SDLK_LESS: return "<";
	case SDLK_EQUALS: return "=";
	case SDLK_GREATER: return ">";
	case SDLK_QUESTION: return "?";
	case SDLK_AT: return "@";
	case SDLK_LEFTBRACKET: return "[";
	case SDLK_BACKSLASH: return "\\";
	case SDLK_RIGHTBRACKET: return "]";
	case SDLK_CARET: return "^";
	case SDLK_UNDERSCORE: return "_";
	case SDLK_BACKQUOTE: return "`";
	case SDLK_a: return "a";
	case SDLK_b: return "b";
	case SDLK_c: return "c";
	case SDLK_d: return "d";
	case SDLK_e: return "e";
	case SDLK_f: return "f";
	case SDLK_g: return "g";
	case SDLK_h: return "h";
	case SDLK_i: return "i";
	case SDLK_j: return "j";
	case SDLK_k: return "k";
	case SDLK_l: return "l";
	case SDLK_m: return "m";
	case SDLK_n: return "n";
	case SDLK_o: return "o";
	case SDLK_p: return "p";
	case SDLK_q: return "q";
	case SDLK_r: return "r";
	case SDLK_s: return "s";
	case SDLK_t: return "t";
	case SDLK_u: return "u";
	case SDLK_v: return "v";
	case SDLK_w: return "w";
	case SDLK_x: return "x";
	case SDLK_y: return "y";
	case SDLK_z: return "z";
	case SDLK_DELETE: return "delete";

	case SDLK_WORLD_0: return "World 0";
	case SDLK_WORLD_1: return "World 1";
	case SDLK_WORLD_2: return "World 2";
	case SDLK_WORLD_3: return "World 3";
	case SDLK_WORLD_4: return "World 4";
	case SDLK_WORLD_5: return "World 5";
	case SDLK_WORLD_6: return "World 6";
	case SDLK_WORLD_7: return "World 7";
	case SDLK_WORLD_8: return "World 8";
	case SDLK_WORLD_9: return "World 9";
	case SDLK_WORLD_10: return "World 10";
	case SDLK_WORLD_11: return "World 11";
	case SDLK_WORLD_12: return "World 12";
	case SDLK_WORLD_13: return "World 13";
	case SDLK_WORLD_14: return "World 14";
	case SDLK_WORLD_15: return "World 15";
	case SDLK_WORLD_16: return "World 16";
	case SDLK_WORLD_17: return "World 17";
	case SDLK_WORLD_18: return "World 18";
	case SDLK_WORLD_19: return "World 19";
	case SDLK_WORLD_20: return "World 20";
	case SDLK_WORLD_21: return "World 21";
	case SDLK_WORLD_22: return "World 22";
	case SDLK_WORLD_23: return "World 23";
	case SDLK_WORLD_24: return "World 24";
	case SDLK_WORLD_25: return "World 25";
	case SDLK_WORLD_26: return "World 26";
	case SDLK_WORLD_27: return "World 27";
	case SDLK_WORLD_28: return "World 28";
	case SDLK_WORLD_29: return "World 29";
	case SDLK_WORLD_30: return "World 30";
	case SDLK_WORLD_31: return "World 31";
	case SDLK_WORLD_32: return "World 32";
	case SDLK_WORLD_33: return "World 33";
	case SDLK_WORLD_34: return "World 34";
	case SDLK_WORLD_35: return "World 35";
	case SDLK_WORLD_36: return "World 36";
	case SDLK_WORLD_37: return "World 37";
	case SDLK_WORLD_38: return "World 38";
	case SDLK_WORLD_39: return "World 39";
	case SDLK_WORLD_40: return "World 40";
	case SDLK_WORLD_41: return "World 41";
	case SDLK_WORLD_42: return "World 42";
	case SDLK_WORLD_43: return "World 43";
	case SDLK_WORLD_44: return "World 44";
	case SDLK_WORLD_45: return "World 45";
	case SDLK_WORLD_46: return "World 46";
	case SDLK_WORLD_47: return "World 47";
	case SDLK_WORLD_48: return "World 48";
	case SDLK_WORLD_49: return "World 49";
	case SDLK_WORLD_50: return "World 50";
	case SDLK_WORLD_51: return "World 51";
	case SDLK_WORLD_52: return "World 52";
	case SDLK_WORLD_53: return "World 53";
	case SDLK_WORLD_54: return "World 54";
	case SDLK_WORLD_55: return "World 55";
	case SDLK_WORLD_56: return "World 56";
	case SDLK_WORLD_57: return "World 57";
	case SDLK_WORLD_58: return "World 58";
	case SDLK_WORLD_59: return "World 59";
	case SDLK_WORLD_60: return "World 60";
	case SDLK_WORLD_61: return "World 61";
	case SDLK_WORLD_62: return "World 62";
	case SDLK_WORLD_63: return "World 63";
	case SDLK_WORLD_64: return "World 64";
	case SDLK_WORLD_65: return "World 65";
	case SDLK_WORLD_66: return "World 66";
	case SDLK_WORLD_67: return "World 67";
	case SDLK_WORLD_68: return "World 68";
	case SDLK_WORLD_69: return "World 69";
	case SDLK_WORLD_70: return "World 70";
	case SDLK_WORLD_71: return "World 71";
	case SDLK_WORLD_72: return "World 72";
	case SDLK_WORLD_73: return "World 73";
	case SDLK_WORLD_74: return "World 74";
	case SDLK_WORLD_75: return "World 75";
	case SDLK_WORLD_76: return "World 76";
	case SDLK_WORLD_77: return "World 77";
	case SDLK_WORLD_78: return "World 78";
	case SDLK_WORLD_79: return "World 79";
	case SDLK_WORLD_80: return "World 80";
	case SDLK_WORLD_81: return "World 81";
	case SDLK_WORLD_82: return "World 82";
	case SDLK_WORLD_83: return "World 83";
	case SDLK_WORLD_84: return "World 84";
	case SDLK_WORLD_85: return "World 85";
	case SDLK_WORLD_86: return "World 86";
	case SDLK_WORLD_87: return "World 87";
	case SDLK_WORLD_88: return "World 88";
	case SDLK_WORLD_89: return "World 89";
	case SDLK_WORLD_90: return "World 90";
	case SDLK_WORLD_91: return "World 91";
	case SDLK_WORLD_92: return "World 92";
	case SDLK_WORLD_93: return "World 93";
	case SDLK_WORLD_94: return "World 94";
	case SDLK_WORLD_95: return "World 95";

	case SDLK_KP0: return "[0]";
	case SDLK_KP1: return "[1]";
	case SDLK_KP2: return "[2]";
	case SDLK_KP3: return "[3]";
	case SDLK_KP4: return "[4]";
	case SDLK_KP5: return "[5]";
	case SDLK_KP6: return "[6]";
	case SDLK_KP7: return "[7]";
	case SDLK_KP8: return "[8]";
	case SDLK_KP9: return "[9]";
	case SDLK_KP_PERIOD: return "[.]";
	case SDLK_KP_DIVIDE: return "[/]";
	case SDLK_KP_MULTIPLY: return "[*]";
	case SDLK_KP_MINUS: return "[-]";
	case SDLK_KP_PLUS: return "[+]";
	case SDLK_KP_ENTER: return "Enter";
	case SDLK_KP_EQUALS: return "Equals";

	case SDLK_UP: return "Up";
	case SDLK_DOWN: return "Down";
	case SDLK_RIGHT: return "Right";
	case SDLK_LEFT: return "Left";
	case SDLK_INSERT: return "Insert";
	case SDLK_HOME: return "Home";
	case SDLK_END: return "End";
	case SDLK_PAGEUP: return "Page Up";
	case SDLK_PAGEDOWN: return "Page Down";

	case SDLK_F1: return "F1";
	case SDLK_F2: return "F2";
	case SDLK_F3: return "F3";
	case SDLK_F4: return "F4";
	case SDLK_F5: return "F5";
	case SDLK_F6: return "F6";
	case SDLK_F7: return "F7";
	case SDLK_F8: return "F8";
	case SDLK_F9: return "F9";
	case SDLK_F10: return "F10";
	case SDLK_F11: return "F11";
	case SDLK_F12: return "F12";
	case SDLK_F13: return "F13";
	case SDLK_F14: return "F14";
	case SDLK_F15: return "F15";

	case SDLK_NUMLOCK: return "Num Lock";
	case SDLK_CAPSLOCK: return "Caps Lock";
	case SDLK_SCROLLOCK: return "Scroll Lock";
	case SDLK_RSHIFT: return "Right Shift";
	case SDLK_LSHIFT: return "Left Shift";
	case SDLK_RCTRL: return "Right Ctrl";
	case SDLK_LCTRL: return "Left Ctrl";
	case SDLK_RALT: return "Right Alt";
	case SDLK_LALT: return "Left Alt";
	case SDLK_RMETA: return "Right Meta";
	case SDLK_LMETA: return "Left Meta";
	case SDLK_LSUPER: return "Left Super";	/* "Windows" keys */
	case SDLK_RSUPER: return "Right Super";	
	case SDLK_MODE: return "Alt GR";
	case SDLK_COMPOSE: return "Compose";

	case SDLK_HELP: return "Help";
	case SDLK_PRINT: return "Print Screen";
	case SDLK_SYSREQ: return "Sys Req";
	case SDLK_BREAK: return "Break";
	case SDLK_MENU: return "Menu";
	case SDLK_POWER: return "Power";
	case SDLK_EURO: return "Euro";
	case SDLK_UNDO: return "Undo";
	default: return "Unknown";
	}
}

