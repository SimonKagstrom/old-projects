// "Firepower"-spel, p†b”rjat 96-11-28

#include "stdlib.h"
#include "stdio.h"
#include "conio.h"
#include "alloc.h"
#include "dos.h"
#include "mem.h"

#include "fp_vars.h"
#include "timer.h" // Bios-ticks. B”r accelereras.
#include "mouse.h" // S„ger sig sj„lvt, m†h„nda?
#include "keyboard.h" // Byter tangentbordshanterare. Flera knappar samtidigt.
#include "graphics.h" // Just det!
#include "fp_objs.h" // Objektet Tank och relaterade funktioner.
#include "fp_utils.h" // Inl„sning av kartan, list-funktioner.

/*--------------------------Konstanter------------------------------*/

const  	STARTX				=	SCREENW/2-SPRITEW/2,
			STARTY				= 	SCREENH/2-BACKGRH-SPRITEH/2,
			RADARX[2]			= 	{270,317}, // Gr„nserna f”r radar-rutan
			RADARY[2]         =  {163,196},
			RADAR_RES			=	25; // Radarns uppl”sning, h”gre tal = n†r l„ngre.

/*------------------------------Variabler------------------------------*/

unsigned char *Pvirt_scr = NULL; // Pekare till virtuell sk„rm
unsigned char *Pvirt_background = NULL; // Pekar till bakgrundsbilden
unsigned char *Ppic = NULL; // Pekare till grafiken, b†de bakgrund och sprites.
unsigned char *Pdummy = NULL; // Bara s† att Pvirt_background ska ha n†tt att skriva ”ver...
class Tank *Ptanks_start = NULL;
class Tank *Ptanks = NULL; // R”rlig nodpekare.
class Tank *Pplayer = NULL;
int oldx;
int oldy;

/*-----------------------------Funktioner------------------------------*/

void radar (int x, int y, unsigned char col) // S„tt dit radar-blipp
{
	int xpos = RADARX[0]+(RADARX[1]-RADARX[0])/2+(x-Pplayer->xpos)/RADAR_RES;
	int ypos = RADARY[0]+(RADARY[1]-RADARY[0])/2+(y-Pplayer->ypos)/RADAR_RES;
	if ((xpos < RADARX[1] && xpos > RADARX[0]) && (ypos < RADARY[1] && ypos > RADARY[0])) putpixel (xpos,ypos,col,vga); // Fienderna d„r de ska vara.
	putpixel (RADARX[0]+(RADARX[1]-RADARX[0])/2,RADARY[0]+(RADARY[1]-RADARY[0])/2,2,vga); // Gubben i mitten.
}

void radar (int x, int y, unsigned char col, char) // Sudda radar-blipp
{
	int xpos = RADARX[0]+(RADARX[1]-RADARX[0])/2+(x-oldx)/RADAR_RES;
	int ypos = RADARY[0]+(RADARY[1]-RADARY[0])/2+(y-oldy)/RADAR_RES;
	if ((xpos < RADARX[1] && xpos > RADARX[0]) && (ypos < RADARY[1] && ypos > RADARY[0])) putpixel (xpos,ypos,col,vga);
}

char collision (class Tank *Pin) // L„gg in i n†tt av objekten ist„llet?
{
	if (diff((Pin->xpos+15)-(Pplayer->xpos+15)) < 30 && diff((Pin->ypos+15)-(Pplayer->ypos+15)) < 30)
	{
//			sound (1000);
		return 1;
	}
	return 0;
}

void sight_on_object (class Tank *Pin, int xvalue, int yvalue) // B”r ocks† flyttas, kolla alla objekt osv.
{
	if ((diff (Pin->xpos+15-xvalue) < 15) && (diff (Pin->ypos+15-yvalue) < 15))
	{
		sound (200);
	}
}

void fire() // Flyttas senare!
{
	drawbox (mouse.xpos-SMALLOBJW/2,mouse.ypos-SMALLOBJH/2,SMALLOBJW,SMALLOBJH,Ppic+SMALLOBJ_OFF+SMALLOBJW*SMALLOBJH*12,Pvirt_scr);
}

void game ()
{
	int i,j;
	signed int mod1 = 0;
	signed int mod2 = 0;
	int playerx = STARTX;
	int playery = 100-BACKGRH;
	int xcount = 0;
	int ycount = 0;
	int test = 0;
	int enemydir = 0;
	while (keydown[ESC] == 0) // Hoppa ur p† escape...
	{
		mouse_status();
		Pplayer->Pturret->determine_sector (mouse.xpos-140,mouse.ypos-50,SPRITEW,SPRITEH);
		drawbox (playerx,playery,SPRITEW,SPRITEH,Ppic+(Pplayer->direction*SPRITEW*SPRITEH), Pvirt_scr); // S„tt spelare i mitten av sk„rmen.
		drawbox (playerx+4,playery+5,SMALLOBJW,SMALLOBJH,Ppic+SMALLOBJ_OFF+(Pplayer->Pturret->direction*SMALLOBJW*SMALLOBJH), Pvirt_scr); // S„tt kanontorn p† spelare..
		test = 0;
		for (Ptanks = Ptanks_start; Ptanks->next != NULL; Ptanks = Ptanks->next)
		{
			radar(Ptanks->xpos,Ptanks->ypos,0,0);
			enemydir = random(3) + 75;
			if (collision (Ptanks)) test = 1;
			Ptanks->input(enemydir);
			sight_on_object (Ptanks, mouse.xpos+Pplayer->xpos-140,mouse.ypos+Pplayer->ypos-60); // Funkar inte riktigt...
			if ((diff(Ptanks->xpos-Pplayer->xpos) < (SCREENW/2)+SPRITEW) && (diff(Ptanks->ypos-Pplayer->ypos) < (SCREENH/2)+SPRITEH)) drawbox (Ptanks->xpos-Pplayer->xpos+145,Ptanks->ypos-Pplayer->ypos+60, SPRITEW, SPRITEH, Ppic+(Ptanks->direction*SPRITEW*SPRITEH), Pvirt_scr);
			radar(Ptanks->xpos,Ptanks->ypos,4);
		}
		if (mouse.buttons) fire();
		while (keydown[25]) {clreol();}
		gotoxy (1,23); printf("x:%d y:%d\nxcount:%d ycount:%d\nmousex:%d mousey:%d",Pplayer->xpos, Pplayer->ypos, xcount, ycount,mouse.xpos,mouse.ypos);
		if (keydown[20]) Pplayer->forward_speed = 0; // TA BORT SENARE!!! T = Turbo. Fr buggs only.
		if (keydown[21]) Pplayer->forward_speed = 1; // TA BORT SENARE!!! Y = Normal fart.
		oldx = Pplayer->xpos; oldy = Pplayer->ypos; // Kan s„ttas f”re scrollingen...
		if (!test) Pplayer->input (keydown);
		if (oldx != Pplayer->xpos || oldy != Pplayer->ypos) // Om flyttat - s† scrolla scrolla. Buggar fortfarande ltiegrann.
		{
			mod1 = 0; mod2 = 0;
			if (Pplayer->ypos > oldy) // Ned†t
			{
				mod1 = SCREENW;
				if (Pplayer->xpos > oldx) {mod2 = -1;} // H”ger
				if (Pplayer->xpos < oldx) {mod2 = 1;}  // V„nster
				ycount++;
			}
			if (Pplayer->ypos < oldy) // Upp†t
			{
				mod2 = SCREENW;
				if (Pplayer->xpos > oldx) {mod1 = 1;}  // H”ger
				if (Pplayer->xpos < oldx) {mod1 = -1;} // V„nster
				ycount--;
			}
			if (Pplayer->xpos > oldx) {xcount++;} // H”ger
			if (Pplayer->xpos < oldx) {xcount--;} // V„nster
			// Flytta runt sk„rmbilden:
			if (Pplayer->xpos > oldx && Pplayer->ypos == oldy) {mod1 = 0; mod2 = -1;}
			if (Pplayer->xpos < oldx && Pplayer->ypos == oldy) {mod1 = 0; mod2 = 1;}
			movmem (Pvirt_background+mod1, Pvirt_background+mod2, SCREENW*(SCREENH-BACKGRH));
			// M†ste vara movmem, memcpy klarar inte att flytta till sig sj„lv.
			// Positionen i bakgrunds-bilderna.
			if (xcount < 0) xcount = BACKGRW-1;
			if (xcount > BACKGRW-1) xcount = 0;
			if (ycount < 0) ycount = BACKGRH-1;
			if (ycount > BACKGRH-1) ycount = 0;
			// Rita ut scroll i Y-led.
			if (Pplayer->ypos < oldy) for (i = 0; i <= BACKGRW*8; i+=BACKGRW) drawbox (i-xcount,0,BACKGRW,1, Ppic+BACKGR_OFF+((map[(i/BACKGRW+Pplayer->xpos/BACKGRW)+(Pplayer->ypos/BACKGRH)*MAX_MAPW])*BACKGRW*BACKGRW)+BACKGRH*ycount, Pvirt_background);
			if (Pplayer->ypos > oldy) for (i = 0; i <= BACKGRW*8; i+=BACKGRW) drawbox (i-xcount,SCREENH-BACKGRH-1,BACKGRW,1, Ppic+BACKGR_OFF+((map[(i/BACKGRW+Pplayer->xpos/BACKGRW)+(4+Pplayer->ypos/BACKGRH)*MAX_MAPW])*BACKGRW*BACKGRH)+BACKGRH*ycount, Pvirt_background);
			// Rita scroll i X-led
			if (Pplayer->xpos < oldx) for (i = 0; i <= BACKGRH*4; i+=BACKGRH) drawbox (-(BACKGRH-1),i-ycount,BACKGRW,BACKGRH, Ppic+BACKGR_OFF+((map[(Pplayer->xpos/BACKGRW)+(i/BACKGRH+Pplayer->ypos/BACKGRH)*MAX_MAPW])*BACKGRW*BACKGRH)+xcount-(BACKGRW-1), Pvirt_background);
			if (Pplayer->xpos > oldx) for (i = 0; i <= BACKGRH*4; i+=BACKGRH) drawbox (SCREENW-1,i-ycount,BACKGRW,BACKGRH, Ppic+BACKGR_OFF+((map[(8+Pplayer->xpos/BACKGRW)+(i/BACKGRH+Pplayer->ypos/BACKGRH)*MAX_MAPW])*BACKGRW*BACKGRH)+xcount, Pvirt_background);
		}
		drawbox (mouse.xpos-SMALLOBJW/2, mouse.ypos-SMALLOBJH/2, SMALLOBJW,SMALLOBJH,Ppic+SMALLOBJ_OFF+8*SMALLOBJW*SMALLOBJH, Pvirt_scr);
		memcpy (vga, Pvirt_scr,SCREENW*(SCREENH-BACKGRH));
		memcpy (Pvirt_scr, Pvirt_background, SCREENW*(SCREENH-BACKGRH));
		nosound();
	}
}

/*-----------------------------Main------------------------------*/

int main ()
{
	int i,j, test;
	Ppic = (unsigned char *) farmalloc (128000); // (SPRITEW*SPRITEH)*12
	Pvirt_scr = (unsigned char *) malloc(SCREENW*(SCREENH-BACKGRH)); // Om ”ver 64kb - farcalloc()
	Pvirt_background = (unsigned char *) malloc(SCREENW*(SCREENH-BACKGRH)); // B”r antagligen allokeras sist, skriver ”ver annat minne annars... Hmm...
	Pdummy = (unsigned char *) malloc (512); // Pvirt_background skriver ”ver en del efter sig.. :-/
	if (Pvirt_scr == NULL || Pvirt_background == NULL || Ppic == NULL)
	{
		printf("Out of memory. Increase HEAP (Opt/debug) or conventional memory.\n");
		goto end_program;
	}
	randomize ();
	cls (0,Pvirt_scr, SCREENW*(SCREENH-BACKGRH));
	cls (0,Pvirt_background, SCREENW*(SCREENH-BACKGRH));
	cls (0,Ppic,128000);
	if ((read_gfx (Ppic,"firepwr.dat", "bana.dat", "spics.dat")) == 1) // L„ser grafik.
	{
		printf ("Could not find the gfx-files, exiting...");
		goto end_program;
	}
	directvideo = 1; // Skriver direkt till sk„rmen. Gotoxy, printf.
	setmode (0x13);
	if ((read_pal ("firepwr.pal")) == 1) // L„ser och s„tter palett.
	{
		setmode (0x03);
		printf ("Could not find the palettefile, exiting...");
		goto end_program;
	}
	if ((read_map ("simon.map")) == 1)
	{
		setmode (0x03);
		printf ("Could not find the mapfile, exiting...");
		goto end_program;
	}
	Pplayer = new Tank;
	Pplayer->next = NULL;
	Ptanks = new Tank;
	Ptanks->next = NULL;
	Ptanks_start = Ptanks;
	test = 0;
	for (Pmapobj = Pmapobj_start; Pmapobj->next != NULL; Pmapobj = Pmapobj->next)
	{
		if (Pmapobj->type == 0)
		{
			test++;
			Pplayer->next = NULL;
			Pplayer->prev = NULL;
			init_vars (Pplayer,Pmapobj->forward_speed,Pmapobj->reverse_speed,8,Pmapobj->xpos-STARTX,Pmapobj->ypos-60,0,1,Pmapobj->power,100);
			Pplayer->Pturret = new Turret;
		}
		if (Pmapobj->type == 1) // Definera fiende-tankarnas position etc. Fixa "test"...
		{
			init_vars (Ptanks,random(3)+1,3,8,Pmapobj->xpos-145,Pmapobj->ypos-60,0,1,Pmapobj->power,100);
			Ptanks->next = new Tank;
			Ptanks->next->prev = Ptanks;
			Ptanks = Ptanks->next;
			Ptanks->next = NULL;
		}
	}
	for (Pmapobj = Pmapobj_start; Pmapobj->next != NULL; Pmapobj = Pmapobj->next) // Frig”r minne f”r kartobjekten.
	{
		Pmapobj_start = Pmapobj;
		delete (Pmapobj_start);
	}
	if (test == 0)
	{
		setmode (0x03);
		printf ("No player-object on map! Exiting...\n\n");
		goto end_program;
	}

	show_gfx_file ("pwrlogo.dat", 0, 0,vga); // Visa "inladdningsbilden"
	while (!kbhit());
	flushkbd();
	for (i = 0; i <= 7; i++) // Rita f”rsta bakgrundsbilden.
		for (j = 0; j <= 3; j++) drawbox (i*BACKGRW,j*BACKGRH,BACKGRW, BACKGRH,Ppic+BACKGR_OFF+((map[(i+(Pplayer->xpos/BACKGRW)+((j+(Pplayer->ypos/40)))*MAX_MAPW)]*BACKGRW*BACKGRH)), Pvirt_background);
	show_gfx_file ("dash.dat", 0, SCREENH-BACKGRH,vga);
	show_mouse();
	oldkbdint = getvect(INTR);
	setvect (INTR, handler);
	game (); // Spel-loopen
	setvect (INTR, oldkbdint);
	setmode (0x03);
	end_program:
	free (Pvirt_scr);
	free (Pvirt_background);
	free (Ppic);
	free (Pdummy);
	for (Ptanks = Ptanks_start; Ptanks != NULL; Ptanks = Ptanks->next) // Frig”r minne f”r fiende-tanksen.
	{
		Ptanks_start = Ptanks;
		delete (Ptanks_start);
	}
	delete (Pplayer);
	printf ("Tank: %u bytes\nVehicle: %u bytes\nThing: %u bytes\nTurret: %u bytes\n\n",sizeof(Tank),sizeof(Vehicle), sizeof(Thing),sizeof(Turret));
	return 0;
}
