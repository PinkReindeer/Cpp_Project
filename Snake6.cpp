#include "windows.h"
#include <iostream>
#include <conio.h>
#include <time.h>
#include <deque>

bool running = true;

const int width = 80;
const int height = 20;

struct Point {
	int x;
	int y;
};

Point fruit;
Point head;

std::deque<Point> tails;
int tail_length;

enum SnakeDirection{LEFT, RIGHT, UP, DOWN};
SnakeDirection sDir;

// change screen buffer to flat array so we can treat it like a string when print to console
// for example with width = 3, height = 3 we have array like below: 
// 	***\n* *\n***\n with null terminated at the end

char screen_buffer[(width + 1)* height + 1] = { };

void set_pixel(Point p, char value)
{
	screen_buffer[(width + 1) * p.y + p.x] = value;
}

void CmdHideCursor()
{
	HANDLE std_out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO cursor_info;
	GetConsoleCursorInfo(std_out, &cursor_info);
	cursor_info.bVisible = FALSE;

	SetConsoleCursorInfo(std_out, &cursor_info);
}

void SnakeNewFruit() 
{
	fruit.x = (rand() % (width - 2)) + 1;
	fruit.y = (rand() % (height - 2)) + 1;
}

void SnakeInitialize()
{
	sDir = RIGHT;
	
	head.x = width / 2;
	head.y = height / 2;

	tail_length = 0;
	tails.clear();
	
	// generate different sequence of the random number each time run the game
	srand((unsigned int)time(NULL));

	SnakeNewFruit();
	
	for (int i = 0; i < height; ++i) 
	{
		set_pixel({width, i}, '\n');
		
		for (int j = 0; j < width; ++j) 
		{
			bool check = (i == 0 || j == 0 || i == height - 1 || j == width - 1);
			set_pixel({j, i}, check ? '*' : ' ');
		}
	}
}

void SnakeRender()
{
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD topLeft = {0, 0};
    SetConsoleCursorPosition(hOutput, topLeft);

    for(const auto &i: tails)
    {
        if(i.x > 0 && i.y > 0)
        {
            set_pixel(i, 'o');
        }
    }

    set_pixel(head, 'O');
    set_pixel(fruit, 'F');

	std::cout << screen_buffer;
    std::cout << "\nPlayer score: " << tail_length;

	for (const auto &i : tails)
    {
        set_pixel(i, ' ');
    }
	
	set_pixel(head, ' ');
}

void GameLogic()
{
	tails.push_front(head);

	switch (sDir)
	{
	case LEFT:
		--head.x;
		break;
	case RIGHT:
		++head.x;
		break;
	case UP:
		--head.y;
		break;
	case DOWN:
		++head.y;
		break;
	default:
		break;
	}

	if (head.x <= 0 || head.x > width - 2 || head.y <= 0 || head.y > height - 2)
	{
		running = false;
	}

	for (const auto &i: tails)
    {
        if (i.x == head.x && i.y == head.y) 
        {
            running = false;
        }
    }

	if (head.x == fruit.x && head.y == fruit.y)
	{
		++tail_length;
		SnakeNewFruit();
	}
	else 
	{
		tails.pop_back();
	}
}

void UserInput()
{
	if(_kbhit())
	{
		switch (_getch())
		{
			case 'a':
				if (sDir != RIGHT) sDir = LEFT;
				break;
			case 'd':
				if (sDir != LEFT) sDir = RIGHT;
				break;
			case 'w':
				if (sDir != DOWN) sDir = UP;
				break;
			case 's':
				if (sDir != UP) sDir = DOWN;
				break;
			case ' ':
                while (_getch() != ' ');
                break;
            case 'x':
                running = false;
                break;
			default:
				break;
		}
	}
}

int main()
{
	SnakeInitialize();
	CmdHideCursor();
	while (running)
	{
		SnakeRender();
		UserInput();
		GameLogic();

		Sleep(100);
	}
	return 0;
}