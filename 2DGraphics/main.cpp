#include "GameLib/Framework.h"

using namespace GameLib;

#include <fstream>
using namespace std;

//듫릶긵깓긣?귽긵
void readFile(char** buffer, int* size, const char* filename);
void mainLoop();
const unsigned pixelSize = 32;

//볫렅뙰봹쀱긏깋긚
//긡깛긵깒?긣궸궶궣귒궼궇귡궬귣궎궔갎궶궚귢궽딈멳궬궚궳귖뺈떗궢궲궓궞궎갃
//궞궻긏깋긚먬뙻궻뭷궳궼T궴궋궎긏깋긚궕궇귡궔궻귝궎궸댌귦귢갂
//궞귢귩럊궎렄궸궼T궻궴궞귣궸int궴궔bool궴궔볺귢궲럊궎갃
template< class T > class Array2D {
public:
	Array2D() : mArray(0), mSize0(0), mSize1(0){}
	~Array2D() {
		delete[] mArray;
		mArray = 0;  //?귽깛?궸0귩볺귢귡궻궼긏긜궸궢귝궎갃
	}
	void setSize(int size0, int size1) {
		mSize0 = size0;
		mSize1 = size1;
		mArray = new T[size0 * size1];
	}
	T& operator()(int index0, int index1) {
		return mArray[index1 * mSize0 + index0];
	}
	const T& operator()(int index0, int index1) const {
		return mArray[index1 * mSize0 + index0];
	}
private:
	T* mArray;
	int mSize0;
	int mSize1;
};

//륉뫴긏깋긚
class State {
public:
	State(const char* stageData, int size);
	void update(char input);
	void draw() const;
	bool hasCleared() const;
private:
	enum Object {
		OBJ_SPACE,
		OBJ_WALL,
		OBJ_BLOCK,
		OBJ_MAN,

		OBJ_UNKNOWN,
	};
	void setSize(const char* stageData, int size);

	int mWidth;
	int mHeight;
	Array2D< Object > mObjects;
	Array2D< bool > mGoalFlags;
};

//긐깓?긫깑빾릶
State* gState = 0;

//깇?긗렳몧듫릶갃뭷릆궼mainLoop()궸듴뱤궛
namespace GameLib {
	void Framework::update() {
		mainLoop();
	}
}

void mainLoop() {
	if (!gState) {
		const char* filename = "IMG_1855.dds";
		char* stageData;
		int fileSize;
		readFile(&stageData, &fileSize, filename);
		if (!stageData) {
			cout << "stage file could not be read." << endl;
			return;
		}
		gState = new State(stageData, fileSize);
		delete[] stageData;
		stageData = 0;
		gState->draw();
		return; 
	}
	bool cleared = false;
	if (gState->hasCleared()) {
		cleared = true;
	}
	//볺쀍롦벦
	cout << "a:left s:right w:up z:down. command?" << endl; //?띿먣뼻
	char input;
	cin >> input;

	//뛛륷
	gState->update(input);
	//?됪
	gState->draw();

	if (input == 'q') {
		Framework::instance().requestEnd();
	}
	//???깛돓궠귢궲귡갎
	if (Framework::instance().isEndRequested()) {
		if (gState) {
			delete gState;
			gState = 0;
		}
		return;
	}


	if (cleared) {
		//뢪궋궻긽긞긜?긙
		cout << "Congratulation! you win." << endl;
		delete gState;
		gState = 0;
	}
}

//---------------------댥돷듫릶믦?------------------------------------------

void readFile(char** buffer, int* size, const char* filename) {
	ifstream in(filename, ifstream::binary);
	if (!in) {
		*buffer = 0;
		*size = 0;
	}
	else {
		in.seekg(0, ifstream::end);
		*size = static_cast<int>(in.tellg());
		in.seekg(0, ifstream::beg);
		*buffer = new char[*size];
		in.read(*buffer, *size);
	}
}

void drawCell(int x, int y, unsigned color)
{
	unsigned* vram = Framework::instance().videoMemory();
	unsigned windowWidth = Framework::instance().width();
	for (int i = 0; i < pixelSize; i++)
	{
		for (int j = 0; j < pixelSize; j++)
		{
			vram[(y * pixelSize + i) * windowWidth + (x * pixelSize + j)] = color;
		}
	}

}

State::State(const char* stageData, int size) {
	//긖귽긛뫇믦
	setSize(stageData, size);
	//봹쀱둴뺎
	mObjects.setSize(mWidth, mHeight);
	mGoalFlags.setSize(mWidth, mHeight);
	//룊딖뭠궳뻹귕궴궘
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			mObjects(x, y) = OBJ_WALL; //궇귏궯궫븫빁궼빮
			mGoalFlags(x, y) = false; //긕?깑궣귗궶궋
		}
	}
	int x = 0;
	int y = 0;
	for (int i = 0; i < size; ++i) {
		Object t;
		bool goalFlag = false;
		switch (stageData[i]) {
		case '#': t = OBJ_WALL; break;
		case ' ': t = OBJ_SPACE; break;
		case 'o': t = OBJ_BLOCK; break;
		case 'O': t = OBJ_BLOCK; goalFlag = true; break;
		case '.': t = OBJ_SPACE; goalFlag = true; break;
		case 'p': t = OBJ_MAN; break;
		case 'P': t = OBJ_MAN; goalFlag = true; break;
		case '\n': x = 0; ++y; t = OBJ_UNKNOWN; break; //둂뛱룉뿚
		default: t = OBJ_UNKNOWN; break;
		}
		if (t != OBJ_UNKNOWN) 
		{ 
			mObjects(x, y) = t; 
			mGoalFlags(x, y) = goalFlag; 
			++x;
		}
	}
}

void State::setSize(const char* stageData, int size) {
	mWidth = mHeight = 0; //룊딖돸
	//뙸띪댧뭫
	int x = 0;
	int y = 0;
	for (int i = 0; i < size; ++i) {
		switch (stageData[i]) {
		case '#': case ' ': case 'o': case 'O':
		case '.': case 'p': case 'P':
			++x;
			break;
		case '\n':
			++y;
			//띍묈뭠뛛륷
			mWidth = max(mWidth, x);
			mHeight = max(mHeight, y);
			x = 0;
			break;
		}
	}
}

void State::draw() const {
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			Object o = mObjects(x, y);
			bool goalFlag = mGoalFlags(x, y);
			unsigned color = 0;
			if (goalFlag) {
				switch (o) {
				case OBJ_SPACE: cout << '.'; color = 0x0000ff; break;
				case OBJ_WALL: cout << '#'; color = 0xffffff; break;
				case OBJ_BLOCK: cout << 'O'; color = 0xff00ff; break;
				case OBJ_MAN: cout << 'P'; color = 0x00ffff; break;
				}
			}
			else {
				switch (o) {
				case OBJ_SPACE: cout << ' '; color = 0x000000; break;
				case OBJ_WALL: cout << '#'; color = 0xffffff; break;
				case OBJ_BLOCK: cout << 'o'; color = 0xff0000; break;
				case OBJ_MAN: cout << 'p'; color = 0x00ff00; break;
				}
			}
			drawCell(x, y, color);
			//vram[y * windowWidth + x] = color;
		}
		cout << endl;
	}
}

void State::update(char input) {
	//댷벍뜼빁궸빾듂
	int dx = 0;
	int dy = 0;
	switch (input) {
	case 'a': dx = -1; break; //뜺
	case 's': dx = 1; break; //덭
	case 'w': dy = -1; break; //뤵갃Y궼돷궕긵깋긚
	case 'z': dy = 1; break; //돷갃
	}
	//뭒궋빾릶뼹귩궰궚귡갃
	int w = mWidth;
	int h = mHeight;
	Array2D< Object >& o = mObjects;
	//릐띆뷭귩뙚랊
	int x, y;
	x = y = -1;
	bool found = false;
	for (y = 0; y < mHeight; ++y) {
		for (x = 0; x < mWidth; ++x) {
			if (o(x, y) == OBJ_MAN) {
				found = true;
				break;
			}
		}
		if (found) {
			break;
		}
	}
	//댷벍
	//댷벍뚣띆뷭
	int tx = x + dx;
	int ty = y + dy;
	//띆뷭궻띍묈띍룷?긃긞긏갃둖귢궲궋귢궽븉떀됀
	if (tx < 0 || ty < 0 || tx >= w || ty >= h) {
		return;
	}
	//A.궩궻뺴뛀궕뗴뵏귏궫궼긕?깑갃릐궕댷벍갃
	if (o(tx, ty) == OBJ_SPACE) {
		o(tx, ty) = OBJ_MAN;
		o(x, y) = OBJ_SPACE;
		//B.궩궻뺴뛀궕뵠갃궩궻뺴뛀궻렅궻?긚궕뗴뵏귏궫궼긕?깑궳궇귢궽댷벍갃
	}
	else if (o(tx, ty) == OBJ_BLOCK) {
		//2?긚먩궕붝댪볙궔?긃긞긏
		int tx2 = tx + dx;
		int ty2 = ty + dy;
		if (tx2 < 0 || ty2 < 0 || tx2 >= w || ty2 >= h) { //돓궧궶궋
			return;
		}
		if (o(tx2, ty2) == OBJ_SPACE) {
			//룈렅볺귢뫶궑
			o(tx2, ty2) = OBJ_BLOCK;
			o(tx, ty) = OBJ_MAN;
			o(x, y) = OBJ_SPACE;
		}
	}
}

//긳깓긞긏궻궴궞귣궻goalFlag궕덇궰궳귖false궶귞
//귏궬긏깏귺궢궲궶궋
bool State::hasCleared() const {
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			if (mObjects(x, y) == OBJ_BLOCK) {
				if (mGoalFlags(x, y) == false) {
					return false;
				}
			}
		}
	}
	return true;
}



//namespace GameLib
//{
//	enum Object {
//		OBJ_SPACE			= 1 << 0,
//		OBJ_WALL			= 1 << 1,
//		OBJ_BLOCK			= 1 << 2,
//		OBJ_MAN				= 1 << 3,
//
//		OBJ_GOAL			= 1 << 4,
//		OBJ_MAN_ON_GOAL		= 1 << 5,
//		OBJ_BLOCK_ON_GOAL	= 1 << 6,
//
//		OBJ_UNKNOWN			= 1 << 7,
//	};
//
//	void Initialize(Object* state, int w, int h, const char* stageData);
//	void Draw(const Object* state, int w, int h);
//	void Update(Object* state, char input, int w, int h);
//	void GetInput();
//	bool CheckClear(const Object* state, int w, int h);
//
//	const char gStateData[] = "\
//########\n\
//# .. p #\n\
//#oo    #\n\
//#      #\n\
//########";
//
//	const int gStateWidth = 8;
//	const int gStateHeight = 5;
//
//
//
//	void Initialize(Object* state, int w, int h, const char* stageData)
//	{
//		const char* d = stageData;
//		int x = 0;
//		int y = 0;
//		while (*d != '\0')
//		{
//			Object t;
//			switch (*d)
//			{
//			case '#':	t = OBJ_WALL;			break;
//			case ' ':	t = OBJ_SPACE;			break;
//			case 'o':	t = OBJ_BLOCK;			break;
//			case 'O':	t = OBJ_BLOCK_ON_GOAL;	break;
//			case '.':	t = OBJ_GOAL;			break;
//			case 'p':	t = OBJ_MAN;			break;
//			case 'P':	t = OBJ_MAN_ON_GOAL;	break;
//			case '\n':
//				x = 0;
//				++y;
//				t = OBJ_UNKNOWN;		break;
//			default:	t = OBJ_UNKNOWN;		break;
//			}
//			++d;
//			if (t != OBJ_UNKNOWN)
//			{
//				state[y * w + x] = t;
//				++x;
//			}
//		}
//	}
//
//	void Draw(const Object* state, int w, int h)
//	{
//		unsigned int* vram = Framework::instance().videoMemory();
//		unsigned windowWidth = Framework::instance().width();
//		for (int y = 0; y < h; y++)
//		{
//			for (int x = 0; x < w; x++)
//			{
//				const Object o = state[y * w + x];
//				bool goalFlag = o & (OBJ_BLOCK_ON_GOAL | OBJ_MAN_ON_GOAL | OBJ_GOAL) ;
//				unsigned color = 0;
//				if (goalFlag)
//				{
//					switch (o)
//					{
//						case OBJ_SPACE: cout << '.'; color = 0x0000ff;	break;
//						case OBJ_WALL:	cout << '#'; color = 0xffffff;	break;
//						case OBJ_BLOCK: cout << 'O'; color = 0xff00ff;	break;
//						case OBJ_MAN:	cout << 'P'; color = 0x00ffff;	break;
//						default:										break;
//					}
//				}
//				else
//				{
//					switch (o)
//					{
//						case OBJ_SPACE: cout << ' '; color = 0x000000;	break;
//						case OBJ_WALL:	cout << '#'; color = 0xffffff;	break;
//						case OBJ_BLOCK: cout << 'o'; color = 0xff0000;	break;
//						case OBJ_MAN:	cout << 'p'; color = 0x00ff00;	break;
//						default:										break;
//					}
//				}
//				vram[y * windowWidth + x] = color;
//			}
//		}
//	}
//
//	void Update(Object* state, char input, int w, int h)
//	{
//		int dx = 0;
//		int dy = 0;
//		switch (input)
//		{
//		case 'a': dx = -1; break;
//		case 's': dx = 1; break;
//		case 'w': dy = -1; break;
//		case 'z': dy = 1; break;
//		}
//		int i = 0;
//		for (; i < w * h; i++)
//		{
//			if (state[i] & (OBJ_MAN_ON_GOAL | OBJ_MAN))
//			{
//				break;
//			}
//		}
//		int x = i % w;
//		int y = i / w;
//
//		int tx = x + dx;
//		int ty = y + dy;
//		if (tx < 0 || ty < 0 || tx >= w || ty >= h)
//		{
//			return;
//		}
//
//		int p = y * w + x;
//		int tp = ty * w + tx;
//		if (state[tp] & OBJ_SPACE | OBJ_GOAL)
//		{
//			state[tp] = (state[tp] & OBJ_GOAL) ? OBJ_MAN_ON_GOAL : OBJ_MAN;
//			state[p] = (state[p] & OBJ_MAN_ON_GOAL) ? OBJ_GOAL : OBJ_SPACE;
//		}
//		else if (state[tp] & OBJ_BLOCK | OBJ_BLOCK_ON_GOAL)
//		{
//			int tx2 = tx + dx;
//			int ty2 = ty + dy;//플레이어가 이동할 위치 + 입력값 = 2칸 앞
//			if (tx2 < 0 || ty2 < 0 || tx2 >= w || ty2 >= h)
//			{
//				return;
//			}
//
//			int tp2 = (ty + dy) * w + (tx + dx);
//			if (state[tp2] & OBJ_SPACE | OBJ_GOAL)
//			{
//				state[tp2] = (state[tp2] & OBJ_GOAL) ? OBJ_BLOCK_ON_GOAL : OBJ_BLOCK;
//				state[tp] = (state[tp] & OBJ_BLOCK_ON_GOAL) ? OBJ_MAN_ON_GOAL : OBJ_MAN;
//				state[p] = (state[p] & OBJ_MAN_ON_GOAL) ? OBJ_GOAL : OBJ_SPACE;
//			}
//
//		}
//
//	}
//
//	void GetInput()
//	{
//	}
//
//	bool CheckClear(const Object* state, int w, int h)
//	{
//		for (int i = 0; i < w * h; i++)
//		{
//			if (state[i] & OBJ_BLOCK)
//			{
//				return false;
//			}
//		}
//		return true;
//	}
//
//	void Framework::update() {
//		static unsigned i;
//		static unsigned char red, green, blue;
//		red = 0;
//		green = 0;
//		blue = 0;
//
//		{
//
//
//			char c;
//			cin >> c;
//			cout << "Input Character is " << c << endl;
//
//			Object* state = new Object[gStateWidth * gStateHeight];
//			Initialize(state, gStateWidth, gStateHeight, gStateData);
//			while (true)
//			{
//				Draw(state, gStateWidth, gStateHeight);
//				if (CheckClear(state, gStateWidth, gStateHeight))
//				{
//					break;
//				}
//				cout << "A: Left, S: RIght, W: Up, Z:Down. Command?" << endl;
//				char input;
//				cin >> input;
//				Update(state, input, gStateWidth, gStateHeight);
//				//Update(state, )
//			}
//			cout << "Congratulation's you win!" << endl;
//
//			delete[] state;
//			state = nullptr;
//			/*unsigned int* vram = videoMemory();
//			red = 255;
//			green = 255;
//			blue = 255;
//			unsigned int color = (red << 16) | (green << 8) | (blue << 0);
//			for (size_t i = 100; i <= 200; i++)
//			{
//				for (size_t j = 100; j <= 200; j++)
//				{
//					vram[j * width() + i] = color;
//
//				}
//			}*/
//
//			//i += 9973;
//			//i %= (width() * height());
//		}
//
//	}
//}
//
