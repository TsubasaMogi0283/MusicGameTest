#include "Input.h"
#include "WindowsSetup.h"

Elysia::Input::Input() {
	//インスタンスの取得
	//ウィンドウズクラス
	windowsSetup_ = WindowsSetup::GetInstance();
}

Elysia::Input* Elysia::Input::GetInstance() {
	static Input instance;
	return &instance;
}

void Elysia::Input::Initialize() {
	
	//DirectInputオブジェクトの生成
	HRESULT hr = DirectInput8Create(
		windowsSetup_->GetHInstance(),
		DIRECTINPUT_VERSION, 
		IID_IDirectInput8, 
		(void**)&directInput_, nullptr);
	assert(SUCCEEDED(hr));


	//キーボードデバイスの生成
	hr = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(hr));
	//マウスデバイスの生成
	hr = directInput_->CreateDevice(GUID_SysMouse, &mouseDevice_, NULL);
	assert(SUCCEEDED(hr));
	//コントローラーデバイスの生成
	HRESULT joyStickDeviceHR = {};
	joyStickDeviceHR = directInput_->CreateDevice(GUID_Joystick, &joyStickDevice_, NULL);
	//コントローラーに接続していない時は作らない
	if (SUCCEEDED(joyStickDeviceHR) != true) {
		joyStickDevice_ = nullptr;
	}

	//入力データの形式のセット
	//標準形式
	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));
	//マウスの方の入力データ形式セット
	hr = mouseDevice_->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(hr));
	//コントローラーの方の入力データ形式セット
	if (SUCCEEDED(joyStickDeviceHR) == true) {
		joyStickDeviceHR = joyStickDevice_->SetDataFormat(&c_dfDIJoystick);
		assert(SUCCEEDED(joyStickDeviceHR));

	}
	

	//排他制御レベルのセット
	//
	//DISCL_FOREGROUND...画面が手前にある場合のみ入力を受け付ける
	//DISCL_NONEXCLUSIVE...デバイスをこのアプリだけで専有しない
	//DISCL_NOWINKEY...Windowsキーを無効にする

	hr = keyboard_->SetCooperativeLevel(windowsSetup_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));
	hr = mouseDevice_->SetCooperativeLevel(windowsSetup_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(hr));
	if (SUCCEEDED(joyStickDeviceHR) == true) {
		joyStickDeviceHR = joyStickDevice_->SetCooperativeLevel(WindowsSetup::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		assert(SUCCEEDED(joyStickDeviceHR));

	}

}

#pragma region キーボード

bool Elysia::Input::IsPushKey(const uint8_t& keyNumber) const{
	if (currentKey_[keyNumber] != 0) {
		return true;
	}
	return false;

}

bool Elysia::Input::IsTriggerKey(const uint8_t& keyNumber) const{
	if (currentKey_[keyNumber] != 0 && preKey_[keyNumber] == 0) {
		return true;
	}
	return false;
}

bool Elysia::Input::IsReleaseKey(const uint8_t& keyNumber) const{
	if (currentKey_[keyNumber] == 0 && preKey_[keyNumber] != 0) {
		return true;
	}
	return false;
}

#pragma endregion

#pragma region マウス
bool Elysia::Input::IsPushMouse(const uint32_t& mouseNumber)const {
	if (currentMouse_.rgbButtons[mouseNumber] != 0) {
		return true;
	}
	return false;
}


bool Elysia::Input::IsTriggerMouse(const uint32_t& mouseNumber)const {
	if (currentMouse_.rgbButtons[mouseNumber] != 0 && preMouse_.rgbButtons[mouseNumber] == 0) {
		return true;
	}
	return false;
}
bool Elysia::Input::IsReleaseMouse(const uint32_t& mouseNumber) const{
	if (currentMouse_.rgbButtons[mouseNumber] == 0 && preMouse_.rgbButtons[mouseNumber] != 0) {
		return true;
	}
	return false;
}
#pragma endregion

#pragma region コントローラー


void Elysia::Input::SetVibration(const float_t& leftMotor, const float_t& rightMotor){
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	//0から65535まで
	const float_t MAX_SIZE = 65535.0f;
	vibration = {
		.wLeftMotorSpeed = WORD(leftMotor * MAX_SIZE),
		.wRightMotorSpeed = WORD(rightMotor * MAX_SIZE) }
	;
	XInputSetState(0, &vibration);
}

void Elysia::Input::StopVibration(){
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration = {
		.wLeftMotorSpeed= WORD(0.0f),
		.wRightMotorSpeed= WORD(0.0f) }
	;
	XInputSetState(0, &vibration);
}



#pragma endregion

void Elysia::Input::Update() {

	//キーボード
	//前回のキー入力を保存
	memcpy(preKey_, currentKey_, sizeof(currentKey_));
	//情報の取得開始
	keyboard_->Acquire();
	keyboard_->GetDeviceState(sizeof(currentKey_), currentKey_);


	//マウス
	//キーと違ってそのまま代入で大丈夫だよ
	preMouse_ = currentMouse_;
	//情報の取得開始
	mouseDevice_->Acquire();
	mouseDevice_->GetDeviceState(sizeof(DIMOUSESTATE), &currentMouse_);

	//コントローラー
	
	

	//再初期化
	//接続されたらまたデバイスを作り直す
	if (joyStickDevice_ == nullptr) {
		HRESULT hr = directInput_->CreateDevice(GUID_Joystick, &joyStickDevice_, NULL);
		if (SUCCEEDED(hr)) {
			hr = joyStickDevice_->SetDataFormat(&c_dfDIJoystick);
			assert(SUCCEEDED(hr));
			hr = joyStickDevice_->SetCooperativeLevel(windowsSetup_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
			assert(SUCCEEDED(hr));
		}
	}
	else {
		//前回の状態を保存
		preState_ = currentState_;

		//情報の取得開始
		if (joyStickDevice_ != nullptr) {
			joyStickDevice_->Acquire();
		}

	}
	//状態を取得
	if (XInputGetState(0, &currentState_) != ERROR_SUCCESS) {
		//コントローラーが接続されていない場合
		ZeroMemory(&currentState_, sizeof(XINPUT_STATE));
	}
}
