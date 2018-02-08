#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include <string>
#include <graphics.h>

//количество команд у бота
#define C_SIZE 64
// задержка глобальная
#define W_DELAY 10
// ширина мира
#define W_WIDTH 128
// высота мира
#define W_HEIGHT 128
// размер объекта
#define A_SIZE 8
// отступ от левого края до карты
#define W_LEFT 20
// отступ от верхнего края до карты
#define W_TOP 20
// количество ботов
#define P_SIZE 32
// количество хавки
#define F_SIZE 512
// максимальное количество команд за ход
#define MAX_STEP 32
// базовое здоровье
#define MAX_HEALTH 64
// максимальное здоровье
#define MAX_LIFE 65536
// количество яда
#define S_SIZE 512
// коэффициент деградации
#define DEGRADAT 3/4
// количество лавы
#define L_SIZE 0
// количество съедобных ботов
#define E_SIZE 0

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

using namespace std;
int DRAW_MODE = 1; // флаг отрисовки

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
		int pointer;
		int mutant;
		int edible;
		
		int health, max_health;
		int commands[C_SIZE];
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
	
	for (int i = 0; i < C_SIZE; i++) {
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

vector <Actor*> people;
vector <Food*> food;
vector <Poison*> poison;
vector <Lava*> lava;
vector <Actor*> edible;

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
   			if ((abs(i - W_HEIGHT / 2) < 4 && abs(j - W_WIDTH / 2) > 32) || (abs(j - W_WIDTH / 2) < 4 && abs(i - W_HEIGHT / 2) > 32))
   				World[i][j] = X_WALL;
   			if ((abs(i - W_HEIGHT / 2) < 8 ) && abs(j - W_WIDTH / 2) < 8 )
   				World[i][j] = X_WALL;
			
			//if ((abs(i % 32 - 0) <= 16 && abs(j % 32 - 0) < 4) || (abs(i % 32 - 0) >= 16 && abs(i % 32 - 0) < 32 && abs(j % 32 - 0) >= 16 && abs(j % 32 - 0) < 20))
			//		World[i][j] = X_WALL;
			   				
   			if (World[i][j] == X_WALL) {
   				if (DRAW_MODE == 1) {
   					setfillstyle(1, DARKGRAY);
   					bar(j * A_SIZE + W_LEFT, i * A_SIZE + W_TOP, j * A_SIZE + A_SIZE + W_LEFT, i * A_SIZE + A_SIZE + W_TOP);
   				}
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
   	
   	for (int i = 0; i < L_SIZE; i++) {
		int tx = rand() % (W_WIDTH - 2) + 1, ty = rand() % (W_HEIGHT - 2) + 1;
		while (World[ty][tx] != X_NONE) {
			tx = rand() % (W_WIDTH - 2) + 1;
			ty = rand() % (W_HEIGHT - 2) + 1;
		}
   		lava.push_back(new Lava(tx, ty));
   	}
	   
	for (int i = 0; i < P_SIZE; i++) {
		int tx = rand() % (W_WIDTH - 2) + 1, ty = rand() % (W_HEIGHT - 2) + 1;
		while (World[ty][tx] != X_NONE) {
			tx = rand() % (W_WIDTH - 2) + 1;
			ty = rand() % (W_HEIGHT - 2) + 1;
		}
   		people.push_back(new Actor(tx, ty, 0));
   	}

	for (int i = 0; i < E_SIZE; i++) {
		int tx = rand() % (W_WIDTH - 2) + 1, ty = rand() % (W_HEIGHT - 2) + 1;
		while (World[ty][tx] != X_NONE) {
			tx = rand() % (W_WIDTH - 2) + 1;
			ty = rand() % (W_HEIGHT - 2) + 1;
		}
   		edible.push_back(new Actor(tx, ty, 1));
   	}
   	
   	
   	time(&INIT_TIMER);
	time(&TIMER);
	int mins = 0, secs = 0;   	
   	
   	auto it = people.begin();
   	auto itf = food.begin();
   	auto itp = poison.begin();
   	auto itl = lava.begin();
   	auto ite = edible.begin();

	int gen1 = 0, gen2 = 0;

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


		itl = lava.begin();
		
		while (itl != lava.end()) {
			if (World[(*itl)->y][(*itl)->x] == X_LAVA) {
				++itl;
			} else {
				itl = lava.erase(itl);
			}
		}
		
		if (lava.size() < L_SIZE) {
			int diff = L_SIZE - lava.size();
			for (int i = 0; i < diff; i++) {
				int tx = rand() % (W_WIDTH - 2) + 1, ty = rand() % (W_HEIGHT - 2) + 1;
				while (World[ty][tx] != X_NONE) {
					tx = rand() % (W_WIDTH - 2) + 1;
					ty = rand() % (W_HEIGHT - 2) + 1;
				}
				lava.push_back(new Lava(tx, ty)); 
			}
		}

		
		it = people.begin();
		while (it != people.end()) {
			if (people.size() == 0) {
				break;
			}
			if ((*it)->health > 0) {
				if ((*it)->health > MAX_LIFE) {
					(*it)->health  = MAX_LIFE;
					(*it)->max_health = MAX_LIFE;
				}
				(*it)->work();
				++it;
			} else {
				World[(*it)->y][(*it)->x] = X_NONE;
				if (DRAW_MODE == 1) {
					setfillstyle(1, BLACK);
					(*it)->draw();
				}
				it = people.erase(it);
			}
			
			if (people.size() <= P_SIZE / 4 && people.size() > 0) {
				int ybw = people.size();
				for (int j = 0; j < ybw; j++) {
						int n = rand() % C_SIZE;
						int v = rand() % C_SIZE;
					for (int i = 0; i < 3; i++) {
						if (j > ybw / 4) {
							people.push_back(new Actor((*people[j]), -1, -1)); 
						} else {
							people.push_back(new Actor((*people[j]), n, v)); 
						}
					}
					for (auto itt = people.begin(); itt != people.end(); ++itt)
						it = itt;
				}
				gen1++;
			}

		}
		

		ite = edible.begin();
		while (ite != edible.end()) {
			if (edible.size() == 0)
				break;
			if ((*ite)->health > 0 && World[(*ite)->y][(*ite)->x] == X_EDIBLE) {
				if ((*ite)->health > MAX_LIFE) {
					(*ite)->health  = MAX_LIFE;
					(*ite)->max_health = MAX_LIFE;
				}
				(*ite)->work();
				++ite;
			} else {
				World[(*ite)->y][(*ite)->x] = X_NONE;
				if (DRAW_MODE == 1) {
					setfillstyle(1, BLACK);
					(*ite)->draw();
				}
				ite = edible.erase(ite);
			}

			if (edible.size() <= E_SIZE / 4 && edible.size() > 0) {
				int ybw = edible.size();
				for (int j = 0; j < ybw; j++) {
						int n = rand() % C_SIZE;
						//int r = rand() % 4;
						int v = rand() % C_SIZE;
						//if (r != 0)
						//	v = rand() % C_SIZE / 4;
						
					for (int i = 0; i < 3; i++) {
						if (j > ybw / 4) {
							edible.push_back(new Actor((*edible[j]), -1, -1)); 
						} else {
							edible.push_back(new Actor((*edible[j]), n, v)); 
						}
					}
					for (auto itet = edible.begin(); itet != edible.end(); ++itet)
						ite = itet;
				}
				gen2++;
			}

		}

		time(&TIMER);
		secs = difftime(TIMER, INIT_TIMER);
		mins = secs / 60;
		secs = secs % 60;
		
		int l1 = (P_SIZE == 0)? 0 : (*people.begin())->max_health, l2 = (E_SIZE == 0)? 0 : (*edible.begin())->max_health;
		string str[12] = {
		"BOTS: " + to_string(people.size()) + "  :  " + to_string(edible.size()),
		"LIFE: " + to_string(l1) + "  :  " + to_string(l2),
		" ",
		"GEN: " + to_string(gen1) + "  :  " + to_string(gen2),
		" ",
		"TIME: " + to_string(mins) + "m " + to_string(secs) + "s",
		" ",
		"CARNIVORES: " + to_string(people.size()) + "  :  " + to_string(people.capacity()),
		"HERBIVORES: " + to_string(edible.size()) + "  :  " + to_string(edible.capacity()),
		"FOOD: " + to_string(food.size()) + "  :  " + to_string(food.capacity()),
		"POISON: " + to_string(poison.size()) + "  :  " + to_string(poison.capacity()),
		"LAVA: " + to_string(lava.size()) + "  :  " + to_string(lava.capacity())
		};/*
		for (int i = 0; i < C_SIZE; i++) {
			if (i < C_SIZE / 4) {
				str[8] += to_string((*people.begin())->commands[i]);
				str[8] += " ";
			}
			if (i >= C_SIZE / 4 && i < C_SIZE / 2) {
				str[9] += to_string((*people.begin())->commands[i]);
				str[9] += " ";			
			}
			if (i >= C_SIZE / 2 && i < C_SIZE * 3 / 4) {
				str[10] += to_string((*people.begin())->commands[i]);
				str[10] += " ";			
			}
			if (i >= C_SIZE * 3 / 4) {
				str[11] += to_string((*people.begin())->commands[i]);
				str[11] += " ";			
			}
		}*/
		setfillstyle(1, BLACK);
		bar(getmaxx() / 2 + 190, 190, getmaxx(), getmaxy());
		for (int i = 0; i < 12; i++) {
			char *strc = new char[str[i].size() + 1];
			copy(str[i].begin(), str[i].end(), strc);
			strc[str[i].size()] = '\0';
			
			outtextxy(getmaxx() / 2 + 200, 200 + (12 + i - 1) * 30, strc);
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
		if (GetAsyncKeyState(17)) { // CTRL
			if (DRAW_MODE == 1) {
				DRAW_MODE = 0;
				cleardevice();
				delay(500);
			} else {
				delay(500);
				DRAW_MODE = 1;
				for (int i = 0; i < W_HEIGHT; i++) {
					for (int j = 0; j < W_WIDTH; j++) {
						if (World[i][j] == X_WALL) {
							setfillstyle(1, DARKGRAY);
   							bar(j * A_SIZE + W_LEFT, i * A_SIZE + W_TOP, j * A_SIZE + A_SIZE + W_LEFT, i * A_SIZE + A_SIZE + W_TOP);
						}
					}
				}
				setfillstyle(1, GREEN);
				for (auto i = food.begin(); i != food.end(); ++i)				
					(*i)->draw();
				setfillstyle(1, RED);
				for (auto i = poison.begin(); i != poison.end(); ++i)				
					(*i)->draw();
				setfillstyle(1, MAGENTA);
				for (auto i = lava.begin(); i != lava.end(); ++i)				
					(*i)->draw();			
				setfillstyle(1, WHITE);
				for (auto i = people.begin(); i != people.end(); ++i)				
					(*i)->draw();
				setfillstyle(1, CYAN);
				for (auto i = edible.begin(); i != edible.end(); ++i)				
					(*i)->draw();				
			}				
		}

		//swapbuffers;
		if (DRAW_MODE == 1)
			delay(W_DELAY);

	}


	closegraph();
	system("pause");
	return 0;	
}
