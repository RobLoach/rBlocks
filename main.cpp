/*******************************************************************************
*
*   Title:		RBlocks
*   Author:	    Rob Loach (http://robloach.net)
*   Date:		December 17, 2004
*
*   Description:
*				A simple remake of the all time classic Tetris.
*
*   Required Libraries:
*               SDL 1.2.7
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

 
//******************************************************************************
// Includes
#include <fstream>	// File Stream
#include <string>	// Strings
#include <SDL/SDL.h>// SDL Libraries

#include "sdlwrapper.h"	// SDL Wrapper
#include "sprite.h"		// Sprite Wrapper
#include "rand.h"		// RandomNumber()
#include "SFont.h"		// Text writing

//******************************************************************************
// Variables
const int SCREENWIDTH = 357;
const int SCREENHEIGHT = 462;
const bool FULLSCREEN = false;
const int TILESIZE = 21;
const int BOARDLEFT = 5;
const int BOARDTOP = 5;
const int NEXTPIECETOP=3;
const int NEXTPIECELEFT=12;

// Game States
const int QUITGAME = -1;
const int MAINMENU = 0;
const int STARTNEWGAME = 1;
const int INGAME = 2;
const int CONTROLS = 3;
const int ABOUT = 4;

// Menu Items
const int TITLETOP = 80;
const int TITLELEFT = 35;

// Score
const int SCOREPERMOVEDOWN = 1;
const int SCOREPERBLOCK = 10;
const int SCOREPERLINE = 100;
const int SCORECOMBO2 = 100;
const int SCORECOMBO3 = 200;
const int SCORECOMBO4 = 400;

const int SCOREPERLEVEL = 1000;

const int BLOCKMOVEDOWNSTARTTIME = 200;

int ControlRotate = SDLK_UP;
int ControlLeft = SDLK_LEFT;
int ControlRight = SDLK_RIGHT;
int ControlDown = SDLK_DOWN;
int ControlDrop = SDLK_SPACE;
int ControlPause = SDLK_RETURN;

int gameState = MAINMENU;
int blockMoveStartTime;		// The time when the block was dropped

int score;
int level;
int linesCleared;

bool pause;

// Graphics
CSDLWrapper SDLWrapper;

// Sprites
CSprite blocks;
CSprite background;
CSprite sTitle;

SFont_Font* Font;
SFont_Font* FontSel;

// The piece information
struct sPiece{
	int type;
	int x;
	int y;
	bool tile[4][4];
};
sPiece curPiece, nextPiece;
int GameBoard[10][20];


//******************************************************************************
// Function Prototypes
void InitGame();
void NewGame();
void DrawTile(int x, int y, int type);
void DrawBoard();
void DrawGameMap();
void DrawPiece(sPiece piece);
int Events();
void MakeNewPiece();
void RotateBlock(sPiece *piece, bool CheckCollision);
bool CollisionTest(int x, int y);
bool Move(int x, int y);
int MainMenu();
void DrawBackground();
void DrawScore();
void NewLevel();
int GetLevelSpeed();
void WriteFile();
std::string Xor(std::string buffer, std::string key);
void DrawFontInCenter(const char *text, const SFont_Font *Font, int y);


//******************************************************************************
// Functions


// String Encryption
std::string Xor(std::string buffer, std::string key)
{
	int keylength = key.length();;
	int bufferlength = buffer.length();
	int bufferindex = 0;
	int keyindex = 0;
	std::string output;
	char buffer_c;
	char key_c;
	while (bufferindex < bufferlength){
		if (keyindex >= keylength)
			keyindex=0;
		buffer_c = buffer[bufferindex];
		key_c = key[keyindex];
		output += buffer_c ^ key_c;
		keyindex++;
		bufferindex++;
	}
	return output;
}

//******************************************************************************
// Game State Functions

// Init Game - Initializes the program
void InitGame(){
	// Initiate the program
    SDLWrapper.Initiate(SCREENWIDTH, SCREENHEIGHT, FULLSCREEN, "rBlocks");
    
    //SDLWrapper.ShowMouse(false);
	SDLWrapper.IgnoreESC(true);

	InitRandomNumber();

	// Load graphics
    blocks.Init("data/blocks.bmp", SDLWrapper.screen);
	blocks.TransColour(1,1,1);
	background.Init("data/background.bmp", SDLWrapper.screen);
    sTitle.Init("data/title.bmp", SDLWrapper.screen);
	sTitle.TransColour(1,1,1);

	Font = SFont_InitFont(SDL_LoadBMP("data/font.bmp"));
	if(!Font) {
		fprintf(stderr, "An error occured while loading the font.");
		exit(1);
	}
	FontSel = SFont_InitFont(SDL_LoadBMP("data/fontsel.bmp"));
	if(!FontSel) {
		fprintf(stderr, "An error occured while loading the selection font.");
		exit(1);
	}

}

// New Game - Starts a new game
void NewGame(){
	// Starts a new game
	MakeNewPiece();
	InitRandomNumber();
	MakeNewPiece();
	score = 0;
	level=0;
	linesCleared = 0;
	pause=false;
	// Clear board
	for(int x=0; x< 10; x++){
		for(int y=0; y < 20; y++){
			GameBoard[x][y] = 0;
		}
	}

    NewLevel();
}

// Events - The InGame Events
int Events(){
	SDL_Event Event;
	while (SDL_PollEvent(&Event)) {
		if(Event.type == SDL_KEYDOWN){
			int Sym = Event.key.keysym.sym;
			if(Sym == SDLK_ESCAPE){
				if(pause){
					// Go back to main menu
					gameState = MAINMENU;
					WriteFile();
				} else {
					pause = !pause;
				}
			} else if(Sym == ControlDown){
				// Move down slowly
				if(!pause){
					Move(0,1);
					score+=SCOREPERMOVEDOWN;
				}
			} else if(Sym == ControlRotate){
				// Rotate block
				if(!pause)
					RotateBlock(&curPiece,true);
			} else if(Sym == ControlLeft){
				// Move block left
				if(!pause)
					Move(-1,0);
			} else if(Sym == ControlRight){
				// Move block right
				if(!pause)
					Move(1,0);
			} else if(Sym == ControlPause){
				pause = !pause;
			} else if(Sym == ControlDrop){
				if(!pause){
					while(!Move(0,1)){
						score+=SCOREPERMOVEDOWN;
					}
				}
			}
		}else{
			if(Event.type == SDL_QUIT)
                gameState = QUITGAME;
		}
	}
}

// About - The About Screen
int About(){
	SDL_Event Event;
	while (SDL_PollEvent(&Event)) {
		if(Event.type == SDL_KEYDOWN){
			int Sym = Event.key.keysym.sym;
			if((Sym != SDLK_UP) && (Sym != SDLK_DOWN))
				return MAINMENU;
		}
	}

	static int creditstop = SCREENHEIGHT-TILESIZE;
	static int TextHeight = SFont_TextHeight(Font);

	DrawBackground();
	sTitle.Paint(TITLELEFT,TITLETOP);

	DrawFontInCenter("Created By", Font, creditstop);
	DrawFontInCenter("Rob Loach", FontSel, creditstop+TextHeight);
	DrawFontInCenter("Thank You", Font, creditstop+TextHeight*4);
	DrawFontInCenter("coldacid", FontSel, creditstop+TextHeight*5);
	DrawFontInCenter("IFooBar", FontSel, creditstop+TextHeight*6);
	DrawFontInCenter("Omid", FontSel, creditstop+TextHeight*7);
	DrawFontInCenter("GameDev", FontSel, creditstop+TextHeight*8);
	DrawFontInCenter("robloach.net", Font, creditstop+TextHeight*12);

	Uint8* keys = SDLWrapper.GetKeys();
	if(keys[SDLK_DOWN])
		creditstop+=2;
	else if(keys[SDLK_UP])
		creditstop-=2;
	else
		creditstop--;
	if(creditstop<-TextHeight*13)
		creditstop = SCREENHEIGHT-TILESIZE;
	else if(creditstop > SCREENHEIGHT-TILESIZE)
		creditstop = -TextHeight*13;

    DrawBoard();
	DrawScore();

	return ABOUT;
}

// Main Menu
int MainMenu(){
    
	SDL_Event Event;
	static int selecteditem = 0;
	while (SDL_PollEvent(&Event)) {
		if(Event.type == SDL_KEYDOWN){

			if(Event.key.keysym.sym == SDLK_DOWN)
				selecteditem++;
			if(Event.key.keysym.sym == SDLK_UP)
				selecteditem--;

			int Sym = Event.key.keysym.sym;
			switch(Sym) {
			case SDLK_ESCAPE:
				return QUITGAME;
				break;
			case SDLK_RETURN:
				switch(selecteditem){
				case 0: return STARTNEWGAME;
				case 1: return CONTROLS;
				case 2: return ABOUT;
				case 3: return QUITGAME;
				}
			}
		}else{
			if(Event.type == SDL_QUIT)
                return QUITGAME;
		}
	}
	if(selecteditem<0)
		selecteditem=3;
	if(selecteditem>3)
		selecteditem=0;
    DrawBoard();
	DrawBackground();
	DrawScore();
	sTitle.Paint(TITLELEFT,TITLETOP);

	const int NewGameTop = 200;
	const int MenuItemLine = 40;

	switch(selecteditem){
	case 0:
		DrawFontInCenter("New Game", FontSel, NewGameTop);
		DrawFontInCenter("Controls", Font,NewGameTop+MenuItemLine);
		DrawFontInCenter("About", Font,NewGameTop+MenuItemLine*2);
		DrawFontInCenter("Exit", Font, NewGameTop+MenuItemLine*3);
		break;
	case 1:
		DrawFontInCenter("New Game", Font,NewGameTop);
		DrawFontInCenter("Controls", FontSel,NewGameTop+MenuItemLine);
		DrawFontInCenter("About", Font,NewGameTop+MenuItemLine*2);
		DrawFontInCenter("Exit", Font, NewGameTop+MenuItemLine*3);
		break;
	case 2:
		DrawFontInCenter("New Game", Font, NewGameTop);
		DrawFontInCenter("Controls", Font,NewGameTop+MenuItemLine);
		DrawFontInCenter("About", FontSel,NewGameTop+MenuItemLine*2);
		DrawFontInCenter("Exit", Font, NewGameTop+MenuItemLine*3);
		break;
	case 3:
		DrawFontInCenter("New Game", Font, NewGameTop);
		DrawFontInCenter("Controls", Font,NewGameTop+MenuItemLine);
		DrawFontInCenter("About", Font,NewGameTop+MenuItemLine*2);
		DrawFontInCenter("Exit", FontSel, NewGameTop+MenuItemLine*3);
		break;
	}
	return MAINMENU;
}

// Controls - The Controls Screen
int Controls(){
	SDL_Event Event;
	static int selecteditem = 0;
	static int waitkey = 0;

    DrawBoard();
	DrawBackground();
	DrawScore();
	sTitle.Paint(TITLELEFT,TITLETOP);

	if(waitkey<=0){
		while (SDL_PollEvent(&Event)) {
			if(Event.type == SDL_KEYDOWN){
	
				int Sym = Event.key.keysym.sym;
				if(Event.key.keysym.sym == SDLK_DOWN)
					selecteditem++;
				if(Event.key.keysym.sym == SDLK_UP)
					selecteditem--;
	
				switch(Sym) {
				case SDLK_ESCAPE:
					return MAINMENU;
					break;
				case SDLK_RETURN:
					if(selecteditem >= 6)
						return MAINMENU;
					else
						waitkey = selecteditem+1;
				}
			}else{
				if(Event.type == SDL_QUIT)
	                return QUITGAME;
			}
		}
		if(selecteditem<0)
			selecteditem=6;
		if(selecteditem>6)
			selecteditem=0;
	
		const int MenuTop = 180;
		const int MenuItemLine = 30;
	
		switch(selecteditem){
		case 0:
			DrawFontInCenter("Left", FontSel, MenuTop);
			DrawFontInCenter("Right", Font,MenuTop+MenuItemLine);
			DrawFontInCenter("Down", Font,MenuTop+MenuItemLine*2);
			DrawFontInCenter("Rotate", Font, MenuTop+MenuItemLine*3);
			DrawFontInCenter("Drop", Font, MenuTop+MenuItemLine*4);
			DrawFontInCenter("Pause", Font, MenuTop+MenuItemLine*5);
			DrawFontInCenter("Back", Font, MenuTop+MenuItemLine*7);
			break;
		case 1:
			DrawFontInCenter("Left", Font, MenuTop);
			DrawFontInCenter("Right", FontSel,MenuTop+MenuItemLine);
			DrawFontInCenter("Down", Font,MenuTop+MenuItemLine*2);
			DrawFontInCenter("Rotate", Font, MenuTop+MenuItemLine*3);
			DrawFontInCenter("Drop", Font, MenuTop+MenuItemLine*4);
			DrawFontInCenter("Pause", Font, MenuTop+MenuItemLine*5);
			DrawFontInCenter("Back", Font, MenuTop+MenuItemLine*7);
			break;
		case 2:
			DrawFontInCenter("Left", Font, MenuTop);
			DrawFontInCenter("Right", Font,MenuTop+MenuItemLine);
			DrawFontInCenter("Down", FontSel,MenuTop+MenuItemLine*2);
			DrawFontInCenter("Rotate", Font, MenuTop+MenuItemLine*3);
			DrawFontInCenter("Drop", Font, MenuTop+MenuItemLine*4);
			DrawFontInCenter("Pause", Font, MenuTop+MenuItemLine*5);
			DrawFontInCenter("Back", Font, MenuTop+MenuItemLine*7);
			break;
		case 3:
			DrawFontInCenter("Left", Font, MenuTop);
			DrawFontInCenter("Right", Font,MenuTop+MenuItemLine);
			DrawFontInCenter("Down", Font,MenuTop+MenuItemLine*2);
			DrawFontInCenter("Rotate", FontSel, MenuTop+MenuItemLine*3);
			DrawFontInCenter("Drop", Font, MenuTop+MenuItemLine*4);
			DrawFontInCenter("Pause", Font, MenuTop+MenuItemLine*5);
			DrawFontInCenter("Back", Font, MenuTop+MenuItemLine*7);
			break;
		case 4:
			DrawFontInCenter("Left", Font, MenuTop);
			DrawFontInCenter("Right", Font,MenuTop+MenuItemLine);
			DrawFontInCenter("Down", Font,MenuTop+MenuItemLine*2);
			DrawFontInCenter("Rotate", Font, MenuTop+MenuItemLine*3);
			DrawFontInCenter("Drop", FontSel, MenuTop+MenuItemLine*4);
			DrawFontInCenter("Pause", Font, MenuTop+MenuItemLine*5);
			DrawFontInCenter("Back", Font, MenuTop+MenuItemLine*7);
			break;
		case 5:
			DrawFontInCenter("Left", Font, MenuTop);
			DrawFontInCenter("Right", Font,MenuTop+MenuItemLine);
			DrawFontInCenter("Down", Font,MenuTop+MenuItemLine*2);
			DrawFontInCenter("Rotate", Font, MenuTop+MenuItemLine*3);
			DrawFontInCenter("Drop", Font, MenuTop+MenuItemLine*4);
			DrawFontInCenter("Pause", FontSel, MenuTop+MenuItemLine*5);
			DrawFontInCenter("Back", Font, MenuTop+MenuItemLine*7);
			break;
		case 6:
			DrawFontInCenter("Left", Font, MenuTop);
			DrawFontInCenter("Right", Font,MenuTop+MenuItemLine);
			DrawFontInCenter("Down", Font,MenuTop+MenuItemLine*2);
			DrawFontInCenter("Rotate", Font, MenuTop+MenuItemLine*3);
			DrawFontInCenter("Drop", Font, MenuTop+MenuItemLine*4);
			DrawFontInCenter("Pause", Font, MenuTop+MenuItemLine*5);
			DrawFontInCenter("Back", FontSel, MenuTop+MenuItemLine*7);
			break;
		}
	} else {
		DrawFontInCenter("Enter Key", Font, 270);

		switch(waitkey){
		case 1: DrawFontInCenter("For Left", Font, 300); break;
		case 2: DrawFontInCenter("For Right", Font, 300); break;
		case 3: DrawFontInCenter("For Down", Font, 300); break;
		case 4: DrawFontInCenter("To Rotate", Font, 300); break;
		case 5: DrawFontInCenter("To Drop", Font, 300); break;
		case 6: DrawFontInCenter("To Pause", Font, 300); break;
		}

		while (SDL_PollEvent(&Event)) {
			if(Event.type == SDL_KEYDOWN){
				int Sym = Event.key.keysym.sym;
				if(Sym == SDLK_ESCAPE){
					waitkey=0;
				} else {
					switch(waitkey){
					case 1: ControlLeft = Sym; break;
					case 2: ControlRight = Sym; break;
					case 3: ControlDown = Sym; break;
					case 4: ControlRotate = Sym; break;
					case 5: ControlDrop = Sym; break;
					case 6: ControlPause = Sym; break;
					}
					waitkey = 0;
				}
			}else{
				if(Event.type == SDL_QUIT)
	                return QUITGAME;
			}
		}
	}
	return CONTROLS;
}



//******************************************************************************
// Drawing Functions

// DrawFontInCenter - Draws the given text in the middle of the screen
void DrawFontInCenter(const char *text, const SFont_Font *Font, int y){
	if(y>=0)
		SFont_Write(SDLWrapper.screen, Font,
			21 + background.width() / 2 - SFont_TextWidth(Font, text) / 2,
			y, text);
}

// DrawScore - Displays the score on the screen
void DrawScore(){
	int ScoreTop = (NEXTPIECETOP+6)*TILESIZE+10;
	// Score
	SFont_Write(SDLWrapper.screen, Font, TILESIZE*13-18,ScoreTop,"Score");
	std::string scorestr = std::to_string(score);
	SFont_Write(SDLWrapper.screen, Font, SDLWrapper.screenWidth()-SFont_TextWidth(Font, scorestr.c_str()) - 5,ScoreTop+31, scorestr.c_str());

	// Level
	SFont_Write(SDLWrapper.screen, Font, TILESIZE*13-12,ScoreTop+93,"Level");
	std::string levelstr = std::to_string(level);
    SFont_Write(SDLWrapper.screen, Font, SDLWrapper.screenWidth()-SFont_TextWidth(Font, levelstr.c_str()) - 5,ScoreTop+124, levelstr.c_str());

	// Lines
	SFont_Write(SDLWrapper.screen, Font, TILESIZE*13-12,ScoreTop+186,"Lines");
	std::string linesClearedstr = std::to_string(linesCleared);
    SFont_Write(SDLWrapper.screen, Font, SDLWrapper.screenWidth()-SFont_TextWidth(Font, linesClearedstr.c_str()) - 5,ScoreTop+217, linesClearedstr.c_str());

}

// DrawPiece - Displays the given piece on the screen
void DrawPiece(sPiece piece){
	for(int x=0; x<4; x++){
		for(int y=0; y<4; y++){
			if(piece.tile[x][y]){
				DrawTile(piece.x+x + 1, piece.y+y + 1, piece.type);
			}
		}
	}
}

// DrawTile - Draws the given tile type at the given x y location
void DrawTile(int x, int y, int type){
	blocks.Paint(x*TILESIZE, y*TILESIZE, type * TILESIZE, 0, TILESIZE, TILESIZE);
}

// DrawBackground - Displays the background on the screen
void DrawBackground(){

	background.Paint(TILESIZE,TILESIZE);
}

void DrawBoard(){
	// Draw the border
	for(int y=0; y < 22; y++){
		DrawTile(0,y,0);
		DrawTile(11,y,0);
	}
	for(int x=0; x < 12; x++){
		DrawTile(x,0,0);
		DrawTile(x,21,0);
	}

	// Draw the Preview Screen
	for(int x=12; x < 17; x++){
		DrawTile(x,NEXTPIECETOP,0);
		DrawTile(x,NEXTPIECETOP+5,0);
	}
	for(int y=NEXTPIECETOP+1; y < NEXTPIECETOP + 5; y++){
		DrawTile(16,y,0);
	}
	
}

// Draw Game Map - Draws the gameboard on the screen
void DrawGameMap(){
	for(int x=0; x<10; x++){
		for(int y = 0; y<20; y++){
			if(GameBoard[x][y] != 0)
				DrawTile(x+1,y+1,GameBoard[x][y]);
		}
	}
}



//******************************************************************************
// Level Data and Score

// Get Level Speed - Returns the interval between block moves for each level
int GetLevelSpeed(){
	switch(level){
	case 0:
	case 1: return 800;
	case 2: return 700;
	case 3: return 600;
	case 4: return 550;
	case 5: return 500;
	case 6: return 450;
	case 7: return 400;
	case 8: return 350;
	case 9: return 300;
	case 10: return 275;
	case 11: return 250;
	case 12: return 225;
	case 13: return 200;
	case 14: return 175;
	case 15: return 150;
	case 16: return 125;
	case 17: return 100;
	case 18: return 90;
	case 19: return 85;
	case 20: return 80;
	case 21: return 75;
	case 22: return 70;
	case 23: return 66;
	case 24: return 62;
	case 25: return 58;
	case 26: return 54;
	default: return 50;
	}
}

// New Level - Goes to the next level
void NewLevel(){
    level++;
}

// WriteFile - Saves the highscore in score.dat
void WriteFile(){
	std::ofstream fileOutput("score.dat");
    if(!fileOutput){
        fprintf(stderr, "An error occurred while trying to open score.dat\n");
        return;
    }
	std::string text = "rBlocks-";
	text.append(std::to_string(score));
	text.append("-");
	text.append(std::to_string(level));
	text.append("-");
	text.append(std::to_string(linesCleared));
	std::string key = "key";
	std::string result = Xor(text, key);
	fileOutput << result;
	fileOutput.close();
}


//******************************************************************************
// GamePlay

// Creates a new piece and switches the next piece to the current one.
void MakeNewPiece(){
	curPiece = nextPiece;
	curPiece.x = 3;
	curPiece.y = -1;
    blockMoveStartTime = SDL_GetTicks();

	for(int x = 0; x < 4; x++)
		for(int y = 0; y < 4; y++)
			nextPiece.tile[x][y] = false;

	nextPiece.x = NEXTPIECELEFT-1;
	nextPiece.y = NEXTPIECETOP;
	int rand = RandomNumber(1,7);
	nextPiece.type = rand;
	switch(rand){
	case 1: // Tower
			nextPiece.tile[0][1]=true;
			nextPiece.tile[1][1]=true;
			nextPiece.tile[2][1]=true;
			nextPiece.tile[3][1]=true;
		break;
	case 2: // Box
			nextPiece.tile[1][1]=true;
			nextPiece.tile[1][2]=true;
			nextPiece.tile[2][1]=true;
			nextPiece.tile[2][2]=true;
		break;
	case 3: // Pyramid
			nextPiece.tile[1][1]=true;
			nextPiece.tile[0][2]=true;
			nextPiece.tile[1][2]=true;
			nextPiece.tile[2][2]=true;
		break;
	case 4: // Left L
			nextPiece.tile[0][1]=true;
			nextPiece.tile[1][1]=true;
			nextPiece.tile[1][2]=true;
			nextPiece.tile[2][2]=true;
		break;
	case 5: // Right L
			nextPiece.tile[2][1]=true;
			nextPiece.tile[1][1]=true;
			nextPiece.tile[1][2]=true;
			nextPiece.tile[0][2]=true;
		break;
	case 6: // Left /
			nextPiece.tile[1][1]=true;
			nextPiece.tile[2][1]=true;
			nextPiece.tile[2][2]=true;
			nextPiece.tile[2][3]=true;
		break;
	case 7:// Right /
			nextPiece.tile[2][1]=true;
			nextPiece.tile[1][1]=true;
			nextPiece.tile[1][2]=true;
			nextPiece.tile[1][3]=true;
		break;
	}

	// Check if there was a collision with the new piece
	if(CollisionTest(0,0)){
		// If there was, go to main menu and save score.dat
		gameState = MAINMENU;
		WriteFile();
	}

	// Go onto the next level if required
	if((int)(score / SCOREPERLEVEL) > level-1)
		NewLevel();
	
	
}

// Rotate Block - Rotates the given block and optionally checks for
void RotateBlock(sPiece* piece, bool CheckCollision){

	int i, j;
	bool temp[4][4];

	// Make a new array of the rotated block
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			temp[3-j][i]=piece->tile[i][j];

	if(CheckCollision){
		// Check if the rotated block hits the borders
		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
				if(temp[ i ][j])
					if(piece->x + i < 0 || piece->x + i > 10 - 1 ||
						/*piece->y + j < 0 ||*/ piece->y + j > 20 - 1)
						return;
	
		// Check if the rotated block hits any other blocks on the board
		for(int x=0; x< 10; x++)
			for(int y=0; y< 20; y++)
				if(x >= piece->x && x < piece->x + 4)
					if(y >= piece->y && y < piece->y +4)
						if(GameBoard[x][y] != 0)
							if(temp[x - piece->x][y - piece->y])
								return;
	}

	// Collision check approved
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			piece->tile[i][j]=temp[i][j];
}

// CollisionTest
// Checks if there is a collision with curPiece and the board on the new x and y
//  coordinates.  nx is move which way horizontally and xy is move which way
//  vertically.
bool CollisionTest(int nx, int ny){

	// Get the new coordinates
    int newx = curPiece.x + nx;
    int newy = curPiece.y + ny;

	// Check if it's off the board
    for(int i = 0; i< 4; i++)
        for(int j = 0; j < 4; j++)
            if(curPiece.tile[i][j])
                if(newx + i < 0 ||
					newx + i > 10 - 1 ||
                    newy + j > 20 - 1)
                    return true;

	// Check if there is a collision with a tile on the board
    for(int x = 0; x < 10; x++)
        for(int y = 0; y < 20; y++)
            if(x >= newx && x < newx + 4)
                if(y >= newy && y < newy +4)
                    if(GameBoard[x][y] != 0)
                        if(curPiece.tile[x - newx][y - newy])
                            return true;
    return false;
}

// Remove Row - Takes off the given row
void RemoveRow(int row){

    int x,y;
    int counter=0;

    for(x=0; x< 20; x++)
        for(y=row; y>0; y--)
            GameBoard[x][y]=GameBoard[x][y-1];

}

// Move - Move the current piece.  x and y and by how much to move.
bool Move(int x, int y){

	// If there is a collision with the new coordinates, make new piece.
    if(CollisionTest(x, y)){
        if(y == 1){

            // Add the current block to the game board
            for(int i = 0; i < 4; i++)
                for(int j = 0; j < 4; j++)
                    if(curPiece.tile[i][j])
                        GameBoard[curPiece.x+i][curPiece.y+j] = curPiece.type;

            // Clear rows if needed
			int NumCleared = 0;
            for(int j = 0; j < 20; j++){
                bool filled = true; // if the row is completely filled
                for(int i = 0; i < 10; i++)
                    if(GameBoard[i][j] == 0)
                        filled = false;

                if(filled){
                    RemoveRow(j);
					score += SCOREPERLINE;
                    NumCleared++;
					linesCleared++;
                }
            }
			switch(NumCleared){
			case 0: score+=SCOREPERBLOCK; break;
			case 1: break;
			case 2: score+=SCORECOMBO2; break;
			case 3: score+=SCORECOMBO3; break;
			case 4: score+=SCORECOMBO4; break;
			}
            MakeNewPiece(); // Make the new piece
			return true; // It moves correctly, return true
        }
    } else {
		if(y == 1) // If block is moving down, update the time
    		blockMoveStartTime = SDL_GetTicks();
		// Move the block
        curPiece.x+=x;
        curPiece.y+=y;
		return false;
    }
}


//******************************************************************************

int main(int argc, char *argv[]){
    // Create the window
	InitGame();
	
	// Manage the key input
	SDL_EnableKeyRepeat(300, 30);

    // While the game is going
    while(SDLWrapper.StillGoing()){
        
        // Start the game states
        switch(gameState){
		case QUITGAME: //quit
			SDLWrapper.StillGoing(false);
			break;
		case CONTROLS: // Controls screen
			gameState = Controls();
			break;
        case ABOUT:		// About screen
        	gameState = About();
        break;
        case MAINMENU:	// Menu screen
        	gameState = MainMenu();
        break;
        case STARTNEWGAME: // Initialize new game
            NewGame();
            // Move onto the in-game state
        	gameState = INGAME;
        break;
        case INGAME: // In Game
        	
			// User input
			Events();
			
			// Draw the screen
			DrawBackground();
			DrawScore();
			if(!pause){
	            if((SDL_GetTicks() - blockMoveStartTime) > GetLevelSpeed()) {
					Move(0,1);
        			blockMoveStartTime = SDL_GetTicks();
        		}
				DrawGameMap();
				DrawPiece(curPiece);
				DrawPiece(nextPiece);
			}else{
				if(level<=5){
					DrawGameMap();
					DrawPiece(curPiece);
					DrawPiece(nextPiece);
				}
				DrawFontInCenter("Pause", Font, TILESIZE*3);
				DrawFontInCenter(SDLWrapper.GetKeyName(ControlPause), Font, 200);
				DrawFontInCenter("To Continue", FontSel, 200+SFont_TextHeight(FontSel));

				DrawFontInCenter(SDLWrapper.GetKeyName(SDLK_ESCAPE), Font, 300);
				DrawFontInCenter("To Quit", FontSel, 300+SFont_TextHeight(FontSel));
			}
			DrawBoard();
            
        break;
        }
    }

	// Clear the font memory
	SFont_FreeFont(Font);
	SFont_FreeFont(FontSel);

    return 0;
}
