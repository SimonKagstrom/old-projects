/*
Hanterar flera samtidiga tangnettryck.
Konstanter:
			UP	 			= 72,
			DOWN 			= 80,
			RIGHT			= 77,
			LEFT 			= 75,
			CTRL 			= 29,
			ESC			=	1;

Variabler:
unsigned char port60h, port61h, port20h - h†rdvaruportar f”r tangent-kontroll.
unsigned char keydown[128] - om tangent x nedtryckt keydown[x] == 1
oldkbdint - sparar adressen f”r den ersatta tangent-interrupten

Funktioner:
void interrupt ( *oldkbdint)(__CPPARGS)
void interrupt handler(__CPPARGS) - Skapar ny tangentbords-hanterare.
void flushkbd() - Rensar tangentbuffern.
*/

#define INTR 0X09 // Tangentbords-interrupten.
#ifdef __cplusplus
	#define __CPPARGS ...
 #else
	#define __CPPARGS
#endif

//-------------------------Konstanter-------------------------

const		UP	 			= 72, // Olika tangentkoder
			DOWN 			= 80,
			RIGHT			= 77,
			LEFT 			= 75,
			CTRL 			= 29,
			ESC			=	1;

//-------------------------Variabler-------------------------

unsigned char port60h, port61h, port20h;
unsigned char keydown[128];

//-------------------------Funktioner-------------------------

void interrupt (*oldkbdint)(__CPPARGS);

void interrupt handler(__CPPARGS) {//(__CPPARGS)
 port60h = inportb(0x60);
 keydown[port60h & 127] = (port60h <= 127);
 port61h = inportb(0x61);
 outportb(0x61, (port61h | 128));
 outportb(0x61, port61h);
 port20h = 0x20;
 outportb (port20h,0x20);
}

void flushkbd()
{
	asm {
		mov ax, 0xc00
		int 0x21
	}
}