#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <graphics.h>

#include "Untitled1.h"

using namespace std;


vector <Actor*> people;
vector <Food*> food;
vector <Poison*> poison;
vector <Lava*> lava;
vector <Actor*> edible;

// ����� �� ������
int string_to_var(char *str)
{
	string var;
	string line = str;
	string buf = "";
	for(unsigned int i = 0; i < line.size(); i++) {
	    if(line[i] != ' ') { 
	          buf += line[i];    
			  //cout << buf << endl; 
	    }
	    else{
	          var = buf;
	          buf = "";
    	}
	}
	//var = buf;	
	//cout << "MEOW:  " << var << endl;
	return stoi(var);
}

// ������ ���������� ����������
int read_param()
{
	
	const int len = 80, strings = 16;
	const char ch = '\n';
	char arr[len][strings];
	
	ifstream fs("param.txt", ios::in | ios::binary); 
	//if(!fs)
	//	return 1;
	
	for(int r = 0; r < strings; r++)
	{
		fs.getline(arr[r], len - 1, ch); 
		
		//cout << r + 1 << " = "<< arr[r] << endl; 
	}
	
	W_DELAY = string_to_var(arr[0]);
	MAX_STEP = string_to_var(arr[1]);
	MAX_HEALTH = string_to_var(arr[2]);
	MAX_LIFE = string_to_var(arr[3]);
	C_SIZE = string_to_var(arr[4]);
	P_SIZE = string_to_var(arr[5]);
	E_SIZE = string_to_var(arr[6]);
	F_SIZE = string_to_var(arr[7]);
	S_SIZE = string_to_var(arr[8]);
	L_SIZE = string_to_var(arr[9]);
	//DEGRADAT = string_to_var(arr[10]);
	
	/*
	cout << W_DELAY << endl;
	cout << MAX_STEP << endl;
	cout << MAX_HEALTH << endl;
	cout << MAX_LIFE << endl;
	*/
	
	
	fs.close();
	return 0;
}

// ������� �������
int main()
{
	try {
		read_param();
	}
	catch (...) {
		cout << "FILE READ ERROR!\n";
		W_DELAY = 8;
		MAX_STEP = 16;
		MAX_HEALTH = 96;
		MAX_LIFE = 65536;
		C_SIZE = 64;
		P_SIZE = 64;
		E_SIZE = 0;
		F_SIZE = 512;
		S_SIZE = 512;
		L_SIZE = 0;
	}
	
	
	srand(time(NULL));
	if (getmaxwidth() < 1600 || getmaxheight() < 900)
		A_SIZE = 5;
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
   	
   	for (unsigned int i = 0; i < F_SIZE; i++) {
		int tx = rand() % (W_WIDTH - 2) + 1, ty = rand() % (W_HEIGHT - 2) + 1;
		while (World[ty][tx] != X_NONE) {
			tx = rand() % (W_WIDTH - 2) + 1;
			ty = rand() % (W_HEIGHT - 2) + 1;
		}
   		food.push_back(new Food(tx, ty)); 
   	}
   	
   	for (unsigned int i = 0; i < S_SIZE; i++) {
		int tx = rand() % (W_WIDTH - 2) + 1, ty = rand() % (W_HEIGHT - 2) + 1;
		while (World[ty][tx] != X_NONE) {
			tx = rand() % (W_WIDTH - 2) + 1;
			ty = rand() % (W_HEIGHT - 2) + 1;
		}
   		poison.push_back(new Poison(tx, ty));
   	}
   	
   	for (unsigned int i = 0; i < L_SIZE; i++) {
		int tx = rand() % (W_WIDTH - 2) + 1, ty = rand() % (W_HEIGHT - 2) + 1;
		while (World[ty][tx] != X_NONE) {
			tx = rand() % (W_WIDTH - 2) + 1;
			ty = rand() % (W_HEIGHT - 2) + 1;
		}
   		lava.push_back(new Lava(tx, ty));
   	}
	   
	for (unsigned int i = 0; i < P_SIZE; i++) {
		int tx = rand() % (W_WIDTH - 2) + 1, ty = rand() % (W_HEIGHT - 2) + 1;
		while (World[ty][tx] != X_NONE) {
			tx = rand() % (W_WIDTH - 2) + 1;
			ty = rand() % (W_HEIGHT - 2) + 1;
		}
   		people.push_back(new Actor(tx, ty, 0));
   	}

	for (unsigned int i = 0; i < E_SIZE; i++) {
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
				auto *tmp = *itf;
				itf = food.erase(itf);
				delete tmp;
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
				auto *tmp = *itp;
				itp = poison.erase(itp);
				delete tmp;				
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
				auto *tmp = *itl;
				itl = lava.erase(itl);
				delete tmp;
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
				auto tmp = *it;
				it = people.erase(it);
				delete tmp;
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
				auto *tmp = *ite;
				ite = edible.erase(ite);
				delete tmp;
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
			
			outtextxy(getmaxx() / 2 + 200, (12 + i - 1) * 30, strc);
			delete strc;
		}

		/*
		if (GetAsyncKeyState(27)) {
			break;
		}*/
		if (GetAsyncKeyState(19)) { 
			delay(500);
			while (!GetAsyncKeyState(19)) {
				delay(250);
			}
			delay(500);
		}

		if (GetAsyncKeyState(17) && GetAsyncKeyState(68) && GetAsyncKeyState(83)) { // CTRL + D + S
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
