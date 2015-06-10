// Tecknen p† sk„rmen f”ljer muspekaren! P†b”rjat 96-08-16, p† kv„llen.

#include "stdio.h"
#include "conio.h"

struct mstruct {
				unsigned int buttons,xpos,ypos;
			 } mouse;
int		 flwdelay = 1; // Hur "snabbt" tecknen ska reagera

char screen[2][4000];
unsigned char attrib;

void showmouse () {
 asm {
	mov ax, 0x20
	int 0x33
	mov ax, 0x07
	mov cx, 16
	mov dx, 624
	int 0x33
	mov ax, 0x08
	mov cx, 16
	mov dx, 384
	int 0x33
	mov ax, 0x02
	int 0x33
 }
}

void removemouse () {
 asm {
	mov ax, 0x1f
	int 0x33
 }
}

void mousepos () {
 unsigned int temp,temp2,temp3;
 asm {
	mov ax, 0x03
	int 0x33
	mov temp, bx
	mov temp2, cx
	mov temp3, dx
 }
 mouse.buttons = temp;
 mouse.xpos = temp2 / 8;
 mouse.ypos = temp3 / 8;
}

void ch_put (int pos, char bokst) {
 asm {
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
 asm {
	mov bx, pos
	shl bx, 1
	mov ax, 0xb800
	mov es, ax
	mov ax, [es:bx]
	mov bokst, al
	mov attrib, ah
 }
 return (bokst);
}

void initialize () {
 int i;
 showmouse ();
 for (i = 0; i <= 3999; i++) {
	screen[1][i] = 0;
	screen[2][i] = 0;
 }
 for (i = 0; i <= 3999; i++) {
	screen[1][i] = ch_get (i);
	screen[2][i] = attrib;
 }
 clrscr ();
 for (i = 0; i <= 3999; i++) {
	attrib = screen[2][i];
	ch_put(i,screen[1][i]);
 }
 mouse.xpos = 40;
 mouse.ypos = 25;
}

void followmouse () {
 int x,y,i,counter,diff[2];
 while (!kbhit()) {
	mousepos ();
	for (y = 0; y <= 49; y++) for (x = 0; x <= 79; x++) {
	 attrib = screen[2][y*80+x];
	 if (screen[1][y*80+x] == 32) attrib = 0;
	 ch_put (y*80+x,screen[1][y*80+x]);
	}
	counter++;
	if (counter > flwdelay) {
	 for (y = 0; y <= 49; y++) for (x = 0; x <= 79; x++) {
		if (x > mouse.xpos) diff[1] = x - mouse.xpos;
		else diff[1] = mouse.xpos - x;
		if (y > mouse.ypos) diff[2] = y - mouse.ypos;
		else diff[2] = mouse.ypos - y;
		if ((diff[1] > diff[2]) && ((screen[1][y*80+(x+1)] == 32) || (screen[1][y*80+(x-1)] == 32))) {
			if ((screen[1][y*80+x] != 32) && (x >= mouse.xpos) && (screen[1][y*80+(x-1)] == 32)) {
				for (i = 1; i <= 2; i++) {
					screen[i][y*80+(x-1)] = screen[i][y*80+x];
					screen[i][y*80+x] = 32;
				}
			}
			if ((screen[1][y*80+x] != 32) && (x <= mouse.xpos) && (screen[1][y*80+(x+1)] == 32)) {
				for (i = 1; i <= 2; i++) {
					screen[i][y*80+(x+1)] = screen[i][y*80+x];
					screen[i][y*80+x] = 32;
				}
				x++;
			}
		} else if ((screen[1][(y+1)*80+x] == 32) || (screen[1][(y-1)*80+x] == 32)) {
			if ((screen[1][y*80+x] != 32) && (y <= mouse.ypos) && (screen[1][(y+1)*80+x] == 32)) {
				for (i = 1; i <= 2; i++) {
					screen[i][(y+1)*80+x] = screen[i][y*80+x];
					screen[i][y*80+x] = 32;
				}
				y++;
			}
			if ((screen[1][y*80+x] != 32) && (y >= mouse.ypos) && (screen[1][(y-1)*80+x] == 32)) {
				for (i = 1; i <= 2; i++) {
					screen[i][(y-1)*80+x] = screen[i][y*80+x];
					screen[i][y*80+x] = 32;
				}
			}
		}
	 }
	counter = 0;
	}
	attrib = 12;
	ch_put (mouse.ypos*80+mouse.xpos,'þ');
 }
}

int main () {
 text_info ti;
 gettextinfo (&ti);
 if (ti.screenheight != 50) {
	printf ("Du m†ste k”ra detta program i 50-linjers l„ge!\n");
	return (10);
 }
 initialize ();
 followmouse ();
 removemouse ();
 return (0);
}
