#include "GameLib/Framework.h"


using namespace GameLib;

#include <fstream>
#include <ddraw.h>
using namespace std;

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


class State {
public:
	State(const char* stageData, int size);
	~State();
	void update();
	void draw() const;
	void drawPicture(int dstX, int dstY, int srcX, int srcY, int width, int height, Image* pImg)const;
	bool hasCleared() const;
	void getMoveDir(int& x, int& y) const;
private:
	enum Object {
		OBJ_SPACE,
		OBJ_WALL,
		OBJ_BLOCK,
		OBJ_MAN,

		OBJ_UNKNOWN,
	};
	void setSize(const char* stageData, int size);
	unsigned char moveDir;
	unsigned char moveCount;

	bool prevFront;
	bool prevLeft;
	bool prevRight;
	bool prevBack;

	int mWidth;
	int mHeight;
	
private:
	Image* mPImg;
	Array2D< Object > mObjects;
	Array2D< bool > mGoalFlags;
};

State* gState = 0;

namespace GameLib {
	void Framework::update() {
		mainLoop();
	}
}
#define SPACE Framework::instance()

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
	//cout << "a:left s:right w:up z:down. command?" << endl; 

	gState->update();
	gState->draw();



	if (Framework::instance().isEndRequested()) 
	{
		if (gState) 
		{
			delete gState;
			gState = 0;
		}
		return;
	}


	if (cleared) 
	{
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

State::State(const char* stageData, int size) 
{
	setSize(stageData, size);
	mObjects.setSize(mWidth, mHeight);
	mGoalFlags.setSize(mWidth, mHeight);
	for (int y = 0; y < mHeight; ++y) {
		for (int x = 0; x < mWidth; ++x) {
			mObjects(x, y) = OBJ_WALL;
			mGoalFlags(x, y) = false; 
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
		case '\n': x = 0; ++y; t = OBJ_UNKNOWN; break;
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

	prevFront = false;
	prevLeft = false;
	prevRight = false;
	prevBack = false;

	moveDir = 0;
	moveCount = 0;
}

State::~State()
{
	delete mPImg;
}

void State::setSize(const char* stageData, int size) {
	mWidth = mHeight = 0; 
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
					int _x(x), _y(y );
					if (id == IMAGE_ID_PLAYER)
					{
						int dirX(0), dirY(0);
						getMoveDir(dirX, dirY);
						drawPicture(
							(_x - (dirX)) * 32 + (dirX * moveCount),
							(_y - (dirY)) * 32 + (dirY * moveCount), id * 32, 0, 32, 32, mPImg);
					}
					else
					{
						drawPicture(_x * 32, _y * 32, id * 32, 0, 32, 32, mPImg);
					}
				}
			}
			else
			{
				//그릴 좌표 위치, 이미지 잘라올 시작 위치, 이미지 크기, 이미지 픽셀 배열
				drawPicture(x * 32, y * 32, IMAGE_ID_WALL * 32, 0, 32, 32, mPImg);
			}
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

void State::update() 
{
	int dx = 0;
	int dy = 0;

	if (moveDir != 0)
	{
		if (moveCount == 32)
		{
			moveCount = 0;
			moveDir = 0;
		}
		else
		{
			moveCount++;
			return;
		}
	}

	bool front = SPACE.isKeyOn('w');
	bool left = SPACE.isKeyOn('a');
	bool right = SPACE.isKeyOn('s');
	bool back = SPACE.isKeyOn('z');

	if (right != prevRight)
	{
		prevRight = right; dx = 1;
	}
	if (left != prevLeft)
	{
		prevLeft = left; dx = -1;
	}
	if (front != prevFront)
	{
		prevFront = front; dy = -1;
	}
	if (back != prevBack)
	{
		prevBack = back; dy = 1;
	}

	int w = mWidth;
	int h = mHeight;
	Array2D< Object >& o = mObjects;
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
	int tx = x + dx;
	int ty = y + dy;
	if (tx < 0 || ty < 0 || tx >= w || ty >= h) 
	{
		return;
	}
	if (o(tx, ty) == OBJ_SPACE) 
	{
		o(tx, ty) = OBJ_MAN;
		o(x, y) = OBJ_SPACE;

		if (dx != 0)
		{
			moveDir = dx < 0 ? 1 : 2;
		}
		if (dy != 0)
		{
			moveDir = dy < 0 ? 4 : 8;
		}
	}
	else if (o(tx, ty) == OBJ_BLOCK) 
	{
		int tx2 = tx + dx;
		int ty2 = ty + dy;
		if (tx2 < 0 || ty2 < 0 || tx2 >= w || ty2 >= h) 
		{ 
			return;
		}
		if (o(tx2, ty2) == OBJ_SPACE) 
		{
			o(tx2, ty2) = OBJ_BLOCK;
			o(tx, ty) = OBJ_MAN;
			o(x, y) = OBJ_SPACE;

			if (dx != 0)
			{
				moveDir = dx < 0 ? 1 : 2;
			}
			if (dy != 0)
			{
				moveDir = dy < 0 ? 4 : 8;
			}
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

void State::getMoveDir(int& x, int& y) const
{
	if (moveDir == 0)
	{
		x = 0;
		y = 0;
		return;
	}
	int xValue = moveDir & 3;
	int yValue = moveDir & 12;
	if (xValue != 0) //값이 있다면
	{
		x = xValue == 1 ? -1 : 1; // 1이라면 왼쪽 2라면 오른쪽
	}
	if (yValue != 0)
	{
		y = yValue == 4 ? -1 : 1; //4라면 위로 8이라면 아래로
	}
}
