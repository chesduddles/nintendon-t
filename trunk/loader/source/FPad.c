/*

Nintendont (Loader) - Playing Gamecubes in Wii mode on a Wii U

Copyright (C) 2013  crediar

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation version 2.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/
#include "FPad.h"

static u32 WPAD_Pressed;
static u32 PAD_Pressed;
static s8  PAD_Stick_Y;
static s8  PAD_Stick_X;
static bool SLock;
static u32 SpeedX;
static u32 Repeat;
#define DELAY_START	900
#define DELAY_STEP	100
#define DELAY_STOP	100

#define LEFT_STICK (status->exp.classic.ljs.mag > 0.15)
#define LEFT_STICK_UP ((status->exp.classic.ljs.ang >= 300 && status->exp.classic.ljs.ang <= 360) \
		|| (status->exp.classic.ljs.ang >= 0 && status->exp.classic.ljs.ang <= 60))
#define LEFT_STICK_RIGHT (status->exp.classic.ljs.ang >= 30 && status->exp.classic.ljs.ang <= 150)
#define LEFT_STICK_DOWN (status->exp.classic.ljs.ang >= 120 && status->exp.classic.ljs.ang <= 240)
#define LEFT_STICK_LEFT (status->exp.classic.ljs.ang >= 210 && status->exp.classic.ljs.ang <= 330)

void FPAD_Init( void )
{
	PAD_Init();
	WPAD_Init();

	WPAD_Pressed = 0;
	PAD_Pressed = 0;
	PAD_Stick_Y = 0;
	PAD_Stick_X = 0;
	Repeat = 0;
}
void FPAD_Update( void )
{
	u8 i;

	WPAD_Pressed = 0;
	PAD_Pressed = 0;
	PAD_Stick_Y = 0;
	PAD_Stick_X = 0;

	WPAD_ScanPads();
	for(i = 0; i < WPAD_MAX_WIIMOTES; ++i)
	{
		WPADData *status = WPAD_Data(i);
		if(status->err == WPAD_ERR_NONE)
		{
			WPAD_Pressed |= status->btns_d | status->btns_h;
			if(status->exp.type == WPAD_EXP_CLASSIC && LEFT_STICK)
			{
				PAD_Stick_Y |= LEFT_STICK_UP ? 31 : (LEFT_STICK_DOWN ? -31 : 0);
				PAD_Stick_X |= LEFT_STICK_RIGHT ? 31 : (LEFT_STICK_LEFT ? -31 : 0);
			}
		}
	}

	if(!IsWiiU())
	{
		PAD_ScanPads();
		for(i = 0; i < PAD_CHANMAX; ++i)
		{
			PAD_Pressed |= PAD_ButtonsDown(i) | PAD_ButtonsHeld(i);
			PAD_Stick_Y |= PAD_StickY(i);
			PAD_Stick_X |= PAD_StickX(i);
		}
	}
	if( WPAD_Pressed == 0 && PAD_Pressed == 0 && ( PAD_Stick_Y < 25 && PAD_Stick_Y > -25 )  && ( PAD_Stick_X < 25 && PAD_Stick_X > -25 ) )
	{
		Repeat = 0;
		SLock = false;
		SpeedX= DELAY_START;
	}
	if(SLock == true)
	{
		if(Repeat > 0)
		{
			Repeat--;
			if(Repeat == 0)
				SLock = false;
		}
	}
}
bool FPAD_Up( bool ILock )
{
	if( !ILock && SLock ) return false;

	if((WPAD_Pressed & (WPAD_BUTTON_UP|WPAD_CLASSIC_BUTTON_UP)) || (PAD_Pressed & PAD_BUTTON_UP) || (PAD_Stick_Y > 30))
	{
		Repeat = 10;
		SLock = true;
		return true;
	}
	return false;
}

bool FPAD_Down( bool ILock )
{
	if( !ILock && SLock ) return false;

	if( (WPAD_Pressed & (WPAD_BUTTON_DOWN|WPAD_CLASSIC_BUTTON_DOWN)) || (PAD_Pressed & PAD_BUTTON_DOWN) || (PAD_Stick_Y < -30))
	{
		Repeat = 10;
		SLock = true;
		return true;
	}
	return false;
}

bool FPAD_Left( bool ILock )
{
	if( !ILock && SLock ) return false;

	if((WPAD_Pressed & (WPAD_BUTTON_LEFT|WPAD_CLASSIC_BUTTON_LEFT)) || (PAD_Pressed & PAD_BUTTON_LEFT) || (PAD_Stick_X < -30))
	{
		Repeat = 5;
		SLock = true;
		return true;
	}
	return false;
}
bool FPAD_Right( bool ILock )
{
	if( !ILock && SLock ) return false;

	if( (WPAD_Pressed & (WPAD_BUTTON_RIGHT|WPAD_CLASSIC_BUTTON_RIGHT)) || (PAD_Pressed & PAD_BUTTON_RIGHT) || ( PAD_Stick_X > 30 ))
	{
		Repeat = 5;
		SLock = true;
		return true;
	}
	return false;
}
bool FPAD_OK( bool ILock )
{
	if( !ILock && SLock ) return false;

	if( (WPAD_Pressed & (WPAD_BUTTON_A|WPAD_CLASSIC_BUTTON_A)) || ( PAD_Pressed & PAD_BUTTON_A ) )
	{
		Repeat = 0;
		SLock = true;
		return true;
	}
	return false;
}

bool FPAD_X( bool ILock )
{
	if( !ILock && SLock ) return false;

	if( (WPAD_Pressed & (WPAD_BUTTON_1|WPAD_CLASSIC_BUTTON_X)) || ( PAD_Pressed & PAD_BUTTON_X ))
	{
		Repeat = 0;
		SLock = true;
		return true;
	}
	return false;
}

bool FPAD_Cancel( bool ILock )
{
	if( !ILock && SLock ) return false;

	if( (WPAD_Pressed & (WPAD_BUTTON_B|WPAD_CLASSIC_BUTTON_B)) || ( PAD_Pressed & PAD_BUTTON_B ))
	{
		Repeat = 0;
		SLock = true;
		return true;
	}
	return false;
}

bool FPAD_Start( bool ILock )
{
	if( !ILock && SLock ) return false;

	if( WPAD_Pressed & (WPAD_BUTTON_HOME|WPAD_CLASSIC_BUTTON_HOME) || ( PAD_Pressed & PAD_BUTTON_START ))
	{
		Repeat = 0;
		SLock = true;
		return true;
	}
	return false;
}