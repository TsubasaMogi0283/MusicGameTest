#pragma once

/**
 * @file Input.h
 * @brief 入力クラス
 * @author 茂木翼
 */

//ここでDirectXInputのバージョン設定をする
#define DIRECTINPUT_VERSION	0x0800

#include <cstdint>
#include <cmath>
#include <dinput.h>
#include <Xinput.h>
#include <wrl.h>
using namespace Microsoft::WRL;



#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"xinput.lib")

/// <summary>
/// ElysiaEngine
/// </summary>
namespace Elysia {

	/// <summary>
	/// ウィンドウズクラス
	/// </summary>
	class WindowsSetup;

	/// <summary>
	/// 入力
	/// </summary>
	class Input final {
	private:

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Input();

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Input() = default;

	public:

		/// <summary>
		/// インスタンスの取得
		/// </summary>
		/// <returns>インスタンス</returns>
		static Input* GetInstance();


		/// <summary>
		/// コピーコンストラクタ禁止
		/// </summary>
		/// <param name="input"></param>
		Input(const Input& input) = delete;

		/// <summary>
		/// 代入演算子を無効にする
		/// </summary>
		/// <param name="input"></param>
		/// <returns></returns>
		Input& operator=(const Input& input) = delete;

	public:

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// 更新
		/// </summary>
		void Update();


#pragma region キーボード
		/// <summary>
		///押しているかどうかを判定
		/// </summary>
		/// <param name="keyNumber">キー番号</param>
		/// <returns>状態</returns>
		bool IsPushKey(const uint8_t& keyNumber)const;

		/// <summary>
		/// 押した瞬間かどうかを判定
		/// </summary>
		/// <param name="keyNumber">キー番号</param>
		/// <returns>状態</returns>
		bool IsTriggerKey(const uint8_t& keyNumber)const;

		/// <summary>
		/// 離した瞬間かどうかを取得
		/// </summary>
		/// <param name="keyNumber">キー番号</param>
		/// <returns>状態</returns>
		bool IsReleaseKey(const uint8_t& keyNumber)const;

#pragma endregion

#pragma region マウス


		/// <summary>
		/// 押し続けているか判定
		/// </summary>
		/// <param name="keyNumber">キー番号</param>
		/// <returns>状態</returns>
		bool IsPushMouse(const uint32_t& keyNumber)const;

		/// <summary>
		/// 押した瞬間を判定
		/// </summary>
		/// <param name="keyNumber">キー番号</param>
		/// <returns>状態</returns>
		bool IsTriggerMouse(const uint32_t& keyNumber)const;

		/// <summary>
		/// 離した瞬間を判定
		/// </summary>
		/// <param name="mouseNumber">キー番号</param>
		/// <returns>状態</returns>
		bool IsReleaseMouse(const uint32_t& mouseNumber)const;

#pragma endregion

#pragma region コントローラー

		/// <summary>
		/// 現在の状態を取得
		/// </summary>
		/// <returns>現在の取得</returns>
		inline XINPUT_STATE GetCurrentState()const {
			return currentState_;
		}

		/// <summary>
		/// 押しているかどうかを判定
		/// </summary>
		/// <param name="button">ボタン</param>
		/// <returns>状態</returns>
		inline bool IsPushButton(const int32_t& button)const {
			return currentState_.Gamepad.wButtons & button;
		};

		/// <summary>
		/// 押した瞬間を判定する
		/// </summary>
		/// <param name="button">ボタン</param>
		/// <returns>状態</returns>
		inline bool IsTriggerButton(const int32_t& button) const {
			return (currentState_.Gamepad.wButtons & button) && !(preState_.Gamepad.wButtons & button);
		};

		/// <summary>
		/// 離した瞬間か判定する
		/// </summary>
		/// <param name="button">ボタン</param>
		/// <returns>状態</returns>
		inline bool IsReleaseButton(const int32_t& button)const {
			return  !(currentState_.Gamepad.wButtons & button) && (preState_.Gamepad.wButtons & button);
		};

		/// <summary>
		/// 振動の設定
		/// </summary>
		/// <param name="leftMotor">左のモーター</param>
		/// <param name="rightMotor">右のモーター</param>
		void SetVibration(const float_t& leftMotor, const float_t& rightMotor);

		/// <summary>
		/// 振動を止める
		/// </summary>
		void StopVibration();

#pragma endregion

		/// <summary>
		/// カーソルの表示・非表示
		/// </summary>
		/// <param name="isDisplay">見せるかどうか</param>
		inline void SetIsDisplayCursor(const bool& isDisplay) {
			ShowCursor(isDisplay);
		};

	private:
		//ウィンドウズクラス
		WindowsSetup* windowsSetup_ = nullptr;

	private:
		//DirectInputの初期化
		ComPtr<IDirectInput8> directInput_ = nullptr;
		//キーボードデバイスの生成
		ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
		//マウスのデバイスを生成
		ComPtr<IDirectInputDevice8> mouseDevice_ = nullptr;
		//コントローラーのデバイスを生成
		ComPtr<IDirectInputDevice8> joyStickDevice_ = nullptr;

		//全キーの入力状態を取得する
		BYTE preKey_[256] = {};
		BYTE currentKey_[256] = {};

		//コントローラーの入力状態
		XINPUT_STATE currentState_ = {};
		XINPUT_STATE preState_ = {};

		//マウスの入力状態を取得
		DIMOUSESTATE currentMouse_ = {};
		DIMOUSESTATE preMouse_ = {};

	};

};