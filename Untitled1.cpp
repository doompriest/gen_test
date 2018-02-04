#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include <string>
#include <graphics.h>

//количество команд у бота
#define C_SIZE 32
// задержка глобальная
#define W_DELAY 1
// ширина мира
#define W_WIDTH 128
// высота мира
#define W_HEIGHT 96
// размер объекта
#define A_SIZE 8
// отступ от левого края до карты
#define W_LEFT 20
// отступ от верхнего края до карты
#define W_TOP 20
// количество ботов
#define P_SIZE 16
// количество хавки
#define F_SIZE 256
// максимальное количество команд за ход
#define MAX_STEP 32
// базовое здоровье
#define MAX_HEALTH 64
// максимальное здоровье
#define MAX_LIFE 2048
// количество яда
#define S_SIZE 256
// коэффициент деградации
#define DEGRADAT 3/4

// код пустоты
#define X_NONE 0
// код бота
#define X_BOT 1 
// код стены
#define X_WALL -1
// код еды
#define X_FOOD -2
// код яда
#define X_POISON 2

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


static time_t TIMER, INIT_TIMER;
int World[W_HEIGHT][W_WIDTH];

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
	setfillstyle(1, GREEN);
	this->draw();
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
	setfillstyle(1, RED);
	this->draw();
}

void Poison::draw()
{
	bar(this->x * A_SIZE + W_LEFT, this->y * A_SIZE + W_TOP, this->x * A_SIZE + A_SIZE + W_LEFT, this->y * A_SIZE + A_SIZE + W_TOP);
}

// КЛАСС БОТА
class Actor {
	
	public:
		
		int x, y;
		int pointer;
		int mutant;
		
		int health, max_health;
		int commands[C_SIZE];
		int direction;
		
		Actor(int x, int y);
		Actor(Actor &parent, int no, int val);
		void work();
		void move(int direction);
		void see(int direction);
		void use(int direction);
		void pointer_increase(int step);
		void react(int value);
		void turn(int direction);
		void draw();
		
};

Actor::Actor(int x, int y)
{
	this->x = x;
	this->y = y;
	this->mutant = 0;

	this->pointer = 0;
	this->max_health = rand() % MAX_HEALTH + 1;
	if (this->max_health < MAX_HEALTH / 2)
		this->max_health = MAX_HEALTH / 2;
	this->health = this->max_health;
	this->direction = rand() % 4;
	
	for (int i = 0; i < C_SIZE; i++) {
		this->commands[i] = C_SIZE - i;//rand() % 16;//rand() % (C_SIZE / 2);
	}
	
	
	World[this->y][this->x] = X_BOT;
	
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
	
	for (int i = 0; i < C_SIZE; i++) {
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

	World[this->y][this->x] = X_BOT;
}

void Actor::pointer_increase(int step) 
{
	this->pointer += step;
	if (this->pointer >= C_SIZE)
		this->pointer %= C_SIZE;
}

void Actor::react(int value)
{
	switch(value) {
		case X_POISON: this->pointer_increase(1); break;	
		case X_BOT: this->pointer_increase(3); break;	
		case X_NONE: this->pointer_increase(5); break;
		case X_WALL: this->pointer_increase(2); break;
		case X_FOOD: this->pointer_increase(4); break;
		
	}
}


void Actor::draw()
{
	
	bar(this->x * A_SIZE + W_LEFT, this->y * A_SIZE + W_TOP, this->x * A_SIZE + A_SIZE + W_LEFT, this->y * A_SIZE + A_SIZE + W_TOP);
		
}

void Actor::move(int direction)
{

	int delta = (this->direction % 2 == 0) ? 1: -1;
	delta += this->direction;
	direction -= delta;
	if (direction < 0)
		direction += 4;
	
	int tmp = 0, tmp1 = 0;
	
	
	switch(direction) {
		case 0: {
			tmp = World[this->y][this->x - 1];
			if (tmp != X_WALL && tmp != X_BOT) {
				World[this->y][this->x] = X_NONE;
				this->x--;
			}
		}break;
		case 1: {
			tmp = World[this->y - 1][this->x];
			if (tmp != X_WALL && tmp != X_BOT) {
				World[this->y][this->x] = X_NONE;
				this->y--;
			}
		} break;
		case 2: {
			tmp = World[this->y][this->x + 1];
			if (tmp != X_WALL && tmp != X_BOT) {
				World[this->y][this->x] = X_NONE;
				this->x++;
			}
		} break;
		case 3: {
			tmp = World[this->y + 1][this->x];
			if (tmp != X_WALL && tmp != X_BOT) {
				World[this->y][this->x] = X_NONE;
				this->y++;
			}
		}break;
	}
	
	this->react(tmp);
		
	if (World[this->y][this->x] == X_POISON)
		this->health = 0;	
	if (World[this->y][this->x] == X_FOOD)
		this->health += MAX_HEALTH / 2;
	World[this->y][this->x] = X_BOT;
}

void Actor::use(int direction)
{

	int delta = (this->direction % 2 == 0) ? 1: -1;
	delta += this->direction;
	direction -= delta;
	if (direction < 0)
		direction += 4;
	
	switch(direction) {
		case 0: {	
			switch(World[this->y][this->x - 1]) {
				case X_FOOD: {
					World[this->y][this->x - 1] = X_NONE;
					this->health += MAX_HEALTH / 2;
				} break;
				case X_POISON: {
					World[this->y][this->x - 1] = X_NONE;
					this->health += MAX_HEALTH / 2;
				} break;
			}
			this->react(World[this->y][this->x - 1]);
		}break;
		case 1: {
			switch(World[this->y - 1][this->x]) {
				case X_FOOD: {
					World[this->y - 1][this->x] = X_NONE;
					this->health += MAX_HEALTH / 2;
				} break;
				case X_POISON: {
					World[this->y - 1][this->x] = X_NONE;
					this->health += MAX_HEALTH / 2;				
				} break;
			}
			this->react(World[this->y - 1][this->x]);
		} break;
		case 2: {
			switch(World[this->y][this->x + 1]) {
				case X_FOOD: {
					World[this->y][this->x + 1] = X_NONE;
					this->health += MAX_HEALTH / 2;
				} break;
				case X_POISON: {
					World[this->y][this->x + 1] = X_NONE;
					this->health += MAX_HEALTH / 2;
				} break;
			}
			this->react(World[this->y][this->x + 1]);
		} break;
		case 3: {
			switch(World[this->y + 1][this->x]) {
				case X_FOOD: {
					World[this->y + 1][this->x] = X_NONE;
					this->health += MAX_HEALTH / 2;
				} break;
				case X_POISON: {
					World[this->y + 1][this->x] = X_NONE;
					this->health += MAX_HEALTH / 2;				
				} break;
			}
			this->react(World[this->y + 1][this->x]);
		} break;
	}	
	
	if (this->health > this->max_health)
		this->max_health = this->health;
}

void Actor::see(int direction)
{
	int delta = (this->direction % 2 == 0) ? 1: -1;
	delta += this->direction;
	direction -= delta;
	if (direction < 0)
		direction += 4;
	switch(direction) {
		case 0: {
			this->react(World[this->y][this->x - 1]);
			/*
			switch(World[this->y][this->x - 1]) {
				case 0: this->pointer_increase(5); break;
				case -2: this->pointer_increase(4); break;
				case 1: this->pointer_increase(3); break;
			}*/
		}break;
		case 1: {
			this->react(World[this->y - 1][this->x]);
			/*
			switch(World[this->y - 1][this->x]) {
				case 0: this->pointer_increase(5); break;
				case -2: this->pointer_increase(4); break;
				case 1: this->pointer_increase(3); break;
			}*/
		} break;
		case 2: {
			this->react(World[this->y][this->x + 1]);
			/*
			switch(World[this->y][this->x + 1]) {
				case 0: this->pointer_increase(5); break;
				case -2: this->pointer_increase(4); break;
				case 1: this->pointer_increase(3); break;
			}*/
		} break;
		case 3: {
			this->react(World[this->y + 1][this->x]);
			/*
			switch(World[this->y + 1][this->x]) {
				case 0: this->pointer_increase(5); break;
				case -2: this->pointer_increase(4); break;
				case 1: this->pointer_increase(3); break;
			}*/
		} break;
	}	
}

void Actor::turn(int direction)
{
	int delta = (this->direction % 2 == 0) ? 1: -1;
	delta += this->direction;
	direction -= delta;
	if (direction < 0)
		direction += 4;
	this->direction = direction;
	this->pointer_increase(1);
}

void Actor::work()
{
	int count = 0;

	setfillstyle(1, 0);
	this->draw();	

	while(true) {

		if (this->health <= 0) {
			World[this->y][this->x] = X_NONE;
			//break;
			return;
		}
		if (count >= MAX_STEP)
			break;

		//cout << this->pointer << endl;
		
		if (this->commands[this->pointer] < 4 && this->commands[this->pointer] >= 0) {
			this->move(this->commands[this->pointer]);

			count = MAX_STEP;
			
			if (this->mutant == 0)
				setfillstyle(1, WHITE);
			else
				setfillstyle(1, YELLOW); 
			this->draw();	
			
		}
		
		if (this->commands[this->pointer] >= 4 && this->commands[this->pointer] < 8) {
			this->use(this->commands[this->pointer] % 4);
			
			count = MAX_STEP;
		}
		
		if (this->commands[this->pointer] >= 8 && this->commands[this->pointer] < 12) {
			this->see(this->commands[this->pointer] % 4);
		}
		
		if (this->commands[this->pointer] >= 12 && this->commands[this->pointer] < 16) {
			this->turn(this->commands[this->pointer] % 4); 
		}
		
		if (this->commands[this->pointer] >= 16) {
			this->pointer_increase(this->commands[this->pointer]);
		}

		count++;
		this->health--;
	}
	


	
	
}

vector <Actor*> people;
vector <Food*> food;
vector <Poison*> poison;

int main()
{
	
	srand(time(NULL));
	
	int wnd = initwindow(getmaxwidth(), getmaxheight());
   	setcurrentwindow(wnd);
   	
   	for (int i = 0; i < W_HEIGHT; i++) {
   		for (int j = 0; j < W_WIDTH; j++) {
   			World[i][j] = X_NONE;
   			if (j == 0 || j == W_WIDTH - 1)
   				World[i][j] = X_WALL;
   			if (i == 0 || i == W_HEIGHT - 1)
   				World[i][j] = X_WALL;
   			//if (abs(i - W_HEIGHT / 2) <= 8 && abs(j - W_WIDTH / 2) <= 8)
   			//	World[i][j] = X_WALL;
			//if (abs(i - W_HEIGHT / 2) <= 2 && abs(j - W_WIDTH / 2) <= 2)
			if ((abs(i % 32 - 0) <= 6 && abs(j % 32 - 0) <= 6) || (abs(i % 32 - 0) >= 16 && abs(i % 32 - 0) <= 22 && abs(j % 32 - 0) >= 16 && abs(j % 32 - 0) <= 22))
					World[i][j] = X_WALL;
			   				
   			if (World[i][j] == X_WALL) {
   				setfillstyle(1, DARKGRAY);
   				bar(j * A_SIZE + W_LEFT, i * A_SIZE + W_TOP, j * A_SIZE + A_SIZE + W_LEFT, i * A_SIZE + A_SIZE + W_TOP);
   			}
   		}

   	}
   	
   	for (int i = 0; i < F_SIZE; i++) {
		int tx = rand() % (W_WIDTH - 2) + 1, ty = rand() % (W_HEIGHT - 2) + 1;
		while (World[ty][tx] != X_NONE) {
			tx = rand() % (W_WIDTH - 2) + 1;
			ty = rand() % (W_HEIGHT - 2) + 1;
		}
   		food.push_back(new Food(tx, ty)); 
   	}
   	
   	for (int i = 0; i < S_SIZE; i++) {
		int tx = rand() % (W_WIDTH - 2) + 1, ty = rand() % (W_HEIGHT - 2) + 1;
		while (World[ty][tx] != X_NONE) {
			tx = rand() % (W_WIDTH - 2) + 1;
			ty = rand() % (W_HEIGHT - 2) + 1;
		}
   		poison.push_back(new Poison(tx, ty));
   	}
	   
	for (int i = 0; i < P_SIZE; i++) {
		int tx = rand() % (W_WIDTH - 2) + 1, ty = rand() % (W_HEIGHT - 2) + 1;
		while (World[ty][tx] != X_NONE) {
			tx = rand() % (W_WIDTH - 2) + 1;
			ty = rand() % (W_HEIGHT - 2) + 1;
		}
   		people.push_back(new Actor(tx, ty));
   	}
   	
   	
   	
   	time(&INIT_TIMER);
	time(&TIMER);
	int mins = 0, secs = 0;   	
   	
   	auto it = people.begin();
   	auto itf = food.begin();
   	auto itp = poison.begin();
	int gen = 0, step = 0;
	while(true) {
		
		itf = food.begin();
		while (itf != food.end()) {
			if (World[(*itf)->y][(*itf)->x] == X_FOOD) {
				++itf;
			} else {
				itf = food.erase(itf);
			}
		}
		
		if (food.size() < F_SIZE) {
			int diff = F_SIZE - food.size();
			for (int i = 0; i < diff; i++) {
				int tx = rand() % (W_WIDTH - 2) + 1, ty = rand() % (W_HEIGHT - 2) + 1;
				while (World[ty][tx] != X_NONE) {
					tx = rand() % (W_WIDTH - 2) + 1;
					ty = rand() % (W_HEIGHT - 2) + 1;
				}
				food.push_back(new Food(tx, ty)); 
			}
		}
		
		itp = poison.begin();
		
		while (itp != poison.end()) {
			if (World[(*itp)->y][(*itp)->x] == X_POISON) {
				++itp;
			} else {
				itp = poison.erase(itp);
			}
		}
		
		if (poison.size() < S_SIZE) {
			int diff = S_SIZE - poison.size();
			for (int i = 0; i < diff; i++) {
				int tx = rand() % (W_WIDTH - 2) + 1, ty = rand() % (W_HEIGHT - 2) + 1;
				while (World[ty][tx] != X_NONE) {
					tx = rand() % (W_WIDTH - 2) + 1;
					ty = rand() % (W_HEIGHT - 2) + 1;
				}
				poison.push_back(new Poison(tx, ty)); 
			}
		}


		
		it = people.begin();
		while (it != people.end()) {
			if ((*it)->health > 0) {
				if ((*it)->health > MAX_LIFE)
					(*it)->health  = MAX_LIFE;
				(*it)->work();
				++it;
			} else {
				World[(*it)->y][(*it)->x] = X_NONE;
				setfillstyle(1, BLACK);
				(*it)->draw();
				it = people.erase(it);
			}

			if (people.size() <= P_SIZE / 4 && people.size() > 0) {
				int ybw = people.size();
				for (int j = 0; j < ybw; j++) {
						int n = rand() % C_SIZE;
						int v = rand() % C_SIZE;
					for (int i = 0; i < 3; i++) {
						if (j > 0 ) {
							people.push_back(new Actor((*people[j]), X_WALL, -1)); 
						} else {
							people.push_back(new Actor((*people[j]), n, v)); 
						}
					
					}
					for (auto itt = people.begin(); itt != people.end(); ++itt)
						it = itt;
				
				}
				gen++;
				//cout << gen << endl;
			}


		}
		

		
		if (people.size() <= 0) {
			cout << "GAME OVER\n";
			return 0;
			//for (int i = 0; i < P_SIZE; i++)
			//	people.push_back(new Actor(rand() % (W_WIDTH - 2) + 1, rand() % (W_HEIGHT - 2) + 1));
		}
		
		time(&TIMER);
		secs = difftime(TIMER, INIT_TIMER);
		mins = secs / 60;
		secs = secs % 60;
		
		
		//cout << "X";
		string str[10] = {
		"BOTS: " + to_string(people.size()),
		"LIFE: " + to_string((*people.begin())->max_health),
		"GEN: " + to_string(gen),
		" ",
		" ",
		"TIME: " + to_string(mins) + "m " + to_string(secs) + "s",
		" ",
		"PROGRAM: ",
		"        ",
		"        "
		};
		for (int i = 0; i < C_SIZE; i++) {
			if (i < C_SIZE / 2) {
				str[8] += to_string((*people.begin())->commands[i]);
				str[8] += " ";
			} else {
				str[9] += to_string((*people.begin())->commands[i]);
				str[9] += " ";			
			}
		}
		setfillstyle(1, BLACK);
		bar(getmaxx() / 2 + 190, 190, getmaxx(), getmaxy());
		for (int i = 0; i < 10; i++) {
			char *strc = new char[str[i].size() + 1];
			copy(str[i].begin(), str[i].end(), strc);
			strc[str[i].size()] = '\0';
			
			outtextxy(getmaxx() / 2 + 200, 200 + (10 + i - 1) * 30, strc);
		}

/*
		if (GetAsyncKeyState(27)) {
			break;
		}
		if (GetAsyncKeyState(32)) { 
			delay(500);
			while (!GetAsyncKeyState(32)) {
				delay(250);
			}
			delay(500);
		}
*/



		swapbuffers;
		delay(W_DELAY);
		
		step++;
	}


	closegraph();
	system("pause");
	return 0;	
}
