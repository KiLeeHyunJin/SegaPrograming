#include <iostream>
#include <fstream>
#include "CUtils.h"
using namespace std;

enum Object {
	OBJ_SPACE,
	OBJ_WALL,
	OBJ_GOAL,
	OBJ_BLOCK,
	OBJ_BLOCK_ON_GOAL,
	OBJ_MAN,
	OBJ_MAN_ON_GOAL,

	OBJ_UNKNOWN,
};

void Initialize(Object* state, int w, int h, const char* stageData);
void Draw(const Object* state, int w, int h);
void Update(Object* state, char input, int w, int h);
bool CheckClear(const Object* state, int w, int h);

const char gStateData[] = "\
########\n\
# .. p #\n\
#oo    #\n\
#      #\n\
########";

const int gStateWidth = 8;
const int gStateHeight = 5;

int main()
{
	CUtils<int> pUtils(20,20);
	int temp = pUtils(15, 19);
	temp = 20.0f;
	pUtils(20,1) = 2;
	ifstream inputFile("stageData.txt", ifstream::binary);
	inputFile.seekg(0, ifstream::end);
	int fileSize = static_cast<int>(inputFile.tellg());
	if (fileSize < 0)
	{
		return 0; //파일을 못찾음
	}
	inputFile.seekg(0, ifstream::beg);
	char* fileImage = new char[fileSize];
	inputFile.read(fileImage, fileSize);
	cout.write(fileImage, fileSize);
	return 0 ;

	char c;
	cin >> c;
	cout << "Input Character is " << c << endl;
	
	Object* state = new Object[gStateWidth * gStateHeight];
	Initialize(state, gStateWidth, gStateHeight, gStateData);
	while (true)
	{
		Draw(state, gStateWidth, gStateHeight);
		if (CheckClear(state, gStateWidth, gStateHeight))
		{
			break;
		}
		cout << "A: Left, S: RIght, W: Up, Z:Down. Command?" << endl;
		char input;
		cin >> input;
		Update(state, input, gStateWidth, gStateHeight);
		//Update(state, )
	}
	cout << "Congratulation's you win!" << endl;

	delete[] state;
	state = nullptr;
	return 0;
}

void Initialize(Object* state, int w, int h, const char* stageData)
{
	const char* d = stageData;
	int x = 0;
	int y = 0;
	while (*d != '\0')
	{
		Object t;
		switch (*d)
		{
			case '#':	t = OBJ_WALL;			break;
			case ' ':	t = OBJ_SPACE;			break;
			case 'o':	t = OBJ_BLOCK;			break;
			case 'O':	t = OBJ_BLOCK_ON_GOAL;	break;
			case '.':	t = OBJ_GOAL;			break;
			case 'p':	t = OBJ_MAN;			break;
			case 'P':	t = OBJ_MAN_ON_GOAL;	break;
			case '\n': 
				x = 0;
				++y;
						t = OBJ_UNKNOWN;		break;
			default:	t = OBJ_UNKNOWN;		break;
		}
		++d;
		if (t != OBJ_UNKNOWN)
		{
			state[y * w + x] = t; 
			++x;
		}
	}
}

void Draw(const Object* state, int w, int h)
{
	static const char font[] = { ' ', '#', '.', 'o', 'O', 'p', 'P' };

	for (unsigned int y = 0; y < h; y++)
	{
		for (unsigned int x = 0; x < w; x++)
		{
			const Object o = state[y * w + x];
			cout << font[o];
		}
		cout << endl;
	}
}

void Update(Object* state, char input, int w, int h)
{
	int dx = 0;
	int dy = 0;
	switch (input)
	{
		case 'a': dx = -1; break;
		case 's': dx =  1; break;
		case 'w': dy = -1; break;
		case 'z': dy =  1; break;
	}
	int i = 0;
	for (; i < w * h; i++)
	{
		if (state[i] == OBJ_MAN || state[i] == OBJ_MAN_ON_GOAL) {
			break;
		}
	}
	int x = i % w;
	int y = i / w;

	int tx = x + dx;
	int ty = y + dy;
	if (tx < 0 || ty < 0 || tx >= w || ty >= h)
	{	return;		}

	int p = y * w + x;
	int tp = ty * w + tx;
	if (state[tp] == OBJ_SPACE || state[tp] == OBJ_GOAL)
	{
		state[tp] = (state[tp] == OBJ_GOAL) ? OBJ_MAN_ON_GOAL : OBJ_MAN;
		state[p] = (state[p] == OBJ_MAN_ON_GOAL) ? OBJ_GOAL : OBJ_SPACE;
	}
	else if (state[tp] == OBJ_BLOCK || state[tp] == OBJ_BLOCK_ON_GOAL)
	{
		int tx2 = tx + dx;
		int ty2 = ty + dy;//플레이어가 이동할 위치 + 입력값 = 2칸 앞
		if (tx2 < 0 || ty2 < 0 || tx2 >= w || ty2 >= h)
		{	return;	}

		int tp2 = (ty + dy) * w + (tx + dx);
		if (state[tp2] == OBJ_SPACE || state[tp2] == OBJ_GOAL)
		{
			state[tp2] = (state[tp2] == OBJ_GOAL) ? OBJ_BLOCK_ON_GOAL : OBJ_BLOCK;
			state[tp] = (state[tp] == OBJ_BLOCK_ON_GOAL) ? OBJ_MAN_ON_GOAL : OBJ_MAN;
			state[p] = (state[p] == OBJ_MAN_ON_GOAL) ? OBJ_GOAL : OBJ_SPACE;
		}

	}
	
}

bool CheckClear(const Object* state, int w, int h)
{
	for (int i = 0; i < w * h; i++)
	{
		if (state[i] == OBJ_BLOCK)
		{
			return false;
		}
	}
	return true;
}


