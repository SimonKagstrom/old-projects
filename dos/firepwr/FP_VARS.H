/*--------------------------Konstanter------------------------------*/

const		MAX_MAP_OBJECTS 	=1000,
			MAX_MAPW				=	20, // Kartans h”jd resp. bredd...
			MAX_MAPH				=	20;

/*--------------------------Strukturer------------------------------*/

struct 	map_object_struct
			{
				int xpos;
				int ypos;
				char type;
				char power;
				char forward_speed;
				char reverse_speed;
				char turnspeed;
				char direction;
				struct map_object_struct *next;
				struct map_object_struct *prev;
			};

/*---------------------------Variabler------------------------------*/

unsigned int SPRITEW = 30; // Spritarnas bredd...
unsigned int SPRITEH = 30; // ... Och vidd...
unsigned int BACKGRW	= 40; // ... Samma f”r bakgrunden...
unsigned int BACKGRH	= 40;
unsigned int SMALLOBJW = 22; // ... Samma f”r bakgrunden...
unsigned int SMALLOBJH = 20;
unsigned long BACKGR_OFF = 0; // Kvasikonstanter.
unsigned long SMALLOBJ_OFF = 0;

unsigned int obj_pic_nr;
unsigned int backgr_pic_nr;
unsigned int small_pic_nr;
unsigned char map[MAX_MAPW*MAX_MAPH];
struct map_object_struct *Pmapobj = NULL; // R”rlig nodpekare
struct map_object_struct *Pmapobj_start = NULL; // Pekar till startnoden

