#pragma once

/**
 * @file Audio.h
 * @brief オーディオクラス
 * @author 茂木翼
 */
#define XAUDIO2_HELPER_FUNCTIONS
#include <xaudio2.h>
#include "xaudio2fx.h"
#include <fstream>
#include <x3daudio.h>
#include <mmsystem.h>
#include <cassert>
#include <array>
#include <map>
#include <thread>
#include <mutex>
#include <complex>
#include <vector>
#include <xapo.h>
#include <XAPOFX.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib,"xaudio2.lib")

 //MediaFundation
#include <mfapi.h>
#include <mfidl.h>

#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

#include <wrl.h>
using Microsoft::WRL::ComPtr;


#include "AudioInformation.h"


namespace Elysia {
	/// <summary>
	/// エフェクトのタイプ。LoadAudioの時に使う
	/// </summary>
	enum EffectType {
		Filter,
		Reverb,
	};

	/// <summary>
	/// オーディオ
	/// </summary>
	class Audio final {
	private:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		Audio() = default;

		/// <summary>
		/// デストラクタ
		/// </summary>
		~Audio() = default;
	public:

		/// <summary>
		/// インスタンスの取得
		/// </summary>
		/// <returns></returns>
		static Audio* GetInstance();

		/// <summary>
		/// コピーコンストラクタ禁止
		/// </summary>
		/// <param name="audio"></param>
		Audio(const Audio& audio) = delete;

		/// <summary>
		/// 代入演算子を無効にする
		/// </summary>
		/// <param name="audio"></param>
		/// <returns></returns>
		Audio& operator=(const Audio& audio) = delete;

	public:


		/// <summary>
		/// 初期化。これはDirecX初期化の後に入れてね
		/// </summary>
		void Initialize();

#pragma region 基本セット

		/// <summary>
		/// 読み込み
		/// </summary>
		/// <param name="fileName"></param>
		/// <returns></returns>
		uint32_t Load(const std::string& fileName);

		/// <summary>
		/// 読み込み(MP3)
		/// </summary>
		/// <param name="fileName"></param>
		/// <returns></returns>
		uint32_t LoadWave(const std::string& fileName);


		/// <summary>
		/// 読み込み(Wave)
		/// </summary>
		uint32_t LoadMP3(const std::string& fileName);

		/// <summary>
		/// 再生
		/// </summary>
		/// <param name="audioHandle"></param>
		/// <param name="isLoop"></param>
		void Play(const uint32_t& audioHandle, const bool& isLoop);

		/// <summary>
		/// 再生
		/// </summary>
		void Play(const uint32_t& audioHandle, const uint32_t& loopCount);

	private:

		/// <summary>
		/// 再生
		/// </summary>
		/// <param name="audioHandle"></param>
		/// <param name="isLoop"></param>
		void PlayWave(const uint32_t& audioHandle, const bool& isLoop);


		/// <summary>
		/// 再生(ループ回数あり)
		/// </summary>
		/// <param name="audioHandle"></param>
		/// <param name="loopCount"></param>
		void PlayWave(const uint32_t& audioHandle, const uint32_t& loopCount);

		/// <summary>
		/// 再生
		/// </summary>
		/// <param name="audioHandle"></param>
		/// <param name="isLoop"></param>
		void PlayMP3(const uint32_t& audioHandle, const  bool& isLoop);

		/// <summary>
		/// 再生(ループ回数あり)
		/// </summary>
		/// <param name="audioHandle"></param>
		/// <param name="loopCount"></param>
		void PlayMP3(const uint32_t& audioHandle, const uint32_t& loopCount);

	public:

		/// <summary>
		/// 一時停止
		/// </summary>
		/// <param name="ハンドル名"></param>
		void PauseWave(const uint32_t& audioHandle);

		/// <summary>
		/// 再開
		/// </summary>
		/// <param name="ハンドル名"></param>
		void ResumeWave(const uint32_t& audioHandle);


		/// <summary>
		/// 停止
		/// </summary>
		/// <param name="audioHandle"></param>
		void Stop(const uint32_t& audioHandle);

#pragma endregion


#pragma region ループ


		/// <summary>
		/// ループから抜ける
		/// </summary>
		/// <param name="ハンドル名"></param>
		void ExitLoop(const uint32_t& audioHandle);


		/// <summary>
		/// 再生(後半ループ)
		/// </summary>
		/// <param name="ハンドル名"></param>
		/// <param name="始まる位置(秒)"></param>
		void AfterLoopPlayWave(const uint32_t& audioHandle, const float_t& second);

		/// <summary>
		/// 再生(前半ループ)
		/// </summary>
		/// <param name="ハンドル名"></param>
		/// <param name="ループ間の長さ(秒)"></param>
		void BeforeLoopPlayWave(const uint32_t& audioHandle, const float_t& lengthSecond);


		/// <summary>
		/// 部分ループ
		/// </summary>
		/// <param name="ハンドル名"></param>
		/// <param name="開始位置(秒)"></param>
		/// <param name="ループの長さ(秒)"></param>
		void PartlyLoopPlayWave(const uint32_t& audioHandle, const float_t& start, const float_t& lengthSecond);


#pragma endregion

		/// <summary>
		/// 音量調節
		/// </summary>
		/// <param name="ハンドル名"></param>
		/// <param name="音量"></param>
		void ChangeVolume(const uint32_t& audioHandle, const float_t& volume);

#pragma region ピッチ系

		/// <summary>
		/// 音の高さの変更(滑らか版)
		/// </summary>
		/// <param name="ハンドル名"></param>
		/// <param name="値"></param>
		void ChangeFrequencyRatio(const uint32_t& audioHandle, float_t& ratio_);


		/// <summary>
		/// ピッチの設定(段階的)
		/// </summary>
		void ChangePitch(const uint32_t& audioHandle, const int32_t& scale);


#pragma endregion

		/// <summary>
		/// Pan振り
		/// </summary>
		/// <param name="ハンドル名"></param>
		/// <param name="Panの値"></param>
		void SetPan(const uint32_t& audioHandle, const float_t& pan);


#pragma region フィルター

		//単極は必要無いかな

		/// <summary>
		/// ローパスフィルター
		/// </summary>
		/// <param name="ハンドル名"></param>
		/// <param name="CutOffの値"></param>
		void SetLowPassFilter(const uint32_t& audioHandle, float_t& cutOff);

		/// <summary>
		/// ローパスフィルター(Qあり)
		/// </summary>
		/// <param name="ハンドル名"></param>
		/// <param name="CutOffの値"></param>
		/// <param name="oneOverQ"></param>
		void SetLowPassFilter(const uint32_t& audioHandle, float_t& cutOff, const float_t& oneOverQ);



		/// <summary>
		/// ハイパスフィルター
		/// </summary>
		/// <param name="ハンドル名"></param>
		/// <param name="CutOffの値"></param>
		void SetHighPassFilter(const uint32_t& audioHandlee, float_t& cutOff);

		/// <summary>
		/// ハイパスフィルター(Qあり)
		/// </summary>
		/// <param name="ハンドル名"></param>
		/// <param name="CutOffの値"></param>
		/// <param name="oneOverQ"></param>
		void SetHighPassFilter(const uint32_t& audioHandle, float_t& cutOff, const float_t& oneOverQ);

		/// <summary>
		/// バンドパス
		/// </summary>
		/// <param name="ハンドル名"></param>
		/// <param name="CutOffの値"></param>
		void SetBandPassFilter(const uint32_t& audioHandle, float_t& cutOff);

		/// <summary>
		/// バンドパス(Qあり)
		/// </summary>
		/// <param name="ハンドル名"></param>
		/// <param name="CutOffの値"></param>
		void SetBandPassFilter(const uint32_t& audioHandle, float_t& cutOff, const float_t& oneOverQ);


		/// <summary>
		/// ノッチフィルター
		/// </summary>
		/// <param name="ハンドル名"></param>
		/// <param name="CutOffの値"></param>
		void SetNotchFilter(const uint32_t& audioHandle, float_t& cutOff);

		/// <summary>
		/// ノッチフィルター
		/// </summary>
		/// <param name="audioHandle">ハンドル</param>
		/// <param name="cutOff">カットオフ</param>
		/// <param name="oneOverQ">Q</param>
		void SetNotchFilter(const uint32_t& audioHandle, float_t& cutOff, const float_t& oneOverQ);

#pragma endregion
		/// <summary>
		/// bitの数を取得
		/// </summary>
		/// <param name="audioHandle">ハンドル</param>
		/// <returns></returns>
		int32_t GetBitPerSample(const uint32_t& audioHandle);

		/// <summary>
		/// 現在再生している時間を取得
		/// </summary>
		/// <param name="audioHandle">ハンドル</param>
		/// <returns></returns>
		float_t GetPlayCurrentTime(const uint32_t& audioHandle);

		/// <summary>
		/// サブミックスボイスの作成
		/// </summary>
		/// <param name="channel"></param>
		void CreateSubmixVoice(const uint32_t& channel);

		/// <summary>
		/// 指定したチャンネルに送る
		/// </summary>
		/// <param name="audioHandle"></param>
		/// <param name="channelNumber"></param>
		void SendChannels(const uint32_t& audioHandle, const uint32_t& channelNumber);

		/// <summary>
		/// エフェクトの効果を無効にする
		/// </summary>
		/// <param name="audioHandle"></param>
		void OffEffect(const uint32_t& audioHandle);

		/// <summary>
		/// エフェクトの効果を有効にする
		/// </summary>
		/// <param name="audioHandle"></param>
		void OnEffect(const uint32_t& audioHandle);


		/// <summary>
		/// 解放
		/// </summary>
		void Finalize();


	private:
		/// <summary>
		/// アナライザー
		/// </summary>
		class Analyzer {
		public:
			/// <summary>
			/// コンストラクタ
			/// </summary>
			Analyzer();

			/// <summary>
			/// 初期化
			/// </summary>
			void Initialize(const uint32_t& audioHandle);

			/// <summary>
			/// 更新
			/// </summary>
			void Update();


			/// <summary>
			/// デストラクタ
			/// </summary>
			~Analyzer() = default;
		private:
			// ワーカースレッドで実行されるFFTのメインループ
			void RunFFTThread();


		private:
			//オーディオ
			Elysia::Audio* audio_ = nullptr;

		private:
			// 1. XAudio2関連
			// HACK: IXAPOのインスタンス。本来はComPtrで管理すべき
			Microsoft::WRL::ComPtr<IXAPO> pAudioCapturerAPO_;

			// 2. FFT処理関連
			std::thread fftThread_; // FFTを実行するワーカースレッド
			std::atomic<bool> isRunning_; // スレッド実行フラグ

			// 3. データと同期
			std::vector<float> spectrum_; // 最新のスペクトル結果 (FFTの振幅)
			std::mutex spectrumMutex_;    // spectrum_へのアクセスを保護するミューテックス

			// 4. データキャプチャキュー (APOとFFTスレッド間の通信路)
			// HACK: ここでは簡単のためにstd::vectorで表現
			std::vector<float> pcmDataQueue_;
			std::mutex queueMutex_;


			// FFTのパラメータ
			static constexpr int FFT_SIZE = 2048; // FFTの窓サイズ
			static constexpr int SPECTRUM_BIN_SIZE = FFT_SIZE / 2; // 周波数ビンの数



		};
	private:

		/// <summary>
		/// 指定したハンドルが入っているfilePathを取得する
		/// </summary>
		/// <param name="handle"></param>
		/// <returns></returns>
		inline std::string GetAudioInformationKey(const uint32_t& handle) {
			for (const auto& [filePath, modelInfo] : audioInformation_) {
				if (modelInfo.handle == handle) {
					return filePath;
				}
			}

			//無かったら空を返す
			return {};
		}

	private:

		//自分のエンジンではA4は442Hz基準にする
		//もちろん12段階で1オクターブ
		static const int32_t SCALE_AMOUNT_ = 13;
		const float_t SEMITONE_RATIO_[SCALE_AMOUNT_] = {
			1.00000f, //C4
			1.05946f, //C#4
			1.12246f, //D4
			1.18921f, //D#4
			1.25992f, //E4
			1.33483f, //F4
			1.41421f, //F#4
			1.49831f, //G4
			1.58740f, //G#4
			1.68179f, //A4
			1.78180f, //A#4
			1.88775f, //B4
			2.00000f  //C5
		};

		//低い方
		const float_t MINUS_SEMITONE_RATION[SCALE_AMOUNT_] = {
			1.00000f,	//C4
			0.94387f,	//B3
			0.89090f,	//A3#
			0.84090f,	//A3
			0.79370f,	//G#3
			0.74915f,	//G3
			0.70711f,	//F#3
			0.66742f,	//F3
			0.62996f,	//E3
			0.59460f,	//D#3
			0.56123f,	//D3
			0.52973f,	//C#3
			0.50000f,	//C3
		};



	private:
		//IXAudio2はCOMオブジェクトなのでComPtr管理
		ComPtr<IXAudio2> xAudio2_ = nullptr;
		//マスターボイス
		//最終的にここでまとめるよ(スピーカーみたいな感じだね)
		IXAudio2MasteringVoice* masterVoice_ = nullptr;


		static uint32_t index_;

		//Panに必要な変数
		DWORD dwChannelMask_ = {};
		float_t outputMatrix_[8] = {};

		//Reverb
		IUnknown* pXAPO_ = nullptr;

		//3Dオーディオ
		X3DAUDIO_HANDLE x3DInstance_ = {};
		//リスナー
		X3DAUDIO_LISTENER listener_ = {};
		//エミッタ
		X3DAUDIO_EMITTER emitter_ = {};

		//オーディオに関するものが入っている
		std::map<std::string, AudioInformation>audioInformation_{};

		//サブミックスボイス
		static const uint32_t SUBMIXVOICE_AMOUNT_ = 64u;
		std::array<IXAudio2SubmixVoice*, SUBMIXVOICE_AMOUNT_> submixVoice_{};

	};

}


