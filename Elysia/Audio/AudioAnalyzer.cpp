#include "AudioAnalyzer.h"
#include <Audio.h>

extern const CLSID CLSID_CustomAudioCapturerAPO;


AudioAnalyzer::AudioAnalyzer(){
    //インスタンスの取得
    //オーディオ
    audio_ = Elysia::Audio::GetInstance();
}

void AudioAnalyzer::Initialize(const uint32_t& audioHandle) {

    HRESULT hResult=CoCreateInstance(
        CLSID_CustomAudioCapturerAPO, // [1] 生成したいオブジェクトのCLSID
        NULL,                         // [2] 集約は使用しない
        CLSCTX_INPROC_SERVER,         // [3] インプロセスDLLとして実行
        IID_IXAPO,                    // [4] IXAPOインターフェースを取得
        (void**)&pAudioCapturerAPO_           // [5] IXAPO* ポインタを受け取る変数のアドレス
    );
	
    assert(hResult);


    // 2. エフェクトチェーンの作成
    XAUDIO2_EFFECT_DESCRIPTOR desc;
    desc.pEffect = pCustomApo;
    desc.InitialState = TRUE;
    desc.OutputChannels = /* ボイスの出力チャンネル数 */;

    XAUDIO2_EFFECT_CHAIN effectChain;
    effectChain.EffectCount = 1u;
    effectChain.pEffectDescriptors = &desc;

    // 3. ボイスにエフェクトを設定
    HRESULT hr = audioInformation_[fileKey].sourceVoice->SetEffectChain(&effectChain);

    // 2. APOのSourceVoiceへの登録
    // audioクラス内にIXAudio2SourceVoice*を取得し、SetEffectChainを呼び出す処理が必要
    bool success = audio_->SetEffectToVoice(audioHandle, pAudioCapturerAPO_.Get());
    

    // 3. 内部データ構造の準備
    spectrum_.resize(SPECTRUM_BIN_SIZE, 0.0f); // 初期化

    // 4. FFTスレッドの起動
    isRunning_ = true;
    fftThread_ = std::thread(&AudioAnalyzer::RunFFTThread, this);

}

void AudioAnalyzer::RunFFTThread()
{
}
