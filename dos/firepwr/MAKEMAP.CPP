#include "stdlib.h"
#include "stdio.h"
#include "conio.h"
#include "alloc.h"
#include "dos.h"
#include "mem.h"
#include "io.h"

#include "fp_utils.h"
#include "graphics.h"
#include "mouse.h"

/*---------------------------Variabler------------------------------*/

unsigned char *Pvirt_scr = NULL; // Pekare till virtuell sk„rm
unsigned char *Pvirt_background = NULL; // Pekar till bakgrundsbilden
unsigned char pic[50][SPRITEW*SPRITEH]; // Grafik, ”ka storleken!
unsigned char backgr_pic[40][BACKGRW*BACKGRH]; // Om ”ver 40 - huge
int			  backgr_actual = 0;
int			  object_actual = 0;
int 			  total_objects = 0;
int			  maxx = 320;
int			  maxy = 200;
signed char	  changed_map = 0;
signed char	  obj_or_backgr;
char		  	  filename[15] = "noname.map";

/*--------------------------Funktioner------------------------------*/

void skriv_help(){
	memcpy (Pvirt_scr, vga, 64000);
	cls (0,vga,64000);
	gotoxy(1,1);
	printf ("Fire Power editor 1.0!\n----------------------\n");
	printf ("Filename: %s\n\n",filename);
	printf ("f1      this text.\n");
	printf ("p       clear statusline.\n");
	printf ("i       objectinfo (on cursor)\n");
	printf ("s       save the map.\n");
	printf ("d       draw backgroundpics.\n");
	printf ("l       load new map.\n");
	printf ("z       zap objects on map\n");
	printf ("+       next backgroundpic.\n");
	printf ("-       previous backgroundpic.\n");
	printf ("shift + next objectpic.\n");
	printf ("shift - previous objectpic\n");
	printf ("home    upper left corner\n");
	printf ("end     lower right corner\n");
	printf ("del     delete object\n");
	printf ("space   draw background-block\n");
	printf ("enter   draw object\n");
	printf ("arrows  move the cursor.\n");
	printf ("esc     exit.\n");
	printf ("\nPRESS ANY KEY FOR NEXT SCREEN!\n");
	while (!kbhit());
	cls (0,vga,64000);
	gotoxy (1,1);
	printf ("You may change the X and Y-pos' for the\n");
	printf ("objects to any number, but don't place\n");
	printf ("them outside the playfield.\n\n");
	printf ("The player is represented by the first\n");
	printf ("object (#0), enemies by the second (#1).\n");
	delay (2000);
	while (!kbhit());
	cls (0,vga,64000);
	memcpy (vga,Pvirt_scr,64000);
}

void draw_pointer(int x, int y,char col)
{
	int i;
	for (i = 0; i <= BACKGRW; i++)
	// rita upp horizontella linjer.
	{
		if (y < 200 && x+i < 320) putpixel (x+i,y,col,vga);
		if (y+BACKGRH < 200 && x+i < 320) putpixel (x+i,y+BACKGRH,col,vga);
	}
	for (i = 0; i <= BACKGRH; i++)
	// rita upp vertikala linjer
	{
		if (x < 320 && y+i < 200) putpixel (x,y+i,col,vga);
		if (x+BACKGRW < 320 && y+i < 200) putpixel (x+BACKGRW,y+i,col,vga);
	}
}

void zapobjs() {
	for (Pmapobj = Pmapobj_start; Pmapobj != NULL; Pmapobj = Pmapobj->next) {
		if (Pmapobj->prev != NULL) delete (Pmapobj->prev);
	}
	delete (Pmapobj);
	Pmapobj_start = NULL;
	total_objects = 0;
}

void draw_pics () {
	int x;
	int y;
	int picnr = 0;
	memcpy (Pvirt_scr, vga, 64000);
	cls (0,vga,64000);
	for (y = 0; y <= 200; y +=42) {
		for (x = 0; x <= 280; x +=42) {
			drawbox (x,y,BACKGRW,BACKGRH,backgr_pic[picnr],vga,1);
			if (picnr < backgr_pic_nr) picnr++;
		}
	}
	while (!kbhit());
	memcpy (vga, Pvirt_scr, 64000);
}

void draw_map() {
	int x = 0; // Sk„rm x och y, ”kar med 40.
	int y = 0;
	int x_file = 0;
	int y_file = 0; // X och y positioner i filen, ”kar ett steg varje.
	int oldmousex,oldmousey;
	int i,j;
	int temp;
	char c = 255;
	for (i = 0; i <= 319; i++) putpixel(i, 200-BACKGRH+1,3,vga);
	draw_pointer(x,y,1);
	drawbox (320-BACKGRW,200-BACKGRH,BACKGRW,BACKGRH,backgr_pic[backgr_actual],vga,1);
	drawbox (320-BACKGRW*2+5,200-BACKGRH+5,SPRITEW,SPRITEH,pic[object_actual],vga,1);
	for (i = 0; i <= 7; i++)
		for (j = 0; j <= 3; j++) drawbox (i*BACKGRW,j*BACKGRH,BACKGRW,BACKGRH,backgr_pic[map[i+(j*MAX_MAPW)]],vga);
	do {
		gotoxy(1,22);
		printf("f1 - Help\n\nx: %d y: %d\nx_file: %d y_file: %d Pic:%d ",x,y,x_file,y_file,backgr_actual);
		if (kbhit()) {
			c = getch();
			switch (c) {
				case '+' : if (backgr_actual < backgr_pic_nr-1) backgr_actual++;	drawbox (320-BACKGRW,200-BACKGRH,BACKGRW,BACKGRH,backgr_pic[backgr_actual],vga,1);break;
				case '-' : if (backgr_actual > 0) backgr_actual--; drawbox (320-BACKGRW,200-BACKGRH,BACKGRW,BACKGRH,backgr_pic[backgr_actual],vga,1); break;
				case '?' : if (object_actual < obj_pic_nr) object_actual++; drawbox (320-BACKGRW*2+5,200-BACKGRW+5,SPRITEW,SPRITEH,pic[object_actual],vga,1); break;
				case '_' : if (object_actual > 0) object_actual--; drawbox (320-BACKGRW*2+5,200-BACKGRW+5,SPRITEW,SPRITEH,pic[object_actual],vga,1); break;
				case 'd' : draw_pics(); break;
				case 't' : for (i = 0; i <= 6; i++)
								  for (j = 0; j <= 3; j++) drawbox (i*BACKGRW,j*BACKGRH,BACKGRW,BACKGRH,backgr_pic[map[(i+x_file)+((j+y_file)*MAX_MAPW)]],vga);
							  break;

				case 72	 : if (y_file > 0) y_file--;
								if (y > 0)
								{
									 draw_pointer (x,y,0);
									 y-=BACKGRH;
									 draw_pointer (x,y,1);
								}
								else
								{
									 for (i = 0; i <= 7; i++)
										for (j = 0; j <= 3; j++) {
											if (x_file >= 7) temp = 7; else temp = x_file;
											drawbox (i*BACKGRW,j*BACKGRH,BACKGRW,BACKGRH,backgr_pic[map[i+x_file-temp+(j+y_file)*MAX_MAPW]],vga);
//										drawbox (map_object[0].xpos-(x_file-temp)*BACKGRW, map_object[0].ypos-(y_file)*BACKGRH,SPRITEW,SPRITEH,pic[map_object[0].type],vga);
										for (Pmapobj = Pmapobj_start; Pmapobj != NULL; Pmapobj = Pmapobj->next)
										{
											drawbox (Pmapobj->xpos-(x_file-temp)*BACKGRW, Pmapobj->ypos-(y_file)*BACKGRH,SPRITEW,SPRITEH,pic[Pmapobj->type],vga);
										}

										}
								}
								break;
				case 80	 : if (y_file < MAX_MAPH-1) y_file++;
								if (y < maxy-BACKGRH*2)
								{
									 draw_pointer (x,y,0);
									 y+=BACKGRH;
									 draw_pointer (x,y,1);
								}
								else
								{
									 for (i = 0; i <= 7; i++)
										for (j = 0; j <= 3; j++) {
											if (x_file >= 7) temp = 7; else temp = x_file;
											drawbox (i*BACKGRW,j*BACKGRH,BACKGRW,BACKGRH,backgr_pic[map[i+x_file-temp+(j+y_file-3)*MAX_MAPW]],vga);
										}
										for (Pmapobj = Pmapobj_start; Pmapobj != NULL; Pmapobj = Pmapobj->next)
										{
											drawbox (Pmapobj->xpos-(x_file-temp)*BACKGRW, Pmapobj->ypos-(y_file-3)*BACKGRH,SPRITEW,SPRITEH,pic[Pmapobj->type],vga);
										}
								}
								break;
				case 77	 : if (x_file < MAX_MAPW-1) x_file++;
								if (x < maxx-BACKGRW)
								{
									 draw_pointer (x,y,0);
									 x+=BACKGRW;
									 draw_pointer (x,y,1);
								}
								else
								{
									 for (i = 0; i <= 7; i++)
										for (j = 0; j <= 3; j++){
											if (y_file >= 3) temp = 3; else temp = y_file;
											drawbox (i*BACKGRW,j*BACKGRH,BACKGRW,BACKGRH,backgr_pic[map[(i+x_file-7)+(j+y_file-temp)*MAX_MAPW]],vga);
										}
										for (Pmapobj = Pmapobj_start; Pmapobj != NULL; Pmapobj = Pmapobj->next)
										{
											drawbox (Pmapobj->xpos-(x_file-7)*BACKGRW, Pmapobj->ypos-(y_file-temp)*BACKGRH,SPRITEW,SPRITEH,pic[Pmapobj->type],vga);
										}
								}
								break;
				case 75	 : if (x_file > 0) x_file--; // v„nster
								if (x > 0)
								{
									 draw_pointer (x,y,0);
									 x-=BACKGRW;
									 draw_pointer (x,y,1);
								}
								else
								{
									 for (i = 0; i <= 7; i++)
										for (j = 0; j <= 3; j++) {
											if (y_file >= 3) temp = 3; else temp = y_file;
											drawbox (i*BACKGRW,j*BACKGRH,BACKGRW,BACKGRH,backgr_pic[map[(i+x_file)+(j+y_file-temp)*MAX_MAPW]],vga);
										}
										for (Pmapobj = Pmapobj_start; Pmapobj != NULL; Pmapobj = Pmapobj->next)
										{
											drawbox (Pmapobj->xpos-x_file*BACKGRW, Pmapobj->ypos-(y_file-temp)*BACKGRH,SPRITEW,SPRITEH,pic[Pmapobj->type],vga);
										}
								}
								break;
				case 32  :  {
									map[x_file+(y_file*MAX_MAPW)] = backgr_actual;
									drawbox (x,y,BACKGRW,BACKGRH,backgr_pic[backgr_actual],vga);
									changed_map = 1;
								} break;
				case 13	:  { // L„gg till objekt.. buggar som satan->i_helvetet...
									total_objects++;
									if (total_objects != 1) for (Pmapobj = Pmapobj_start; Pmapobj->next != NULL; Pmapobj = Pmapobj->next);
									if (total_objects != 1)	Pmapobj->next = newnode (Pmapobj,x_file*BACKGRW+5,y_file*BACKGRH+5,object_actual,10,0,0,0);
									else Pmapobj = newnode (Pmapobj,x_file*BACKGRW+5,y_file*BACKGRH+5,object_actual,10,0,0,0);
									if (total_objects == 1) Pmapobj_start = Pmapobj;
									drawbox (x+5,y+5,SPRITEW, SPRITEH,pic[object_actual],vga);
									changed_map = 1;
								} break;
				case 27	: if (changed_map) {
									memcpy (Pvirt_scr, vga, 64000);
									cls (0,vga,64000);
									gotoxy(1,1);
									printf ("Map not saved. Really quit? (y/n)");
									while (c != 'y' && c != 'n' && c != 'Y' && c != 'N') c = getch();
									if (c == 'y' || c == 'Y') c = 27;
									else {
										memcpy (vga, Pvirt_scr, 64000);
									}
							  } break;
				case 's' : if (changed_map) {
									memcpy (Pvirt_scr, vga, 64000);
									cls (0,vga,64000);
									gotoxy(1,1);
									printf ("Save map\n\n");
									printf ("Enter filename. 8+3 chars.\n");
									scanf ("%12s", filename);
									printf ("Writing '%s'...",filename);
									write_map (filename);
									printf ("\n\nReady! Press any key.");
									while(!kbhit());
									changed_map = 0;
									memcpy (vga, Pvirt_scr, 64000);
							  } break;
				case 'l' : {   cls (0,vga,64000);
									gotoxy(1,1);
									printf ("Load map\n\n");
									printf ("Enter filename. 8+3 chars.\n");
									scanf ("%12s", filename);
									if ((read_map(filename) == 1))
									{
										printf("Cannot find '%s'...",filename);
									}
									else
									{
										printf ("Reading '%s'...\n",filename);
										printf ("\nReady! Press any key.");
										x_file = 0; y_file = 0; x = 0; y = 0;
										total_objects = 0;
										for (Pmapobj = Pmapobj_start; Pmapobj->next != NULL; Pmapobj = Pmapobj->next) total_objects++;
										total_objects++;
									}
									while(!kbhit());
									changed_map = 0;
									for (i = 0; i <= 7; i++)
										for (j = 0; j <= 3; j++) drawbox (i*BACKGRW,j*BACKGRH,BACKGRW,BACKGRH,backgr_pic[map[(i+x_file)+((j+y_file)*MAX_MAPW)]],vga);
									for (Pmapobj = Pmapobj_start; Pmapobj != NULL; Pmapobj = Pmapobj->next)
									{
										drawbox (Pmapobj->xpos, Pmapobj->ypos,SPRITEW,SPRITEH,pic[Pmapobj->type],vga);
									}
								} break;
				case 'i' :
								for (Pmapobj = Pmapobj_start; Pmapobj != NULL; Pmapobj = Pmapobj->next)
								{
//									Žndra map_object.type nedan, bara vissa f†r vara initierade...
									if ((Pmapobj->xpos >= x_file*BACKGRW && Pmapobj->xpos < x_file*BACKGRW+BACKGRW) && (Pmapobj->ypos >= y_file*BACKGRH && Pmapobj->ypos < y_file*BACKGRW+BACKGRW))
									{
										memcpy (Pvirt_scr, vga, 64000);
										cls (0,vga,64000);
										drawbox (280,0,BACKGRW,BACKGRH,backgr_pic[map[x_file+y_file*MAX_MAPW]],vga);
										drawbox (280+5,0+5,SPRITEW,SPRITEH,pic[Pmapobj->type],vga);
										gotoxy(1,1);
										printf ("Objectinfo\n----------\n");
										printf ("nr:         %d\n",i);
										printf ("xpos:       %d\n",Pmapobj->xpos);
										printf ("ypos:       %d\n",Pmapobj->ypos);
										printf ("type:       %d\n",Pmapobj->type);
										printf ("power:      %d\n",Pmapobj->power);
										printf ("fwd speed:  %d\n",Pmapobj->forward_speed);
										printf ("rev speed:  %d\n",Pmapobj->reverse_speed);
										printf ("\nChange values? (y/n)\n");
										while (c != 'y' && c != 'n' && c != 'Y' && c != 'N')
											c = getch();
										if (c == 'y' || c == 'Y')
										{
											gotoxy (13,4); printf ("       "); gotoxy (13,4);
											scanf ("%d",&Pmapobj->xpos); gotoxy (13,5); printf ("       "); gotoxy (13,5);
											scanf ("%d",&Pmapobj->ypos); gotoxy (13,7); printf ("       "); gotoxy (13,7);
											scanf ("%d",&Pmapobj->power); gotoxy (13,8); printf ("       "); gotoxy (13,8);
											scanf ("%d",&Pmapobj->forward_speed);gotoxy (13,9); printf ("       "); gotoxy (13,9);
											scanf ("%d",&Pmapobj->reverse_speed);gotoxy (13,9); printf ("       "); gotoxy (13,9);
											changed_map = 1;
										}
										memcpy (vga, Pvirt_scr, 64000);
									}
								} break;
				case 'p' :	gotoxy(1,24); clreol(); break;
				case 'z' :  memcpy (Pvirt_scr, vga, 64000);
								cls (0,vga,64000);
								gotoxy (1,1);
								printf ("Objects: %d\n",total_objects);
								printf("Delete all objects? (Y/N)");
								while (c != 'y' && c != 'n' && c != 'Y' && c != 'N') c = getch();
								if (c == 'y' || c == 'Y') zapobjs();
								memcpy (vga, Pvirt_scr, 64000);
                        changed_map = 1;
								break;
				case 59	:  skriv_help(); break;
				case 83	:   // delete (Ta bort objekt).
								for (Pmapobj = Pmapobj_start; Pmapobj != NULL; Pmapobj = Pmapobj->next)
								{
									if ((Pmapobj->xpos >= x_file*BACKGRW && Pmapobj->xpos < x_file*BACKGRW+BACKGRW) && (Pmapobj->ypos >= y_file*BACKGRH && Pmapobj->ypos < y_file*BACKGRW+BACKGRW))
									{
										if (Pmapobj->next != NULL) Pmapobj->prev->next = Pmapobj->next; // Koppla f”rbi den aktuella noden.
										else Pmapobj->prev->next = NULL;
										delete (Pmapobj); // Frig”r minnet.
										changed_map = 1;
										total_objects--;
									}
								} break;
				case 71  :  { // Home
									x_file = 0; y_file = 0; x = 0; y = 0;
									for (i = 0; i <= 7; i++)
										for (j = 0; j <= 3; j++) drawbox (i*BACKGRW,j*BACKGRH,BACKGRW,BACKGRH,backgr_pic[map[(i+x_file)+((j+y_file)*MAX_MAPW)]],vga);
//									drawbox (map_object[0].xpos, map_object[0].ypos,SPRITEW,SPRITEH,pic[map_object[0].type],vga);
								} break;
				case 79  :  { // End
									x_file = MAX_MAPW-1; y_file = MAX_MAPH-1; x = 280; y = 120;
									for (i = 0; i <= 7; i++)
										for (j = 0; j <= 3; j++) drawbox (i*BACKGRW,j*BACKGRH,BACKGRW,BACKGRH,backgr_pic[map[(i+x_file-7)+((j+y_file-3)*MAX_MAPW)]],vga);
//									drawbox (map_object[0].xpos-(x_file-7)*BACKGRW, map_object[0].ypos-(y_file-3)*BACKGRH,SPRITEW,SPRITEH,pic[map_object[0].type],vga);
								} break;
				default: break;
			}
		}
	} while (c != 27);
}

void main () {
	Pmapobj_start->prev = NULL;
	setmode (0x013);
	Pvirt_scr = (unsigned char *) calloc(SCREENW*SCREENH,1); // Om ”ver 64kb - farcalloc()
	Pvirt_background = (unsigned char *) calloc(64000,1);
	if (Pvirt_scr == NULL || Pvirt_background == NULL) {
		printf("Minnesbrist. ™ka HEAP-storlek eller frig”r konventionellt minne.\n");
		exit(1);
	}
	cls (0,Pvirt_scr,64000);
	cls (0,Pvirt_background, 64000);
	show_mouse();
	if ((read_pal ("firepwr.pal")) == 1) // L„ser och s„tter palett.
	{
		setmode (0x03);
		printf ("Could not find the palettefile, exiting...");
		exit(1);
	}
	if ((read_gfx (pic,backgr_pic,"firepwr.dat", "bana.dat",1)) == 1) // L„ser grafik.
	{
		setmode (0x03);
		printf ("Could not find the gfx-files, exiting...");
		exit(1);
	}
	draw_map();
	setmode (0x03);
	Pmapobj = Pmapobj_start;
	printf ("Program slut... _start x:%d\n",Pmapobj_start->xpos);
	for (Pmapobj = Pmapobj_start; Pmapobj != NULL; Pmapobj = Pmapobj->next) {
		printf ("type: %d xpos: %d ypos %d power %d\n",Pmapobj->type,Pmapobj->xpos,Pmapobj->ypos,Pmapobj->power);
	}
	free (Pvirt_scr);
	free (Pvirt_background);
	remove_mouse();
}