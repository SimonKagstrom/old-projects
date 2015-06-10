/*
Grafik-hantering

Konstanter:
SCREENW, SCREENH: Grafiska sk„rmens storlek.

variabel:
unsigned char *vga = (unsigned char *) MK_FP(0xA000, 0) - Pekar till VGA-minnet

Funktioner:
void setmode (int mode)
void cls(unsigned char col, unsigned char *where) - rensar minnet i *where
void retrace() - V„ntar p† vertikal / horisontell retrace
void putpixel (unsigned int x, unsigned int y, unsigned char col, unsigned char *where)
void drawbox (unsigned int x, unsigned int y, char xsize, char ysize, unsigned char picture[], unsigned char *where)
void drawbox (unsigned int x, unsigned int y, char xsize, char ysize, unsigned char picture[], unsigned char *where, char) - Skriver runt hela sk„rmen, ocks† svart.
void setpal (unsigned int col, unsigned char r,unsigned char g,unsigned char b)
void read_pal(char *filnamn) - L„ser paletten "filnamn"

*/
/*-----------------------------Konstanter-------------------------------*/
const	SCREENW		= 320,
		SCREENH		= 200;

/*--------------------------Globala variabler---------------------------*/

static unsigned char *vga = (unsigned char *) MK_FP(0xA000, 0);

/*-----------------------------Funktioner------------------------------*/

void setmode (int mode) {
	asm {
		push ax
		mov ax, mode
		int 0x10
		pop ax
	}
}

void cls(unsigned char col, unsigned char *where, unsigned long size) {
  _fmemset(where, col, size);
}

void retrace() {
	_DX = 0x03DA;
	l1: asm {
	in  al,dx;
	and al,0x08;
	jnz l1;
 }

	l2: asm {
	in  al,dx;
	and al,0x08;
	jz  l2;
			}
}

inline void putpixel (unsigned int x, unsigned int y, unsigned char col, unsigned char *where) {
	_fmemset(where+x+(y*SCREENW),col,1);
}

unsigned char getpixel (unsigned int x, unsigned int y, unsigned char *where) {
	unsigned char temp;
	_fmemcpy (&temp,where+x+(y*SCREENW),1);
	return (temp);
}

void drawbox (unsigned int x, unsigned int y, char xsize, char ysize, unsigned char *picture, unsigned char *where) {
	int i,j;
	for (i = 0; i <= xsize-1; i++) for (j = 0; j <= ysize-1; j++) if (*(picture+(j*xsize)+i) != 0 && (i+x < SCREENW) && (j+y < SCREENH-40)) putpixel (x+i,y+j, *(picture+(j*xsize)+i), where);
}

void drawbox (unsigned int x, unsigned int y, char xsize, char ysize, unsigned char *picture, unsigned char *where, char) {
	int i,j;
	for (i = 0; i <= xsize-1; i++) for (j = 0; j <= ysize-1; j++) putpixel (x+i,y+j, *(picture+(j*xsize)+i), where);
}

void setpal (unsigned int col, unsigned char r,unsigned char g,unsigned char b) {
 outportb (0x3c8, col);
 outportb (0x3c9, r);
 outportb (0x3c9, g);
 outportb (0x3c9, b);
}

char read_pal(char *filnamn) {
	unsigned int i;
	unsigned int col = 3;
	unsigned char *pal = new unsigned char[768];
	FILE *handle;
	if ((handle = fopen (filnamn, "rb")) == NULL) return 1;
	fread (pal,1,768, handle);
	for (i = 1; i <= 255; i++) {
		setpal (i, pal[col], pal[col+1], pal[col+2]);
		col += 3;
	}
	fclose (handle);
	return 0;
}
