#pragma once

/**
 * @file AudioTest.h
 * @brief オーディオアナライザーのクラス
 * @author 茂木翼
 */


#include <thread>
#include <mutex>

/// <summary>
/// ElysiaEngine
/// </summary>
namespace Elysia {
	/// <summary>
	/// オーディオ
	/// </summary>
	class Audio;
}

/// <summary>
/// オーディオアナライザー
/// </summary>
class AudioAnalyzer{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	AudioAnalyzer();

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
	~AudioAnalyzer()=default;
private:
	// ワーカースレッドで実行されるFFTのメインループ
	void RunFFTThread();


private:
	//オーディオ
	Elysia::Audio* audio_=nullptr;

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

