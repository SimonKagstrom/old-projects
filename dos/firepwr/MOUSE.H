/*---------------------------Strukturer-----------------------------*/
struct mstruct {
				unsigned int buttons,xpos,ypos;
			 } mouse;

/*---------------------------Funktioner-----------------------------*/

void show_mouse2 () {
	asm {
		mov ax, 0x20
		int 0x33
		mov ax, 0x07
		mov ax, 0x02
		int 0x33
		mov cx, 0
		mov dx, 0
		int 0x33
		mov ax, 0x008
		mov cx, 0
		mov dx, 280
		int 0x33
		mov ax, 0x00f
		mov cx, 50
		mov dx, 50
		int 0x33
		mov ax, 0x013
		mov dx, 30000
		int 0x33
	}
}

void show_mouse () {
	asm {
	mov ax, 0x0000
	int 0x33
	mov ax, 0x001a // Pekarens snabbhet
	mov bx,5
	mov cx,5
	mov dx,10000
	int 0x33
	mov ax, 0x0007
	mov cx, 0
	mov dx, 290
	int 0x33
	mov ax, 0x0008
	mov cx, 0
	mov dx, 140
	int 0x33
	mov ax, 0x0004 // Initial-position
	mov cx, 0
	mov dx, 0
	int 0x33
	}
}

void remove_mouse () {
	asm {
		mov ax, 0x1f
		int 0x33
	}
}

void mouse_status () {
	unsigned int temp,temp2,temp3;
	asm {
		mov ax, 0x03
		int 0x33
		mov temp, bx
		mov temp2, cx
		mov temp3, dx
	}
	mouse.buttons = temp;
	mouse.xpos = temp2;
	mouse.ypos = temp3;
}

void mouse_status (int x, int y, int buttons) {
	unsigned int temp,temp2,temp3;
	asm {
		mov ax, 0x03
		int 0x33
		mov buttons, bx
		mov x, cx
		mov y, dx
	}
}

