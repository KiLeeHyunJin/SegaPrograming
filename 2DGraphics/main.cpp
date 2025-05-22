#include "GameLib/Framework.h"


using namespace GameLib;

#include <fstream>
#include <ddraw.h>
using namespace std;

//듫릶긵깓긣?귽긵
void readFile(char** buffer, int* size, const char* filename);
void mainLoop();
unsigned getUnsigned(const char* p);
const unsigned pixelSize = 32;


enum ImageID
{
	IMAGE_ID_PLAYER,
	IMAGE_ID_WALL,
	IMAGE_ID_BLOCK,
	//IMAGE_ID_BLOCK_ON_GOAL,
	IMAGE_ID_GOAL,
	IMAGE_ID_SPACE,
};

template< class T > class Array2D {
public:
	Array2D() : mArray(0), mSize0(0), mSize1(0){}
	~Array2D() {
		delete[] mArray;
		mArray = 0;  
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

class Image
{
public:
	Image(const char* fileName);
	~Image(){}
	int width()const { return mWidth; }
	int height()const { return mHeight; }
	const unsigned* data() const { return mData; }
private:
	int mWidth;
	int mHeight;
	unsigned* mData;
};

Image::Image(const char* fileName)
{
	mWidth = getUnsigned(&fileName[16]);
	mHeight = getUnsigned(&fileName[12]);
	mData = new unsigned[mWidth * mHeight];

	for (unsigned i = 0; i < mHeight * mWidth; ++i)
	{
		mData[i] = getUnsigned(&fileName[128 + (i * 4)]);
	}
}



//륉뫴긏깋긚
class State {
public:
	State(const char* stageData, int size);
	~State();
	void update(char input);
	void draw() const;
	void drawPicture(int dstX, int dstY, int srcX, int srcY, int width, int height, Image* pImg)const;
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
	
private:
	Image* mPImg;
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
		const char* stageName = "stageData.txt";
		char* stageData;
		int fileSize;

		readFile(&stageData, &fileSize, stageName);
		if (!stageData) {
			cout << "stage file could not be read." << endl;
			return;
		}
		gState = new State(stageData, fileSize);
		gState->draw();

		delete[] stageData;
		stageData = 0;
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

unsigned getUnsigned(const char* p)
{
	const unsigned char* cuChr = reinterpret_cast<const unsigned char*>(p);
	unsigned readUnsigned = cuChr[0];
	readUnsigned |= cuChr[1] << 8;
	readUnsigned |= cuChr[2] << 16;
	readUnsigned |= cuChr[3] << 24;
	return readUnsigned;
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

	const char* imgName = "nimotsuKunImage2.dds";
	int notUsed;
	char* imgData;
	readFile(&imgData, &notUsed, imgName);
	mPImg = new Image(imgData);

	delete imgData;
	imgData = 0;
}

State::~State()
{
	delete mPImg;
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

void State::draw()  const
{
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			const Object o = mObjects(x, y);
			bool goalFlag = mGoalFlags(x, y);
			
			if (o != OBJ_WALL)
			{
				if (goalFlag)
				{
					drawPicture(x * 32, y * 32, IMAGE_ID_GOAL * 32, 0, 32, 32, mPImg);
				}
				else
				{
					drawPicture(x * 32, y * 32, IMAGE_ID_SPACE * 32, 0, 32, 32, mPImg);
				}
				ImageID id = IMAGE_ID_SPACE;
				switch (o)
				{
				case OBJ_BLOCK: id = IMAGE_ID_BLOCK;	break;
				case OBJ_MAN:	id = IMAGE_ID_PLAYER;	break;
				}
				if (id != IMAGE_ID_SPACE)
				{
					drawPicture(x * 32, y * 32, id * 32, 0, 32, 32, mPImg);
				}
			}
			else
			{
				drawPicture(x * 32, y * 32, IMAGE_ID_WALL * 32, 0, 32, 32, mPImg);
			}
		
			//그릴 좌표 위치, 이미지 잘라올 시작 위치, 이미지 크기, 이미지 픽셀 배열
			//vram[y * windowWidth + x] = color;
		}
		cout << endl;
	}
}

void State::drawPicture(int dstX, int dstY, int srcX, int srcY, int width, int height, Image* pImg) const
{
	Framework f = Framework::instance();
	unsigned* vram = f.videoMemory();
	int windowWidth = f.width();
	int windowHeight = f.height();
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int pos = ((y + dstY) * windowWidth) + (x + dstX);
			unsigned* dst = &vram[pos];
			unsigned picxel = pImg->data()[(y + srcY) * pImg->width() + (x + srcX)];
			if ((picxel >> 24) >= 128)
			{
				*dst = picxel;
			}
		}
	}
}

void State::update(char input) 
{
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
