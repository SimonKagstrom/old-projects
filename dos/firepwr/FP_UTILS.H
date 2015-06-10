/* Gemensamma funktioner f”r Fire Power och Makemap
	Variabler:
	unsinged int obj_pic_nr		-	antalet objekt-bilder
	unsinged int backgr_pic_nr	-	antalet bakgrunds-bilder
	unsigned char map[MAX_MAPW*MAX_MAPH]; - kartan.

	Funktioner:
	int diff (int value)
	char read_gfx (unsigned char obj_pics[][SPRITEW*SPRITEH], unsigned char bgr_pics[][BACKGRW*BACKGRH], char spritename[], char backgrname[])
	map_object_struct *newnode (struct map_object_struct *Pin, int xpos, int ypos, char type, char power, char forward_speed, char reverse_speed, char direction)
	char write_map (char filename[])
	char read_map (char filename[])
*/

/*--------------------------Funktioner------------------------------*/
inline int diff (int value) {if (value < 0) return (-value); else return (value);}

char read_gfx (unsigned char *obj_pics, char spritename[], char backgrname[], char smallobjname[]) {
	int i,j;
	FILE *handle;
	if ((handle = fopen (spritename, "rb")) == NULL) return 1;
	fread (&obj_pic_nr, sizeof(obj_pic_nr), 1, handle);
	fread (&SPRITEW, sizeof(SPRITEW),1,handle); // L„ser in sprajtarnas bredd resp. h”jd.
	fread (&SPRITEH, sizeof(SPRITEH),1,handle);
	fread (obj_pics, obj_pic_nr*SPRITEW*SPRITEH,1,handle); // L„ser in sprajtgrafiken...
	fclose (handle);
	if ((handle = fopen (backgrname, "rb")) == NULL) return 1;
	fread (&backgr_pic_nr, sizeof(backgr_pic_nr), 1, handle);
	fread (&BACKGRW, sizeof(BACKGRW),1,handle); // L„ser in bakgrundens bredd resp. h”jd.
	fread (&BACKGRH, sizeof(BACKGRH),1,handle);
	BACKGR_OFF = obj_pic_nr*SPRITEW*SPRITEH;
	fread (obj_pics+BACKGR_OFF,backgr_pic_nr*BACKGRW*BACKGRH,1,handle); // L„ser in bakgrundsgrafiken...
	fclose (handle);
	if ((handle = fopen (smallobjname, "rb")) == NULL) return 1;
	fread (&small_pic_nr, sizeof(small_pic_nr), 1, handle);
	fread (&SMALLOBJW, sizeof(SMALLOBJW),1,handle); // L„ser in sm†objektens bredd resp. h”jd.
	fread (&SMALLOBJH, sizeof(SMALLOBJH),1,handle);
	SMALLOBJ_OFF = obj_pic_nr*SPRITEW*SPRITEH+backgr_pic_nr*BACKGRW*BACKGRH;
	fread (obj_pics+SMALLOBJ_OFF,small_pic_nr*SMALLOBJW*SMALLOBJH,1,handle); // L„ser in bakgrundsgrafiken...
	fclose (handle);
	return 0;
}

char show_gfx_file (unsigned char filename[], int x, int y, unsigned char *where)
{
	FILE *handle;
	int sizex;
	int sizey;
	if ((handle = fopen (filename, "rb")) == NULL) return 1;
	fseek (handle,2,SEEK_SET);
	fread (&sizex, sizeof(sizex),1,handle);
	fread (&sizey, sizeof(sizey),1,handle);
	fread (where+y*SCREENW+x, sizey*SCREENW+sizex, 1, handle);
	fclose (handle);
	return 0;
}

struct map_object_struct *newnode (struct map_object_struct *Pin, int xpos, int ypos, char type, char power, char forward_speed, char reverse_speed, char direction) {
	struct map_object_struct *Ptemp;
	struct map_object_struct *Pprev_temp;
	Ptemp = Pin;
	Ptemp->next = new map_object_struct;
	Pprev_temp = Ptemp;
	Ptemp = Ptemp->next;
	Ptemp->prev = Pprev_temp;
	Ptemp->xpos = xpos;
	Ptemp->ypos = ypos;
	Ptemp->type = type;
	Ptemp->power = power;
	Ptemp->forward_speed = forward_speed;
	Ptemp->reverse_speed = reverse_speed;
	Ptemp->direction = direction;
	Ptemp->next = NULL;
	return (Ptemp);
}

char delete_obj (class Tank *Pstart, class Tank *Pin)
{
	char retcode = 0;
	if (&Pin == &Pstart)
	{
		Pstart = Pin->next;
		Pstart->prev = NULL;
	}
	else
	{
		if (Pin->prev != NULL) Pin->next->prev = Pin->prev;
		else Pin->next->prev = NULL;
		if (Pin->next != NULL) Pin->prev->next = Pin->next;
		else Pin->prev->next = NULL;
	}
	delete (Pin);
	return (retcode);
}

char write_map (char filename[]) {
	FILE *handle;
	struct map_object_struct *Ptemp = NULL;
	if ((handle = fopen (filename, "wb")) == NULL) return 1;
	fwrite (map,sizeof(map),1,handle);
	Ptemp = Pmapobj_start;
	while (Ptemp != NULL) { // Skriv objekt-noderna.
		fwrite (Ptemp, sizeof (map_object_struct), 1, handle);
		Ptemp = Ptemp->next;
	};
	Ptemp->type = 255; // Kontrollnoden.
	fwrite (Ptemp, sizeof (map_object_struct), 1, handle);
	fclose (handle);
	return 0;
}

char read_map (char filename[]) {
	FILE *handle;
	struct map_object_struct *Ptemp = new map_object_struct;
	struct map_object_struct *Pprev_temp = Ptemp;
	if ((handle = fopen (filename, "rb")) == NULL) return 1;
	fread (map, sizeof(map),1,handle); // L„ser in sj„lva kartan.
	fread (Ptemp, sizeof(map_object_struct),1,handle); // L„ser in f”rsta noden och lagrar adressen i Pmapobj_start
	Ptemp->next = new map_object_struct;
	Pmapobj_start = Ptemp;
	Pmapobj_start->prev = NULL;
	while (Ptemp->type != 255) { // L„ser in resten av noderna...
		Pprev_temp = Ptemp;
		Ptemp = Ptemp->next;
		fread (Ptemp, sizeof (map_object_struct), 1, handle);
		Ptemp->prev = Pprev_temp;
		Ptemp->next = new map_object_struct;
	}
//	Ptemp = Ptemp->prev;
//	delete (Ptemp->next); // tar bort kontrollnoden...
	Ptemp->next = NULL; // peka inte p† m†f†!
	fclose (handle);
	return 0;
}

