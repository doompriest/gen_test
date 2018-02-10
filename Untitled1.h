#pragma once

// ширина мира
#define W_WIDTH 128
// высота мира
#define W_HEIGHT 128
#define W_LEFT 20
// отступ от верхнего края до карты
#define W_TOP 20


// код пустоты
#define X_NONE 5
// код бота
#define X_BOT 3 
// код стены
#define X_WALL 2
// код еды
#define X_FOOD 4
// код яда
#define X_POISON 1
// код лавы
#define X_LAVA 6
// код ботов-пищи
#define X_EDIBLE 7

static int W_DELAY = 8;
static int MAX_STEP = 16;
static int MAX_HEALTH = 48;
static int MAX_LIFE = 65536;
static unsigned int A_SIZE = 8;
static unsigned int C_SIZE = 64;
static unsigned int P_SIZE = 32;
static unsigned int E_SIZE = 0;
static unsigned int F_SIZE = 512;
static unsigned int S_SIZE = 512;
static unsigned int L_SIZE = 0;
static unsigned int DEGRADAT = 3/4;

static int DRAW_MODE = 1; // флаг отрисовки
static time_t TIMER, INIT_TIMER;
static int World[W_HEIGHT][W_WIDTH];

using namespace std;

// перевод всего в строку
template <class T>
string to_string(T param)
{
    string str = "";
    stringstream ss;
    ss<<param;
    getline(ss, str);
    return str;
}


// координата X по направлению
int get_x(int x, int dir)
{
	int res = x;
	if (dir == 0 || dir == 6 || dir == 7)
		res--;
	if (dir == 2 || dir == 3 || dir == 4)
		res++;
	return res;
}

// координата Y по направлению
int get_y(int y, int dir)
{
	int res = y;
	if (dir == 0 || dir == 1 || dir == 2)
		res--;
	if (dir == 4 || dir == 5 || dir == 6)
		res++;
	return res;
}


// КЛАСС ЕДЫ
class Food {
	public:
		int x, y;
		Food(int x, int y);		
		void draw();
};

Food::Food(int x, int y)
{
	this->x = x;
	this->y = y;
	World[y][x] = X_FOOD;
	if (DRAW_MODE == 1) {
		setfillstyle(1, GREEN);
		this->draw();
	}
}

void Food::draw()
{
	bar(this->x * A_SIZE + W_LEFT, this->y * A_SIZE + W_TOP, this->x * A_SIZE + A_SIZE + W_LEFT, this->y * A_SIZE + A_SIZE + W_TOP);
}

// КЛАСС ЯДА
class Poison {
	public:
		int x, y;
		Poison(int x, int y);		
		void draw();
};

Poison::Poison(int x, int y)
{
	this->x = x;
	this->y = y;
	World[y][x] = X_POISON;
	if (DRAW_MODE == 1) {
		setfillstyle(1, RED);
		this->draw();
	}
}

void Poison::draw()
{
	bar(this->x * A_SIZE + W_LEFT, this->y * A_SIZE + W_TOP, this->x * A_SIZE + A_SIZE + W_LEFT, this->y * A_SIZE + A_SIZE + W_TOP);
}

// КЛАСС ЛАВЫ
class Lava {
	public:
		int x, y;
		Lava(int x, int y);		
		void draw();
};

Lava::Lava(int x, int y)
{
	this->x = x;
	this->y = y;
	World[y][x] = X_LAVA;
	if (DRAW_MODE == 1) {
		setfillstyle(1, MAGENTA);
		this->draw();
	}
}

void Lava::draw()
{
	bar(this->x * A_SIZE + W_LEFT, this->y * A_SIZE + W_TOP, this->x * A_SIZE + A_SIZE + W_LEFT, this->y * A_SIZE + A_SIZE + W_TOP);
}


// КЛАСС БОТА
class Actor {
	
	public:
		
		int x, y;
		unsigned int pointer;
		int mutant;
		int edible;
		
		int health, max_health;
		int *commands;
		int direction;
		
		Actor(int x, int y, int edible);
		Actor(Actor &parent, int no, int val);
		void work();
		void move(int direction);
		void see(int direction);
		void use(int direction);
		void pointer_increase(int step);
		void react(int value);
		void turn(int direction);
		void draw();
		~Actor();
		
};

Actor::Actor(int x, int y, int edible)
{
	this->x = x;
	this->y = y;
	this->mutant = 0;
	this->edible = edible;

	this->pointer = 0;
	this->max_health = rand() % MAX_HEALTH + 1;
	if (this->max_health < MAX_HEALTH / 2)
		this->max_health = MAX_HEALTH / 2;
	this->health = this->max_health;
	this->direction = rand() % 8;
	
	this->commands = new int[C_SIZE];
	
	for (unsigned int i = 0; i < C_SIZE; i++) {
		this->commands[i] = rand() % 16 + 16;//C_SIZE - i;//rand() % 16;//rand() % (C_SIZE / 2);
	}
	
	
	if (this->edible == 0) {
		World[this->y][this->x] = X_BOT;
	} else {
		World[this->y][this->x] = X_EDIBLE;
	}
	
	/*
	this->commands[0] = 9;
	this->commands[1] = 5;
	this->commands[2] = 1;
	this->commands[3] = 29;
	this->commands[4] = 1;
	this->commands[5] = 1;
	this->commands[6] = 14;
	this->commands[7] = 25;
	this->commands[8] = 24;
	this->commands[10] = 22;
	*/
	//cout << "DIRECTION:  "  << this->direction << endl; 
}

Actor::Actor(Actor &parent, int no, int val)
{
	//cout << "ENTER\n";
	
	
	do {
		this->x = rand() % (W_WIDTH - 1) + 1;
		this->y = rand() % (W_HEIGHT - 1) + 1;
	} while(World[this->y][this->x] != X_NONE);
	//cout << "    COORDS\n";
	this->pointer = 0;
	this->max_health = parent.max_health * DEGRADAT;//rand() % (parent.max_health) + 1;
	if (this->max_health < MAX_HEALTH / 2)
		this->max_health = MAX_HEALTH / 2;
	this->health = this->max_health;
	this->direction = parent.direction;
	
	this->commands = new int[C_SIZE];
	for (unsigned int i = 0; i < C_SIZE; i++) {
		this->commands[i] = parent.commands[i];
	}	
	this->mutant = 0;

	if (no >= 0 && val >= 0) {
		this->commands[no] = val;
		this->mutant = 1;
		//this->direction = rand() % 4;
		
		//for (int i = 0; i < C_SIZE; i++) {
		//	this->commands[i] = rand() % C_SIZE;
		//}	
	}

	this->edible = parent.edible;
	if (this->edible == 0)
		World[this->y][this->x] = X_BOT;
	else
		World[this->y][this->x] = X_EDIBLE;
}

void Actor::pointer_increase(int step) 
{
	this->pointer += step;
	if (this->pointer >= C_SIZE)
		this->pointer %= C_SIZE;
}

void Actor::react(int value)
{
	this->pointer_increase(value);
}


void Actor::draw()
{	
	bar(this->x * A_SIZE + W_LEFT, this->y * A_SIZE + W_TOP, this->x * A_SIZE + A_SIZE + W_LEFT, this->y * A_SIZE + A_SIZE + W_TOP);	
}

void Actor::move(int direction)
{

	int dir = direction + this->direction - 1;
	if (dir < 0)
		dir += 8;
	if (dir > 7)
		dir -= 8;
	
	int	tmp = World[get_y(this->y, dir)][get_x(this->x, dir)];
	
	if (tmp != X_WALL && tmp != X_BOT && tmp != X_EDIBLE) {
		World[this->y][this->x] = X_NONE;
		this->x = get_x(this->x, dir);
		this->y = get_y(this->y, dir);
	}

	this->react(tmp);
		
	if (World[this->y][this->x] == X_LAVA)
		this->health = 0;
	if (World[this->y][this->x] == X_POISON)
		this->health = 0;	
	if (World[this->y][this->x] == X_FOOD) {
		if (this->edible == 0)
			this->health += MAX_HEALTH / 2;
		else
			this->health += MAX_HEALTH;
		}
		
	if (this->edible == 0)
		World[this->y][this->x] = X_BOT;
	else
		World[this->y][this->x] = X_EDIBLE;
}

void Actor::use(int direction)
{

	int dir = direction + this->direction - 1;
	if (dir < 0)
		dir += 8;
	if (dir > 7)
		dir -= 8;
	
	int tmp = World[get_y(this->y, dir)][get_x(this->x, dir)];
	
	switch(tmp) {
		case X_FOOD: {
			World[get_y(this->y, dir)][get_x(this->x, dir)] = X_NONE;
			if (this->edible == 0) // хищник
				this->health += MAX_HEALTH / 2;
			else
				this->health += MAX_HEALTH;
		} break;
		case X_POISON: {
			World[get_y(this->y, dir)][get_x(this->x, dir)] = X_NONE;
			if (this->edible == 0)
				this->health += MAX_HEALTH / 4;
			else
				this->health += MAX_HEALTH / 2;
		} break;
		case X_EDIBLE: {
			if (this->edible == 0) { // еда сама себя не кушает
				World[get_y(this->y, dir)][get_x(this->x, dir)] = X_NONE;
				this->health += MAX_HEALTH * 8;
			}
		} break;
		case X_LAVA: this->health -= MAX_HEALTH / 2; break;
	}
	this->react(World[get_y(this->y, dir)][get_x(this->x, dir)]);
	
	if (this->health > this->max_health)
		this->max_health = this->health;
}

void Actor::see(int direction)
{
	int dir = direction + this->direction - 1;
	if (dir < 0)
		dir += 8;
	if (dir > 7)
		dir -= 8;
	
	this->react(World[get_y(this->y, dir)][get_x(this->x, dir)]);

}

void Actor::turn(int direction)
{
	int dir = direction + this->direction - 1;
	if (dir < 0)
		dir += 8;
	if (dir > 7)
		dir -= 8;
	
	this->direction = dir;
	this->pointer_increase(1);
}

void Actor::work()
{
	int count = 0;

	while(true) {
		
		if (DRAW_MODE == 1) {
			setfillstyle(1, 0);
			this->draw();
		}
		
		if (this->health <= 0) {
			World[this->y][this->x] = X_NONE;
			//break;
			return;
		}
		if (count >= MAX_STEP)
			break;

		//cout << this->pointer << endl;
		
		if (this->commands[this->pointer] >= 0 && this->commands[this->pointer] < 8) {			
			this->move(this->commands[this->pointer]);
			count = MAX_STEP;
		}
		
		if (this->commands[this->pointer] >= 8 && this->commands[this->pointer] < 16) {
			this->use(this->commands[this->pointer] % 8);
			count = MAX_STEP;
		}
		
		if (this->commands[this->pointer] >= 16 && this->commands[this->pointer] < 24) {
			this->see(this->commands[this->pointer] % 8);
		}
		
		if (this->commands[this->pointer] >= 24 && this->commands[this->pointer] < 32) {
			this->turn(this->commands[this->pointer] % 8); 
		}
		
		if (this->commands[this->pointer] >= 32) {
			this->pointer_increase(this->commands[this->pointer]);
		}


		count++;
		this->health--;
	}
	
	if (DRAW_MODE == 1) {
		if (this->mutant == 0) {
			if (this->edible == 0)
				setfillstyle(1, WHITE);
			else
				setfillstyle(1, CYAN);
			}
		else {
			if (this->edible == 0)
				setfillstyle(1, YELLOW); 
			else
				setfillstyle(1, LIGHTCYAN);
		}
		this->draw();	
	}
	
	
}

Actor::~Actor()
{
	delete this->commands;
}
