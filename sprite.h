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


/*******************************************************************************
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*******************************************************************************/
 
// Includes
#ifndef _SPRITE_H_INCLUDED_
#define _SPRITE_H_INCLUDED_

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

// CSprite
// The sprite class
class CSprite{
	public:
		SDL_Surface *pImage;  // The sprite's surface
		
		// Sprite Base
		CSprite::CSprite(char cFilename[], SDL_Surface *destination){
		    m_x=0;
		    m_y=0;
			Init(cFilename, destination);
 		}
		CSprite::CSprite(){
		    m_x=0;
		    m_y=0;
  		}
		~CSprite(){
			SDL_FreeSurface(pImage);
		}
		void Init(char cFilename[], SDL_Surface *destination){
		    LoadBMP(cFilename);
		    m_dest = destination;
		}
		void dest(SDL_Surface *dest){ m_dest = dest; }
		SDL_Surface *dest(){ return m_dest; }
		bool LoadBMP(char cFilename[]);
		
		// Positioning
		int x(){ return m_x; }
		int y(){ return m_y; }
		int x(int x){ return m_x = x; }
		int y(int y){ return m_y = y; }
		int width(){ return pImage->w; }
		int height(){ return pImage->h; }
		
		// Paint
		bool Paint();
		bool Paint(int x, int y);
		bool Paint(int x, int y, int srcX, int srcY);
		bool Paint(int x, int y, int srcX, int srcY, int width, int height);
		
		// Transparency
		void TransColour(int r, int g, int b){
		    SDL_SetColorKey(pImage, SDL_SRCCOLORKEY, 
                SDL_MapRGB(pImage->format, r, g, b));
		}    
		void TransColour(){
		    SDL_SetColorKey(pImage, 0, SDL_MapRGB(pImage->format, 0, 0, 0));
		}

	private:
		int m_x; // X Coordinates on the destination
		int m_y; // Y coordinates on the destination
		
		SDL_Surface *m_dest;
};

#endif // _SPRITE_H_INCLUDED_
