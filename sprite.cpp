/*******************************************************************************
*
*   Title:		SDL Sprite Class
*   Author:	    Rob Loach (http://robloach.net)
*   Date:		November 18th, 2004
*
*   Description:
*				A simple sprite SDL class.
*
*   Usage:
*               CSprite sprite("sprite.bmp", SDLWrapper.screen);
*               sprite.x(40);
*               sprite.Paint();
*
*******************************************************************************/


/***************************************************************************
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
***************************************************************************/
 
// Includes
#include "sprite.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

// LoadBMP
// Loads a bitmap image into the surface
bool CSprite::LoadBMP(char cFilename[]){
    if((pImage = SDL_LoadBMP(cFilename)) == NULL)
        return false;
    return true;
}

// Paint
// Paints the surface onto the destination
bool CSprite::Paint(){
    SDL_Rect dest;
    dest.x = m_x;
    dest.y = m_y;
    SDL_BlitSurface(pImage, NULL, m_dest, &dest);
    return true;
}   
bool CSprite::Paint(int x, int y){
    SDL_Rect dest;
    dest.x = x;
    dest.y = y;
    SDL_BlitSurface(pImage, NULL, m_dest, &dest);
    return true;
}   
bool CSprite::Paint(int x, int y, int srcX, int srcY, int width, int height){
    SDL_Rect DestR;
    SDL_Rect SrcR;
    
	DestR.x = x;
    DestR.y = y;
    
	SrcR.x = srcX;
	SrcR.y = srcY;
	SrcR.w = width;
	SrcR.h = height;

	//Draw to the Dest
	SDL_BlitSurface(pImage, &SrcR, m_dest, &DestR);
    return true;
}   
