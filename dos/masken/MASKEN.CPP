// Maskspel p†b”rjat 96-11-09

#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "conio.h"
#include "dos.h"
#include "mem.h"


/*------------------Konstanter------------------*/

const max_players			= 2;
const stdlength			= 5;
const maxlength 			= 400;
const lengthinc			= 10;

const applesfinish		= 20;
const apples_on_screen  = 5;

const char maskchar[2]	= {'Û',15};
const char maskhead[2]  = {'²',13};
const char applechar[2] = {'$',10};
const char wallchar[2]  = {'±', 7};

/*------------------Variabler-------------------*/

struct {
 int xpos;
 int ypos;
}    mask[max_players][maxlength];

char map[4000];
char key;
char dir;

unsigned int  biosclock;
unsigned int  oldbios;
unsigned long timer;
unsigned int  speedfactor = 2;

char        playcount;
signed char xmodify[max_players];
signed char ymodify[max_players];
char        key_hit[max_players];

char nr_players = 2;
int  length[max_players];
int  apples[max_players];
unsigned int  points[max_players];
int  alive[max_players];
int  curlevel;

/*------------------Funktioner------------------*/

void ch_put (int pos, char bokst, char attrib) {
 _asm {
	mov bx, pos
	shl bx, 1
	mov ax, 0xb800
	mov es, ax
	mov al, bokst
	mov ah, attrib
	mov [es:bx], ax
 }
}

char ch_get (int pos) {
char bokst;
 _asm {
	mov bx, pos
	shl bx, 1
	mov ax, 0xb800
	mov es, ax
	mov ax, [es:bx]
	mov bokst, al
 }
 return (bokst);
}

inline unsigned char gettime () { return (peekb(0x040,0x006c)); }

char readkey () {
 unsigned char alreg, ahreg;
 unsigned int  axreg;
 _asm {
	mov ax, 0x1000
	int 0x16
	mov ahreg, ah
	mov alreg, al
	mov axreg, ax
 }
 if ((alreg == 0) || (alreg > 127)) return (ahreg + 128);
 else return (axreg);
}

void cursonoff (int on) {
 if (on == 0) asm {
	mov ah, 0x01
	mov ch, 0x30
	int 0x10
 }
 else asm {
	mov ah, 0x01
	mov ch, 0x07
	mov cl, 0x8
	int 0x10
 }
}

void readmap (unsigned int level) {
 int x,y,filelength;
 char filename[12];
 FILE *mapfile;
 sprintf(filename,"masken.%i",level);
 if (((mapfile = fopen (filename,"rb")) == NULL)) { printf ("\nHittar inte filen!\n"); exit (100);}
 fseek (mapfile,0,SEEK_END);
 filelength = ftell (mapfile);
 fseek (mapfile,0,SEEK_SET);
 fread (map, 1, filelength, mapfile);
 fclose (mapfile);
 for (x = 0; x <= 79; x++) for (y = 0; y <= 49; y++) {
	if (map[y*80+x] == wallchar[0])	ch_put (y*80+x,wallchar[0],wallchar[1]);
 }
}

void putapple () {
 int xpos, ypos;
 do {
	xpos = random (78)+1;
	ypos = random (46)+1;
 } while (ch_get (ypos*80+xpos) == maskchar[0] || ch_get (ypos*80+xpos) == wallchar[0]);
 ch_put (ypos*80+xpos, applechar[0],applechar[1]);
}

void checkdead (char player) {
 int i;
 if (!alive[player]) for (i = 0; i <= length[player]; i++) {
	ch_put (mask[player][i].ypos*80+mask[player][i].xpos,0,0);
	alive[player] = 2;
 }
}

char checkhit (char player, int x, int y) {
 char scrpos,i,kill;
 scrpos = ch_get (y*80+x);
 if (mask[player][0].xpos <= 0 || mask[player][0].xpos >= 79) {alive[player] = 0; kill = 1;} else kill = 0;
 if (mask[player][0].ypos <= 0 || mask[player][0].ypos > 46) {alive[player] = 0; kill = 1;} else kill = 0;
 if (scrpos == maskchar[0]) alive[player] = 0;
 if (scrpos == maskhead[0] && mask[!player][0].xpos == x && mask[!player][0].ypos == y) {
	alive[player] = 0;
	alive[!player] = 0;
 }
 if (scrpos == wallchar[0]) {alive[player] = 0; kill = 1;} else kill = 0;
 if (scrpos == applechar[0]) {
	apples[player]++;
	for (i = 0; i < lengthinc-1; i++) {
	 length[player]++;
	 mask[player][length[player]].xpos = mask[player][length[player]-1].xpos;
	 mask[player][length[player]].ypos = mask[player][length[player]-1].ypos;
	 points[player]+=132;
	}
	putapple();
 }
 return (kill);
}

void movemask (char player, signed char xmod, signed char ymod) {
 int i,j;
 if (checkhit (player, mask[player][0].xpos+xmod, mask[player][0].ypos+ymod) == 0) {
 ch_put (mask[player][length[player]].ypos*80+mask[player][length[player]].xpos,' ',12);
 for (i = length[player]; i >= 1; i--) {
	mask[player][i].xpos = mask[player][i-1].xpos;
	mask[player][i].ypos = mask[player][i-1].ypos;
 }
 mask[player][0].xpos+=xmod;
 mask[player][0].ypos+=ymod;
 for (i = 1; i <= length[player]; i++) ch_put (mask[player][i].ypos*80+mask[player][i].xpos,maskchar[0],maskchar[1]);
 ch_put (mask[player][0].ypos*80+mask[player][0].xpos,maskhead[0],maskhead[1]+player);
 key_hit[player] = 0;
 }
}

void initmask (char player) {
 int i;
 alive[player] = 1;
 apples[player] = 0;
 length[player] = stdlength;
 for (i = 0; i <= length[player]; i++) {
	mask[player][i].xpos = 40+player;
	mask[player][i].ypos = 25+player;
 }
 ch_put (mask[player][0].ypos*80+mask[player][0].xpos, maskhead[0],maskhead[1]+player);
}

void new_game() {
 int i;
 memset (&mask,0,sizeof(mask));
 if (curlevel != -1) curlevel++;
 clrscr();
 gotoxy (1,48);
 if (curlevel != -1) readmap (curlevel);
 printf ("ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ");
 for (i = 0; i <= nr_players-1; i++) initmask(i);
 for (i = 0; i < apples_on_screen; i++) putapple ();
 gotoxy (1,50);
 clreol ();
 if (curlevel > 1) printf ("Grattis! Till n„sta niv†... Tryck tangent!");
 else printf ("Tryck tangent f”r att starta!");
 clreol ();
 while (!kbhit());
}

void putstatus () {
 gotoxy (1,49);
 printf ("Player 1:  Antal „pplen: %i\t Žpplen att „ta: %i\tPo„ng: %u\n",apples[0],applesfinish-apples[0],points[0]);
 gotoxy (1,50);
 printf ("Player 2:  Antal „pplen: %i\t Žpplen att „ta: %i\tPo„ng: %u",apples[1],applesfinish-apples[1],points[1]);
 if (applesfinish-apples[0] == 0 || applesfinish-apples[1] == 0) new_game();
}

unsigned long calibrate () {
 unsigned long i;
 unsigned int clock = gettime();
 while (clock == gettime()) i++;
 return (i);
}

/*-------------------Main-----------------------*/

void main (int nr,char *arg[]) {
 int svar;
 if (nr > 1) {
	if ((svar = strcmp (arg[1], "?") == 0) || (svar = strcmp (arg[1], "/?") == 0)) {
	 printf ("\nMasken 1.0 av Simon K†gstr”m\n---------Anv„ndning---------\n/0..999   Repsektive bana (0 f”r ingen.)\n/?        Denna text\n/P:[1..2] Antal spelare\n");
	 exit (20);
	}
	if ((svar = strcmp (arg[1], "P:1") == 0) || (svar = strcmp (arg[1], "/P:1") == 0)) { nr_players = 1; svar = -1; curlevel = 0;}
	if ((svar = strcmp (arg[2], "P:1") == 0) || (svar = strcmp (arg[2], "/P:1") == 0)) { nr_players = 1; curlevel = 0;}
	if ((svar = strcmp (arg[1], "P:2") == 0) || (svar = strcmp (arg[1], "/P:2") == 0)) { nr_players = 2; svar = -1; curlevel = 0;}
	if ((svar = strcmp (arg[2], "P:2") == 0) || (svar = strcmp (arg[2], "/P:2") == 0)) { nr_players = 2; curlevel = 0;}
	if (svar != -1) {
	 sscanf(arg[1], "%i", &svar);
	 if (svar >= 0 && svar <= 999) curlevel = svar-1;
	 else {
		printf ("\nV„lj banor mellan 0 och 999!");
		exit (30);
	 }
	}
 }
 textmode (64);
 randomize();
 cursonoff (0);
 new_game();
 while (key != 27) {
	biosclock = gettime();
	if (oldbios != biosclock) {oldbios = biosclock; timer++;}
	putstatus ();
	if (kbhit()) {
	 key = readkey ();
	 switch (key) {
		// Player 1
		case 200 : if (alive[0] == 1) if (ymodify[0] !=  1 && ymodify[0] != -1) { xmodify[0] =  0; ymodify[0] = -1; key_hit[0] = 1;} break;
		case 203 : if (alive[0] == 1) if (xmodify[0] !=  1 && xmodify[0] != -1) { xmodify[0] = -1; ymodify[0] =  0; key_hit[0] = 1;} break;
		case 205 : if (alive[0] == 1) if (xmodify[0] != -1 && xmodify[0] !=  1) { xmodify[0] =  1; ymodify[0] =  0; key_hit[0] = 1;} break;
		case 208 : if (alive[0] == 1) if (ymodify[0] != -1 && ymodify[0] !=  1) { xmodify[0] =  0; ymodify[0] =  1; key_hit[0] = 1;} break;
		// Player 2
		case 'q' : if (alive[1] == 1) if (ymodify[1] !=  1 && ymodify[1] != -1) { xmodify[1] =  0; ymodify[1] = -1; key_hit[1] = 1;} break;
		case 'r' : if (alive[1] == 1) if (xmodify[1] !=  1 && xmodify[1] != -1) { xmodify[1] = -1; ymodify[1] =  0; key_hit[1] = 1;} break;
		case 't' : if (alive[1] == 1) if (xmodify[1] != -1 && xmodify[1] !=  1) { xmodify[1] =  1; ymodify[1] =  0; key_hit[1] = 1;} break;
		case 'a' : if (alive[1] == 1) if (ymodify[1] != -1 && ymodify[1] !=  1) { xmodify[1] =  0; ymodify[1] =  1; key_hit[1] = 1;} break;
		case 'Q' : if (alive[1] == 1) if (ymodify[1] !=  1 && ymodify[1] != -1) { xmodify[1] =  0; ymodify[1] = -1; key_hit[1] = 1;} break;
		case 'R' : if (alive[1] == 1) if (xmodify[1] !=  1 && xmodify[1] != -1) { xmodify[1] = -1; ymodify[1] =  0; key_hit[1] = 1;} break;
		case 'T' : if (alive[1] == 1) if (xmodify[1] != -1 && xmodify[1] !=  1) { xmodify[1] =  1; ymodify[1] =  0; key_hit[1] = 1;} break;
		case 'A' : if (alive[1] == 1) if (ymodify[1] != -1 && ymodify[1] !=  1) { xmodify[1] =  0; ymodify[1] =  1; key_hit[1] = 1;} break;
		// Allm„nt
		case '+' : if (speedfactor != 1) { speedfactor--; timer = 0; } break;
		case '-' : if (speedfactor <= 18) { speedfactor++; timer = 0; } break;
	 }
	}
	for (playcount = 0; playcount <= nr_players-1; playcount++) {
	 checkdead (playcount);
	 if (alive[playcount] == 2) { initmask(playcount); xmodify[playcount] = 0; ymodify[playcount] = 0; }
	}
	if (timer >= speedfactor || key_hit[0] || key_hit[1]) {
	 for (playcount = 0; playcount <= nr_players-1; playcount++) if (alive[playcount] == 1) movemask(playcount,xmodify[playcount],ymodify[playcount]);
	 timer = 0;
	}
 }
 cursonoff (1);
}
