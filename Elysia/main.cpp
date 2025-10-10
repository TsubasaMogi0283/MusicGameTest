#include "Framework.h"

//リリースチェッカー
static D3DResourceLeakChecker checker;

//main関数
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//フレームワークの生成
	std::unique_ptr<Elysia::Framework> elysia = std::make_unique<Elysia::Framework>();

	//実行
	elysia->Run();

	//終了
	return 0;
}

