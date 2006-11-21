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
#ifndef _SDLWRAPPER_H_INCLUDED_
#define _SDLWRAPPER_H_INCLUDED_

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <ctime>

// CSDLWrapper
// The main SDL class wrapper
class CSDLWrapper{
public:
    // Initialization
    CSDLWrapper::CSDLWrapper(int screenWidth, int screenHeight, bool fullScreen, char windowTitle[]);
    CSDLWrapper::CSDLWrapper(int screenWidth, int screenHeight, bool fullScreen);
    CSDLWrapper::CSDLWrapper(int screenWidth, int screenHeight);
    CSDLWrapper::CSDLWrapper();
    bool Initiate(int screenWidth, int screenHeight, bool fullScreen, char windowTitle[]);
    
    SDL_Surface *screen;    // The buffer screen output
    Uint8 *GetKeys(){ return SDL_GetKeyState(NULL); } // Returns the array of pressed keys
    
    bool StillGoing();
    void StillGoing(bool going){ m_going = going; }
    bool IgnoreESC(bool ignore){ return m_ignoreESC = ignore; } // Makes it so that ESC doesn't auto close
    bool IgnoreESC() { return m_ignoreESC; }
    
    // Mouse
    // ShowMouse, MouseX, MouseY
    bool ShowMouse(bool Show){
        if(Show)
            SDL_ShowCursor(SDL_ENABLE);
        else
            SDL_ShowCursor(SDL_DISABLE);
        return m_ShowMouse = Show;
    }    
    bool ShowMouse(){ return m_ShowMouse; }
    int MouseX(){ return m_mousex; }
    int MouseY(){ return m_mousey; }
    
    // Pixel Drawing Functions
    // DrawLine, PutPixel
    void DrawLine(int x1, int y1, int x2, int y2, Uint32 color);
    void PutPixel(int x, int y, Uint32 pixel);
    
    // Colour Functions
    // Make Colour
    Uint32 MakeColour(int r, int g, int b);

    // Tick Counter
    int TickCount(){ return m_TickCount; }
    int TickCount(int Count){ return m_TickCount = Count; }
    
    int screenWidth(){ return m_screenWidth; }
    int screenHeight() { return m_screenHeight; }

	char* GetKeyName(int key);
    
private:
    int m_screenWidth;
    int m_screenHeight;
    bool m_going;
    
    bool m_ShowMouse;
    int m_mousex;
    int m_mousey;
    
    int m_Frames;
    int m_Ticks;
    int m_TickCount;
    
    bool m_ignoreESC;
};

#endif // _SDLWRAPPER_H_INCLUDED_
