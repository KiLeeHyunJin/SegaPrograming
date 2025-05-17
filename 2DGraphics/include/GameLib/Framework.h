#ifndef INCLUDED_GAMELIB_FRAMEWORK_H
#define INCLUDED_GAMELIB_FRAMEWORK_H

namespace GameLib{

class Framework{
public:
	Framework();
	///ユ?ザ定?関数。これを書く。
	void update();
	///インス?ンス取得
	static Framework instance();
	///画面幅取得
	int width() const;
	///画面高さ取得
	int height() const;
	///VRAM取得
	unsigned* videoMemory();
	///Frameworkに終了命令を出す
	void requestEnd();
	///Frameworkに終了命令が出ているか調べる
	bool isEndRequested() const;

	//以下ライブラリ使用者は知らなくていい
	void start( void* windowHandle );
	void preUpdate();
	void postUpdate();
	static void create();
	static void destroy();
};

//cinのラッパ
class StandardInput{
public:
	StandardInput();
	StandardInput& operator>>( char& );
private:
	bool mPrevEnterOn;
};
extern StandardInput cin;

} //namespace GameLib

#include "Base/DebugStream.h"

#endif
