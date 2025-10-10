#include "WindowsSetup.h"

#pragma comment(lib,"winmm.lib")

#include "Convert.h"


Elysia::WindowsSetup* Elysia::WindowsSetup::GetInstance() {
	static Elysia::WindowsSetup instance;
	return &instance;
}



LRESULT Elysia::WindowsSetup::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam){
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
	
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対してアプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);

}

void Elysia::WindowsSetup::OutputStringW(std::string& stringText){
	//stringからwstringに変換
	std::wstring newString = Convert::Text::ToWString(stringText);
	//出力
	OutputDebugStringW(newString.c_str());
}

void Elysia::WindowsSetup::OutPutStringA(const std::string& text){
	//出力
	OutputDebugStringA(text.c_str());
}


#pragma region Initializeに入れる関数

//ウィンドウに情報を入れる
void  Elysia::WindowsSetup::RegisterWindowsClass(const wchar_t* title) {
	
	//ウィンドウプロシャージャ
	windowClass_.lpfnWndProc = WindowProc;
	//ウィンドウクラス名
	windowClass_.lpszClassName = L"%s", title;
	//インスタンドハンドル
	windowClass_.hInstance = GetModuleHandle(nullptr);
	//カーソル
	windowClass_.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//ウィンドウクラス登録
	RegisterClass(&windowClass_);

	//クライアント領域サイズ
	//ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc_ = { 0,0,LONG(clientWidth_) ,LONG(clientHeight_) };
	// クライアント領域を元に実際のサイズにwrcを変更
	AdjustWindowRect(&wrc_, WS_OVERLAPPEDWINDOW, false);
	// ウィンドウ生成
	hwnd_ = CreateWindow(
		//クラス名
		windowClass_.lpszClassName,
		//タイトルバーの文字
		title,
		//標準的なウィンドウスタイル
		WS_OVERLAPPEDWINDOW,
		//標準X座標
		CW_USEDEFAULT,
		//標準Y座標
		CW_USEDEFAULT,
		//横幅
		wrc_.right - wrc_.left,
		//縦幅
		wrc_.bottom - wrc_.top,
		//親ハンドル
		nullptr,
		//メニューハンドル
		nullptr,
		//インスタンスハンドル
		windowClass_.hInstance,
		//オプション
		nullptr					
	);

		
	
}

void Elysia::WindowsSetup::DisplayWindow() {
	//ウィンドウを表示
	ShowWindow(hwnd_, SW_SHOW);
}

#pragma endregion

void Elysia::WindowsSetup::Initialize(const wchar_t* title, const int32_t& clientWidth, const int32_t& clientHeight) {
	//値を入れる
	clientWidth_ = clientWidth;
	clientHeight_ = clientHeight;

	//システムタイマーの分解能を上げる
	timeBeginPeriod(1);

	//ウィンドウクラスを登録
	RegisterWindowsClass(title);

	//ウィンドウを表示
	DisplayWindow();
}

void Elysia::WindowsSetup::WindowsMSG(MSG& msg) {
	TranslateMessage(&msg);
	DispatchMessage(&msg);
}

void Elysia::WindowsSetup::Close() {
	//ウィンドウを閉じる
	CloseWindow(hwnd_);
}


