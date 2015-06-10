/*
Firepower-klasser
-----------------
class Thing - Allm„n klass f”r allt p† kartan.
class Vehicle : public Thing - Allt som r”r sig.
class Turret : public Vehicle - Kanontornet.
class Tank : public Vehicle - Allt som skjuter.

Styr genom input(unsigned char key[128]), eller:
			  input(unsigned char key)
UP	 			= 72,
DOWN 			= 80,
RIGHT			= 77,
LEFT 			= 75,
CTRL 			= 29;

(G„ller Vehicle)

... Humm.. Buggar l„tt ur, tangentbordet svarar inte...
(Verkar l”st med konstrukt”ren)...
*/

/*----------------------------Objekt--------------------------------*/

/*----------------------------Thing---------------------------------*/

class Thing {
	public:
		Thing () // Konstruktor. Om variablerna inte initieras buggar det mesta...
		{
			xpos = 0;
			ypos = 0;
			power = 10;
			type = 0;
			next = NULL;
			prev = NULL;
		};
		int xpos;					  // Objektets x-position (p† banan)
		int ypos;					  // Objektets y-position
		int type;
		signed char power;
		Thing *next;
		Thing *prev;
		~Thing () {}; // Destruktor
};

/*---------------------------Vehicle--------------------------------*/

class Vehicle : public Thing {
	protected:
		signed char heading;
		signed char turnspeed;
		unsigned char one_tick;
		unsigned char drive_time;
		signed char has_turned;
		signed char speed;
		char time_handler (char &timer, char time_determ);
		void move_tank (signed char xmod, signed char ymod);
	public:
		Vehicle () // Konstruktor
		{
			forward_speed = 0;
			reverse_speed = 0;
			direction = 0;
			heading = 0;
			turnspeed = 8;
			one_tick = 0;
			drive_time = 0;
			has_turned = 0;
			speed = 1;
			next = NULL;
			prev = NULL;
		};
		signed char forward_speed;// Maxfart fram†t (Dessa tv† borde kunna g”ras protected senare).
		signed char reverse_speed;// Maxfart bak†t
		signed char direction;	  // Vilken riktning tanken „r p† v„g †t
		void determine_dir (signed char *direction, signed char heading, signed char *xmod, signed char *ymod);
		void input (unsigned char key[128]);
		void input (unsigned char key);
		Vehicle *next;
		Vehicle *prev;
		~Vehicle () {};
};

void Vehicle :: move_tank (signed char xmod, signed char ymod)
{
	if (heading == 1) speed = forward_speed;
	if (heading == -1) speed = reverse_speed;
	if (xpos+xmod > 0 && xpos+xmod < MAX_MAPW*BACKGRW-SCREENW) xpos += xmod;
	if (ypos+ymod > 0 && ypos+ymod < MAX_MAPH*BACKGRH-SCREENH+BACKGRH) ypos += ymod;
};

char Vehicle :: time_handler (char &timer, char time_determ)
{
	if (one_tick != gettime())
	{
		timer++;
		if (has_turned) has_turned--;
		one_tick = gettime();
	}
	if (timer >= time_determ) {timer = 0; return (1);}
	return (0);
};

void Vehicle :: determine_dir (signed char *direction, signed char heading, signed char *xmod, signed char *ymod)
{
	if (*direction < 0) *direction = 8+*direction;
	if (*direction == 8 || *direction < 0) *direction = 0;
	switch (*direction)
	{
		case 0 : *ymod = -heading; *xmod = 	 		0; break; // Rakt upp
		case 1 : *ymod = -heading; *xmod =  heading; break; // Upp h”ger
		case 2 : *ymod =    	   0; *xmod = 	heading; break; // H”ger
		case 3 : *ymod =  heading; *xmod = 	heading; break; // Ner h”ger
		case 4 : *ymod =  heading; *xmod = 			0; break; // Rakt ner
		case 5 : *ymod =  heading; *xmod = -heading; break; // Ner v„nster
		case 6 : *ymod =    		0; *xmod = -heading; break; // V„nster
		case 7 : *ymod = -heading; *xmod = -heading; break; // Upp v„nster
	}
};

void Vehicle :: input (unsigned char key[128])  // "Interface"-funktion. Fr der Spieler
{
	signed char ymod, xmod;
	if (key[UP]) heading = 1;
	if (key[DOWN]) heading = -1;
	if (!key[UP] && !key[DOWN]) heading = 0;
	if (!has_turned)
	{
		if (key[LEFT]) {direction--; has_turned = turnspeed;}
		if (key[RIGHT]) {direction++; has_turned = turnspeed;}
	}
	determine_dir (&direction, heading, &xmod, &ymod);
	if (time_handler(drive_time,speed)) move_tank (xmod,ymod);
};

void Vehicle :: input (unsigned char key)  // "Interface"-funktion. F”r "fiender" och annat. F†r inte ha mer „n en knapp nedtryckt samtidigt. Problem?
{
	signed char ymod, xmod;
	if (key == UP) heading = 1;
	if (key == DOWN) heading = -1;
	if (!key == UP && !key == DOWN) heading = 0;
	if (!has_turned)
	{
		if (key == LEFT) {direction--; has_turned = turnspeed;}
		if (key == RIGHT) {direction++; has_turned = turnspeed;}
	}
	determine_dir (&direction, heading, &xmod, &ymod);
	if (time_handler(drive_time, speed)) move_tank (xmod,ymod);
};

/*------------------------------Turret-----------------------------------*/

class Turret : public Vehicle
{
	public:
		Turret ()
		{
			power = 0;
			fire_delay = 6;
			fire_time = fire_delay;
		};
		void determine_sector (int xvalue, int yvalue, int width, int height);
		char fire ();
		int power;
		char fire_delay;
		char fire_time;
		~Turret () {};
};

void Turret :: determine_sector (int xvalue, int yvalue, int width, int height)
{
	signed char xmod = 0;
	signed char ymod = 0;
	if (xvalue < 0) xmod = -1;
	if (xvalue > width) xmod = 1;
	if (yvalue < 0) ymod = -1;
	if (yvalue > height) ymod = 1;
	if (xmod == 0 && (ymod == 0 || ymod == -1)) {	direction = 0;	goto out; }
	if (xmod == 1 && ymod == -1) {	direction = 1;	goto out; }
	if (xmod == 1 && ymod == 0) { direction = 2; goto out; }
	if (xmod == 1 && ymod == 1) {	direction = 3;	goto out; }
	if (xmod == 0 && (ymod == 0 || ymod == 1)) {	direction = 4;	goto out; }
	if (xmod == -1 && ymod == 1) {	direction = 5;	goto out; }
	if (xmod == -1 && ymod == 0) { direction = 6; goto out; }
	if (xmod == -1 && ymod == -1) {	direction = 7;	goto out; }
	out:
};

char Turret :: fire () // Buggar litegrann.
{
	if (time_handler (fire_time, fire_delay))
	{
		fire_time = 0;
		return 1;
	}
	else return 0;
}

/*-------------------------------Tank------------------------------------*/

class Tank : public Vehicle // init_vars borde kanske g”ras till konstrukt”r?
{
	protected:
		unsigned char ammo;
	public:
		Tank () // Konstruktor
		{
			ammo = 0;
			Pturret = NULL;
			next = NULL;
			prev = NULL;
		};
		friend void init_vars (Tank *obj,int forward_speed, int reverse_speed,int turnspeed,int xpos,int ypos,signed char direction, signed char heading, char power, char ammo);
		Turret *Pturret;
		Tank *next; // L„nkad lista?
		Tank *prev;
		~Tank () {}; // Destruktor
};

/*----------------------------Funktioner--------------------------------*/

void init_vars (Tank *obj,int forward_speed, int reverse_speed,int turnspeed,int xpos,int ypos,signed char direction, signed char heading, char power, char ammo) // G”r om till konstrukt”r sedan.
{
	obj->turnspeed = turnspeed;
	obj->forward_speed = forward_speed;
	obj->reverse_speed = reverse_speed;
	obj->xpos = xpos;
	obj->ypos = ypos;
	obj->direction = direction;
	obj->heading = heading;
	obj->power = power;
	obj->ammo = ammo;
};
