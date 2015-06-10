/*
 F”rs”k att g”ra lite grafik i C++.. En del assembler dock. :)
	 Gjort 96-03-02 21.30 -->
	 B”rjar bli MER och MER klart... 96-04-09 11.40
	 En BUSS! Jag „r ett geni!*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>
#include <string.h>

extern "C" {
 #include "mikmod.h"
}


#define  screenw  320    // Sk„rmens propotioner
#define	 screenh  200
#define	 up			  72     // ASCII f”r piltangenterna
#define	 down	    80
#define	 right    77
#define	 left 	  75
#define  true     1      // Boolska variabler
#define  false    0

const		 unsigned char boxw	=		9;
const		 unsigned char boxh =		9;
const		 unsigned char smallfontw	=		4;
const		 unsigned char smallfonth =		7;
const    unsigned char maxgridx = 10;
const		 unsigned char maxgridy = (screenh / boxh)+4; // + stones
const		 unsigned char maxstones = 5;
const		 unsigned char maxstonepics = 7;
const		 unsigned char maxsmallfontpics = 25;
const    unsigned char xoffset = (320 - (boxw*maxgridx)) / 2;
const		 int	         standarddelay = 20;
const		 int			   	 faststddelay	 = 1;

struct	 mapstruct {
					unsigned char grid[maxgridx][maxgridy];
				 };

struct	 clmstruct {
					unsigned char body[maxstones];
					unsigned char	xpos;
					unsigned char	ypos;
					unsigned char	oldxpos;
					unsigned char	oldypos;
				 };

struct 	 playerstruct {
					unsigned char level;
					char name[80];
					unsigned long score;
					unsigned int blocks;
				 };

struct   picstruct {
					unsigned char body[maxstonepics][boxw*boxh];
				 };

mapstruct *map;
clmstruct *column;
playerstruct *player;
picstruct *pic;
UNIMOD    *modfile;

unsigned char maptmp[maxgridx][maxgridy];
unsigned char font6x4[maxsmallfontpics+1][smallfontw*smallfonth];
unsigned char gridx = maxgridx;

FILE     *handle;
int 		 c;            // Sparar statusen fr†n keyb...
int      gus;
int			 stones = 4;   // Antalet klossar
int      delaylevel = 0;
int 		 delaytime = standarddelay - player->level;
int			 timer;        // Kolla p† denna f”r timing.
int      hiscore;
int 		 inclevel;

int      remover;
int			 out;

unsigned int oldtime; // N„r ska timer ”kas??
unsigned char nextcolumn[maxstones];

void tickhandler(void)
{
	MP_HandleTick();    /* play 1 tick of the module */
	MD_SetBPM(mp_bpm);
}

void setmode (unsigned int mode) {
 asm {
	mov ax, mode
	int 0x10
 }
}

void putpixel (unsigned int x, unsigned int y, unsigned char col) {
 asm {
	mov cx, x
	mov ax, y
	mov bx, screenw
	imul bx
	add ax, cx
	mov bx, ax
	mov ax, 0xa000
	mov es, ax
	mov al, col
	mov [es:bx], al
 }
}

void setpal (unsigned char col, unsigned char r,unsigned char g,unsigned char b) {
 outportb (0x3c8, col);
 outportb (0x3c9, r);
 outportb (0x3c9, g);
 outportb (0x3c9, b);
}

void drawbox (unsigned char x, unsigned char y, unsigned char nr) {
 int i,j;
 for (i = 0; i <= boxw-1; i++)
	for (j = 0; j <= boxh-1; j++)
	 if (nr != 0) putpixel ((x*boxw+i)+xoffset,(y*boxh+j), pic->body[nr-1][j*boxw+i]);
	 else putpixel ((x*boxw+i)+xoffset,(y*boxh+j), 0);
}

int init_variables(int flush) {
 if (!flush) {
	map = new mapstruct;            // Kolla om detta funkar som det ska..
	column = new clmstruct;
	player = new playerstruct;
	pic = new picstruct;
	modfile = new UNIMOD;
 }
 else {
	delete map;
	delete column;
	delete player;
	delete pic;
 }
 return (0);
}

void putfont (unsigned char ch, int xpos, int ypos, int fonttype) {
 int x, y;
 if (fonttype == 1) {
	for (x = 0; x <= smallfontw-1; x++) for (y = 0; y <= smallfonth-1; y++)
	if (font6x4[ch][y*smallfontw+x] != 0) putpixel (x+xpos,y+ypos, font6x4[ch][y*smallfontw+x]);
 }
}

void display_text (char *str, int xpos, int ypos) {
 int i;
 for (i = 0; i <= strlen (str); i++) {
	if ((str[i] >= 97) && (str[i] <= 123)) putfont (str[i]-97,xpos+(smallfontw+1)*i,ypos, 1);
	if ((str[i] >= 65) && (str[i] <= 90)) putfont (str[i]-65,xpos+(smallfontw+1)*i,ypos,1);
 }
}

void create_column () {  // Fixa removern
 int i,seed;
 for (i = 0; i <= stones-1; i++) column->body[i] = nextcolumn[i];
 if (remover != 9) {
	for (i = 0; i <= stones-1; i++) {
	 seed = random (6)+1;
	 nextcolumn[i] = seed;
	}
 } else {
	nextcolumn[0] = 8;
	nextcolumn[1] = 8;
	nextcolumn[2] = 8;
	nextcolumn[3] = 8;
	remover = 0;
 }
 if (inclevel == 100) {
	player->level++;
	inclevel = 0;
 }
 if (player->level >= 9) {
	player->level = 9;
	delaylevel = 19;
 }
 else delaylevel = 2*player->level;
 for (i = 0; i <= stones-1; i++) drawbox (20,i+2,nextcolumn[i]);
}

unsigned int get_time () {  // Kom ih†g att fixa Longint n†n g†ng..
 return (peek (0x040,0x006c));
}

void update_map(unsigned char remove) {
 int x,y;
 for (x = 0; x <= gridx-1; x++) for (y = stones; y <= maxgridy-1; y++)
 if (!remove) {
	if (map->grid[x][y] != 0) drawbox (x,y-stones+1,map->grid[x][y]);
 } else	drawbox (x,y-stones+1,map->grid[x][y]);
}

int remove_stones() {
 int x,y,i, retcheck = 0;
 for (x = 0; x <= gridx-1; x++) for (y = stones; y <= maxgridy-1; y++) {
	if (maptmp[x][y] == 1) {
	 for (i = 0; i <= 1; i++) {
		for (x = 0; x <= gridx-1; x++) for (y = stones; y <= maxgridy-1; y++)
		if (maptmp[x][y] == 1)	drawbox (x,y-stones+1,0);
		delay (250);
		for (x = 0; x <= gridx-1; x++) for (y = stones; y <= maxgridy-1; y++)
		if (maptmp[x][y] == 1) drawbox (x,y-stones+1,map->grid[x][y]);
		delay (250);
	 }
	}
 }
 for (x = 0; x <= gridx-1; x++) for (y = stones; y <= maxgridy-1; y++) {
	if (maptmp[x][y] == 1) {
	 map->grid[x][y] = 0;
	 retcheck++;
	 player->blocks++;
	 inclevel++;
	 player->score = player->score + ((5+player->level) * retcheck);
	 for (i = 0; i <= 2; i++) {
		drawbox (x,y-stones+1,map->grid[x][y]);
		delay (12);
		drawbox (x,y-stones+1,0);
		delay (12);
	 }
	}
 }
 return (retcheck);
}


int check_key (int ch) {
int i,tmp;
 switch (ch) {
	case up:
	 tmp = column->body[stones-1];
	 for (i = stones-2; i >= 0; i--) {
		column->body[i+1] = column->body[i];
	 }
	 column->body[0] = tmp;
	 break;
	case down: if (map->grid[column->xpos][column->ypos+stones] == 0 && column->ypos < maxgridy-stones-1) {
	 delaytime = faststddelay;
	 timer = delaytime;
	} break;
	case right: if (column->xpos <= gridx-2) {
	 for (i = 0; i <= stones; i++) {
		if (map->grid[column->xpos+1][(column->ypos+i)-1] != 0) return(10);
		map->grid [column->xpos][(column->ypos+i)-1] = 0;
		drawbox (column->xpos, column->ypos+i-stones, 0);
	 }
	 column->xpos++;
	} break;
	case left: if (column-> xpos > 0) {
		for (i = 0; i <= stones; i++) {
		 if (map->grid[column->xpos-1][(column->ypos+i)-1] != 0) return(10);
		 map->grid [column->xpos][(column->ypos+i)-1] = 0;
		 drawbox (column->xpos, column->ypos+i-stones, 0);
		}
		column->xpos--;
	 } break;
	default: delaytime = standarddelay - delaylevel; break;
 }
 return (0);
}

int check_stones () {
 int x,y,i,out,check = false;
 for (y = stones; y <= maxgridy-1; y++) for (x = 0; x <= gridx-1; x++) maptmp[x][y] = 0;
 for (x = 0; x <= gridx-1; x++) for (y = stones; y <= maxgridy-1; y++)  {

	if ((column->body[1] == 7 || column->body[1] == 8) &! map->grid[column->xpos][column->ypos+stones] == 0) {
	 for (x = 0; x <= gridx-1; x++) for (y = stones; y <= maxgridy-1; y++) {
		if (map->grid[x][y] == map->grid[column->xpos][column->ypos+stones]) maptmp[x][y] = 1;
		if ((map->grid[x][y] == 7) || (map->grid[x][y] == 8)) maptmp[x][y] = 1;
	 }
	 for (x = 0; x <= stones; x++) column->body[x] = 0;
	 break;
	}

	if (x < gridx-2) for (i = 1; i <= 2; i++) {
	 if (map->grid[x][y] == map->grid[x+i][y]) check = true;
	 else {
		check = false;
		break;
	 }
	} else check = false;
	if (check == true &! map->grid[x][y] == 0) for (i = 0; i <= 2; i++) maptmp[x+i][y] = 1;

	for (i = 1; i <= 2; i++) {
	 if (map->grid[x][y] == map->grid[x][y-i]) check = true;
	 else {
		check = false;
		break;
	 }
	}
	if (check == true &! map->grid[x][y] == 0) for (i = 0; i <= 2; i++) maptmp[x][y-i] = 1;

	if (x < gridx-2)	for (i = 1; i <= 2; i++) {
	 if (map->grid[x][y] == map->grid[x+i][y+i]) check = true;
	 else {
		check = false;
		break;
	 }
	} else check = false;
	if (check == true &! map->grid[x][y] == 0) for (i = 0; i <= 2; i++) maptmp[x+i][y+i] = 1;

	if (x < gridx-2)	for (i = 1; i <= 2; i++) {
	 if (map->grid[x][y] == map->grid[x+i][y-i]) check = true;
	 else {
		check = false;
		break;
	 }
	} else check = false;
	if (check == true &! map->grid[x][y] == 0) for (i = 0; i <= 2; i++) maptmp[x+i][y-i] = 1;
 }
 out = remove_stones(); // Funkar? Tror det...
 update_map(1);
 return (out);
}

void check_fall () {
 int x,y,i,counter;
 do {
	counter = 0;
	for (y = maxgridy-2; y >= stones; y--) for (x = gridx-1;x >= 0; x--)
	if (map->grid[x][y] == 0) if (map->grid[x][y-1] != 0) {
	 map->grid[x][y] = map->grid[x][y-1];
	 map->grid[x][y-1] = 0;
	 counter++;
	}
	update_map(1);
 } while (counter != 0);
}

void display_map() { // Bugg-letare
 int x,y;
 setmode (0x03);
 for (y = stones; y <= maxgridy-1; y++) {
	for (x = 0; x <= gridx-1; x++) {
	 printf ("%3i",map->grid[x][y]);
	}
 printf ("\n");
 }
 while (!kbhit());
 setmode (0x013);
}

int move_column() {
 int i,checkout = false;
 // M†ste vara do - while, annars hinner inte if - return kollas...
 do {
	if (kbhit()) c = getch(); {
	 if (c == 27) {
		display_text ("Really Quit",140,90);
		do {
		 if (kbhit()) c = getch();
		} while ((c != 'Y') && (c != 'y') && (c != 'N') && (c != 'n'));
		if ((c == 'Y') || (c == 'y')) {c = 27;return (c);}
		else {
		 c = 0;
		 update_map(1);
		}
	 }
	 else check_key(c);
	 if (c == 'm') display_map(); // Ta bort Efterhand..
	 if (c == 'w') remover = 9; // Ta bort Efterhand..
	 if (c == 'l') player->level++; // Ta bort Efterhand..
	 if (c == 'r') MP_SetPosition(0);
	 c = 0;
	}
	if (get_time() != oldtime) {
	 timer++;
	 oldtime = get_time();
	}
	if (timer >= delaytime) {
	 timer = 0;
	 if (map->grid[column->xpos][column->ypos+stones] == 0) {
		column->oldypos = column->ypos;
		column->ypos++;
	 }
	}
	for (i = 0; i <= stones-1; i++) {
	 if (column->body[i] == 7) column->body[i] = 8;
	 else if (column->body[i] == 8) column->body[i] = 7;
	 drawbox (column->oldxpos,column->oldypos-stones,0);
	 map->grid [column->oldxpos][(column->oldypos+i)] = 0;
	 column->oldxpos = column->xpos;
	 column->oldypos = column->ypos;
	 map->grid [column->xpos][(column->ypos+i)] = column->body[i];
	}
	if (map->grid[column->xpos][column->ypos+stones] != 0) for (i = 0; i <= gridx-1; i++) if (map->grid[i][3] != 0) return(10);
	update_map(0);
	if (!((column->ypos <= (maxgridy-stones-2)) && (map->grid[column->xpos][column->ypos+stones] == 0))) checkout = true;
	if (!(timer == delaytime-1)) checkout = false;
 } while (checkout == false);
 return(0);
}

void menu () {
 const int point = 7;
 int x,y,xpos,ypos, i = 0;
 int checker[2];
 out = false;
 xpos = 80;
 ypos = 70;
 for (x = 0; x <= 319; x++) for (y = 0; y <= 199; y++) putpixel (x,y,0);
 player->level = 0;
 for (i = 0; i <= 1; i++) checker[i] = 193;
 for (x = 0; x <= boxw-1; x++) for (y = 0; y <= boxh-1; y++)
	putpixel (x+xpos,y+ypos, pic->body[point][y*boxw+x]);
 for (i = 0; i <= 1; i++)
	for (x = 0; x <= boxw-1; x++) for (y = 0; y <= boxh-1; y++)
	 putpixel (x+checker[i],y+(80+(i*20)-1), pic->body[point][y*boxw+x]);
 display_text ("Top score",100,40);
 display_text ("PlayField          S  M  L",100,70);
 display_text ("Starting level     A  B  C  D  E  F  G  H",100,90);
 display_text ("Start Game",100,110);
 gotoxy (25,6);
 printf ("%d",hiscore);
 do {
	if (kbhit()) c = getch();
	if (c == down) {
	 for (x = 0; x <= boxw; x++) for (y = 0; y <= boxh; y++)
		putpixel (x+xpos,y+ypos,0);
	 if (ypos < 110) ypos += 20;
	 for (x = 0; x <= boxw-1; x++) for (y = 0; y <= boxh-1; y++)
		putpixel (x+xpos,y+ypos, pic->body[point][y*boxw+x]);
	 c = 0;
	}
	if (c == up) {
	 for (x = 0; x <= boxw; x++) for (y = 0; y <= boxh; y++)
		putpixel (x+xpos,y+ypos,0);
	 if (ypos > 70) ypos -= 20;
	 for (x = 0; x <= boxw-1; x++) for (y = 0; y <= boxh-1; y++)
		putpixel (x+xpos,y+ypos, pic->body[point][y*boxw+x]);
	 c = 0;
	}
	if (c == right) {
		if (checker[0] < 223) for (x = 0; x <= boxw; x++) for (y = 0; y <= boxh; y++)
		 putpixel (x+checker[0]-1,y+ypos-10,0);
		if (checker[1] < 298) for (x = 0; x <= boxw; x++) for (y = 0; y <= boxh; y++)
		 putpixel (x+checker[1]-1,y+ypos-10,0);
	 if ((ypos == 70) && (checker[0] < 223)) {
		checker[0] = checker[0] + 15;
		for (x = 0; x <= boxw-1; x++) for (y = 0; y <= boxh-1; y++)
		 putpixel (x+checker[0],y+ypos-10, pic->body[point][y*boxw+x]);
	 }
	 if ((ypos == 90) && (checker[1] < 298)) {
		checker[1] = checker[1] + 15;
		for (x = 0; x <= boxw-1; x++) for (y = 0; y <= boxh-1; y++)
		 putpixel (x+checker[1]-1,y+ypos-10, pic->body[point][y*boxw+x]);
	 }
	 c = 0;
	}
	if (c == left) {
	 for (i = 0; i <= 1; i++)
	 if (checker[i] > 193) for (x = 0; x <= boxw; x++) for (y = 0; y <= boxh; y++)
		putpixel (x+checker[i],y+ypos-10,0);
	 if ((ypos == 70) && (checker[0] > 193)) {
		checker[0] = checker[0] - 15;
		for (x = 0; x <= boxw-1; x++) for (y = 0; y <= boxh-1; y++)
		 putpixel (x+checker[0]-1,y+ypos-10, pic->body[point][y*boxw+x]);
	 }
	 if ((ypos == 90) && (checker[1] > 193)) {
		checker[1] = checker[1] - 15;
		for (x = 0; x <= boxw-1; x++) for (y = 0; y <= boxh-1; y++)
		 putpixel (x+checker[1]-1,y+ypos-10, pic->body[point][y*boxw+x]);
	 }
	 c = 0;
	}
	if (c == 27) out = true;
 } while ((out != true) || ((c != 13) && (ypos == 110)));
 for (x = 0; x <= 319; x++) for (y = 0; y <= 199; y++) putpixel (x,y,0);
 for (i = 1; i < 10; i++) if (checker[1] == 193+(i*15)) player->level = i;
}

void new_game () {
 int x,y;
 for (x = 0; x <= gridx-1; x++)
	for (y = 0; y <= maxgridy-1; y++) {
	 map->grid[x][y] = 0;
	 drawbox (x,y,0);
	}
 menu ();
 display_text ("Press key to start",115,100);
 player->score = 0;
 player->blocks = 0;
 if (player->level >= 9) {
	player->level = 9;
	delaylevel = 19;
 }
 else delaylevel = 2*player->level;
 delaytime = standarddelay - delaylevel;
 for (x = 0; x <= stones-1; x++) drawbox (20,x+2,nextcolumn[x]);
}


void readpal () {
 unsigned int i,col = 3;
 unsigned char *pal = new unsigned char[768];
 handle = fopen ("test.dat", "rb");
 fread (pal,1,768, handle);
 for (i = 1; i <= 255; i++) {
	setpal (i, pal[col], pal[col+1], pal[col+2]);
	col += 3;
 }
 fclose (handle);
}

void readpic () {      // Ordna till!!
 int x,y,picnr;
 unsigned char buf[boxw];
 handle = fopen ("test.dat", "rb");
 for (picnr = 0; picnr <= maxstonepics; picnr++) {
	for (y = 0; y <= boxh-1; y++) { // L„ser ETT tecken..
	 fseek (handle,(y*(boxw+1))+(90*picnr)+768, SEEK_SET);
	 fread (buf,1,boxw,handle);
	 for (x = 0; x <= boxw-1; x++) pic->body[picnr][y*boxw+x] = buf[x];
	}
 }
 for (picnr = 0; picnr <= 25; picnr++) {
	for (y = 0; y <= smallfonth-1; y++) { // L„ser ETT tecken..
	 fseek (handle,((y*(smallfontw+1))+(35*picnr)+1488), SEEK_SET);
	 fread (buf,1,smallfontw,handle);
	 for (x = 0; x <= smallfontw-1; x++) font6x4[picnr][y*smallfontw+x] = buf[x];
	}
 }
 fclose (handle);
}

int main () {
 int utkomst;
 clrscr();
 printf ("GUS or No sound? [G/N]");
 while ((c != 'G') && (c != 'N') && (c != 'g') && (c !='n')) {
	if (kbhit()) c = getch();
 }
 if ((c == 'G') || (c == 'g')) gus = true;
 else gus = false;
 if (gus) printf ("\nLoading data.. Please Wait...");
 if (gus) {
	md_mixfreq      =44100;                     /* standard mixing freq */
	md_dmabufsize   =10000;                     /* standard dma buf size */
	md_mode         =DMODE_16BITS|DMODE_STEREO; /* standard mixing mode */
	md_device       =0;                         /* standard device: autodetect */
	mp_volume			  =64;
	ML_RegisterLoader(&load_xm);
	MD_RegisterDriver(&drv_gus);
	MD_RegisterPlayer(tickhandler);
	if(!MD_Init()){
	 printf("Driver error: %s.\n",myerr);
	 return 100;
	}
	modfile=ML_LoadFN("mrw-nmar.xm");
	if(modfile==NULL) {
	 printf("Modloader Error: %s\n",myerr);
	 return (100);
	}
	MP_Init(modfile);
	md_numchn=modfile->numchn; // Fler kanaler f”r ljudeffekter senare..
 }
 if (init_variables (0) != 0) return (10);
 randomize();
 setmode (0x13);
 directvideo = 1;
 readpal();
 readpic();
 create_column();
 oldtime = get_time();
 do {
	new_game();
	if (utkomst != 27) while (!kbhit());
	update_map (1);
	if (gus) {
	 MP_SetPosition(0);
	 MD_PlayStart();
	}
	utkomst = 0;
	while ((utkomst != 10) && (utkomst != 27)) {
	 create_column ();
	 gotoxy (6,1);
	 display_text ("Level", 1, 0);
	 display_text ("Score", 1, 16);
	 display_text ("Blocks",1, 32);
	 printf ("%d\n",player->level);
	 gotoxy (6,3);
	 printf ("%d\n",player->score);
	 gotoxy (6,5);
	 printf ("%d\n",player->blocks);
	 column->xpos = 5;
	 column->ypos = 0;
	 column->oldxpos = 5;
	 column->oldypos = 0;
	 utkomst = move_column();
	 while (check_stones () != false) check_fall();
	 update_map(0);
	}
	display_text ("Game Over",140,90);
	if (player->score > hiscore) hiscore = player->score;
	while (!kbhit());
 if (gus)	MD_PlayStop();
 } while (out != true);
 if (gus) {
	ML_Free(modfile);
	MD_Exit();
 }

 init_variables (1);
 setmode (0x03);
 printf ("Columns v1.0 by Marwin & Simon K†gstr”m / Bitwise\n");
 printf ("Modplayer by Jean-Paul Mikkers (MikMod)\n\n");
 printf ("Inte riktigt klart... :-) Kommer aldrig att bli det heller :-/\n");
 return(0);
}
/*

Buggs:

1) (Det f”rsvinner direkt n„r klossen tar i den under) buggchecka...
2) Word-tid. Fixa longint.. (?)
3) F”r enkelt spel...

Fler funktioner:
1) Ljudeffekter
2) Mer grafik
3) Po„ng, levlar I stort sett klart..
4) (Skriv grafiken till virtuell sk„rm..)

*/