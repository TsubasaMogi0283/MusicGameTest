#include "Audio.h"


uint32_t Elysia::Audio::index_ = 0u;


Elysia::Audio* Elysia::Audio::GetInstance() {
	static Elysia::Audio instance;
	return &instance;
}



void Elysia::Audio::CreateSubmixVoice(const uint32_t& channel) {
	//44100Hz固定
	uint32_t sampleRate = 44100u;

	//サブミックスボイスの作成
	HRESULT hResult = Elysia::Audio::GetInstance()->xAudio2_->CreateSubmixVoice(&Elysia::Audio::GetInstance()->submixVoice_[channel], channel, sampleRate);
	assert(SUCCEEDED(hResult));

}


void Elysia::Audio::Initialize() {
	// Media Foundation の初期化
	HRESULT hResult = MFStartup(MF_VERSION);
	assert(SUCCEEDED(hResult));

	//XAudioのエンジンのインスタンスを生成
	hResult = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hResult));

	//マスターボイスを生成
	hResult = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hResult));

	//スピーカ構成を取得
	masterVoice_->GetChannelMask(&dwChannelMask_);


	//X3DAudioを初期化
	hResult = X3DAudioInitialize(dwChannelMask_, X3DAUDIO_SPEED_OF_SOUND, x3DInstance_);
	assert(SUCCEEDED(hResult));


	//一度全部0に初期化
	for (int32_t i = 0; i < 8; i++) {
		outputMatrix_[i] = 0.0f;
	}

	//サブミックスボイス(DTMでのバス)をここで作る
	for (uint32_t i = 1u; i < SUBMIXVOICE_AMOUNT_; ++i) {
		CreateSubmixVoice(i);
	}

	//聞き手
	emitter_.ChannelCount = 1u;
	emitter_.CurveDistanceScaler = 1.0f;
	emitter_.DopplerScaler = 1.0f;
}



#pragma region 実際に使う関数

#pragma region 基本セット
uint32_t Elysia::Audio::Load(const std::string& fileName) {

	//一度読み込んだものは２度読み込まず返すだけ
	if (Elysia::Audio::GetInstance()->audioInformation_.find(fileName) != Elysia::Audio::GetInstance()->audioInformation_.end()) {
		return Elysia::Audio::GetInstance()->audioInformation_[fileName].handle;
	}

	//拡張子を探す
	size_t dotPosition = fileName.find('.');
	std::string extension = {};
	if (dotPosition != std::string::npos) {
		//「/」から最後まで
		extension = fileName.substr(dotPosition + 1);
	}


	//返す値
	uint32_t result = 0u;

	//wavの場合
	if (extension == "wav") {
		return result = LoadWave(fileName);
	}
	//mp3の場合
	else if (extension == "mp3") {
		return result = LoadMP3(fileName);
	}


	//無かったら空を返す
	return result;

}
//読み込み
uint32_t Elysia::Audio::LoadWave(const std::string& fileName) {
	//一度読み込んだものは２度読み込まず返すだけ
	if (Elysia::Audio::GetInstance()->audioInformation_.find(fileName) != Elysia::Audio::GetInstance()->audioInformation_.end()) {
		return Elysia::Audio::GetInstance()->audioInformation_[fileName].handle;
	}


	//indexを取得
	uint32_t handle = index_;
	//加算
	++index_;


#pragma region １,ファイルオープン
	//ファイル入力ストリームのインスタンス
	std::ifstream file;
	//.wavファイルをバイナリモードで開く
	file.open(fileName, std::ios_base::binary);
	//ファイルオープン失敗を検出する
	assert(file.is_open());

#pragma endregion

#pragma region ２,wavデータ読み込み

	//RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	//Formatチャンクの読み込み
	FormatChunk format = {};
	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	//何かここ空白入れないとダメらしい
	//後ろが4だからかな・・
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}

	//チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);


	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		//読み込み位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));

	}

	//メインのデータチャンク
	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	//Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);
#pragma endregion

#pragma region ３,Waveファイルを閉じる
	file.close();

#pragma endregion
#pragma region 読み込んだ音声データを返す

	//波形フォーマットを基にSourceVoiceの生成
	HRESULT hResult = Elysia::Audio::GetInstance()->xAudio2_->CreateSourceVoice(
		&Elysia::Audio::GetInstance()->audioInformation_[fileName].sourceVoice,
		&format.fmt);
	assert(SUCCEEDED(hResult));

	SoundData newSoundData = {

		.wfex = format.fmt,
		.pBuffer = reinterpret_cast<BYTE*>(pBuffer),
		.bufferSize = data.size,
	};

	//記録
	Elysia::Audio::GetInstance()->audioInformation_[fileName].fileName = fileName;
	Elysia::Audio::GetInstance()->audioInformation_[fileName].handle = handle;
	Elysia::Audio::GetInstance()->audioInformation_[fileName].soundData = newSoundData;
	Elysia::Audio::GetInstance()->audioInformation_[fileName].extension = "wave";

	//handleを返す
	return handle;

#pragma endregion

}

uint32_t Elysia::Audio::LoadMP3(const std::string& fileName) {


	//一度読み込んだものは２度読み込まず返すだけ
	if (Elysia::Audio::GetInstance()->audioInformation_.find(fileName) != Elysia::Audio::GetInstance()->audioInformation_.end()) {
		return Elysia::Audio::GetInstance()->audioInformation_[fileName].handle;
	}


	//indexを取得
	uint32_t handle = index_;
	//加算
	index_++;


	//記録
	Elysia::Audio::GetInstance()->audioInformation_[fileName].fileName = fileName;
	Elysia::Audio::GetInstance()->audioInformation_[fileName].handle = handle;
	Elysia::Audio::GetInstance()->audioInformation_[fileName].extension = "mp3";


	//stringからLPCWCHARに変換する
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, fileName.c_str(), (int)fileName.size(), NULL, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, fileName.c_str(), (int)fileName.size(), &wstr[0], size_needed);
	LPCWSTR lpcWString = wstr.c_str();


	//ソースリーダーの作成
	HRESULT hResult = MFCreateSourceReaderFromURL(lpcWString, nullptr, &Elysia::Audio::GetInstance()->audioInformation_[fileName].sourceReader);
	assert(SUCCEEDED(hResult));

	//メディアタイプの取得
	IMFMediaType* pMFMediaType{ nullptr };
	MFCreateMediaType(&pMFMediaType);
	pMFMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pMFMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	Elysia::Audio::GetInstance()->audioInformation_[fileName].sourceReader->SetCurrentMediaType(DWORD(MF_SOURCE_READER_FIRST_AUDIO_STREAM), nullptr, pMFMediaType);

	pMFMediaType->Release();
	pMFMediaType = nullptr;
	Elysia::Audio::GetInstance()->audioInformation_[fileName].sourceReader->GetCurrentMediaType(DWORD(MF_SOURCE_READER_FIRST_AUDIO_STREAM), &pMFMediaType);

	//オーディオデータ形式の作成
	WAVEFORMATEX* waveFormat{};
	MFCreateWaveFormatExFromMFMediaType(pMFMediaType, &waveFormat, nullptr);

	while (true)
	{
		IMFSample* pMFSample{ nullptr };
		DWORD dwStreamFlags{ 0 };
		Audio::GetInstance()->audioInformation_[fileName].sourceReader->ReadSample(DWORD(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, nullptr, &dwStreamFlags, nullptr, &pMFSample);

		if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM)
		{
			break;
		}

		IMFMediaBuffer* pMFMediaBuffer{ nullptr };
		pMFSample->ConvertToContiguousBuffer(&pMFMediaBuffer);

		BYTE* pBuffer{ nullptr };
		DWORD cbCurrentLength{ 0 };
		pMFMediaBuffer->Lock(&pBuffer, nullptr, &cbCurrentLength);

		Elysia::Audio::GetInstance()->audioInformation_[fileName].mediaData.resize(Elysia::Audio::GetInstance()->audioInformation_[fileName].mediaData.size() + cbCurrentLength);
		memcpy(Elysia::Audio::GetInstance()->audioInformation_[fileName].mediaData.data() + Elysia::Audio::GetInstance()->audioInformation_[fileName].mediaData.size() - cbCurrentLength, pBuffer, cbCurrentLength);

		pMFMediaBuffer->Unlock();

		pMFMediaBuffer->Release();
		pMFSample->Release();
	}

	Elysia::Audio::GetInstance()->xAudio2_->CreateSourceVoice(&Elysia::Audio::GetInstance()->audioInformation_[fileName].sourceVoice, waveFormat);


	return handle;
}

void Elysia::Audio::Play(const uint32_t& audioHandle, const bool& isLoop) {
	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	if (audioInformation_[fileKey].extension == "wave") {
		PlayWave(audioHandle, isLoop);
	}
	else if (audioInformation_[fileKey].extension == "mp3") {
		PlayMP3(audioHandle, isLoop);
	}
}

void Elysia::Audio::Play(const uint32_t& audioHandle, const uint32_t& loopCount) {
	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);
	//また再生するときに最初から再生されるように一旦止める
	HRESULT hResult = audioInformation_[fileKey].sourceVoice->Stop();
	assert(SUCCEEDED(hResult));

	if (audioInformation_[fileKey].extension == "wave") {
		PlayWave(audioHandle, loopCount);
	}
	else if (audioInformation_[fileKey].extension == "mp3") {
		PlayMP3(audioHandle, loopCount);
	}

}


void Elysia::Audio::PlayMP3(const uint32_t& audioHandle, const bool& isLoop) {
	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	HRESULT hResult = audioInformation_[fileKey].sourceVoice->FlushSourceBuffers();
	assert(SUCCEEDED(hResult));

	//bufferの設定
	XAUDIO2_BUFFER buffer{};
	buffer.pAudioData = Audio::GetInstance()->audioInformation_[fileKey].mediaData.data();
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.AudioBytes = sizeof(BYTE) * static_cast<UINT32>(Elysia::Audio::GetInstance()->audioInformation_[fileKey].mediaData.size());
	if (isLoop == true) {
		//ずっとループさせたいならLoopCountにXAUDIO2_LOOP_INFINITEをいれよう
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	else {
		buffer.LoopCount = XAUDIO2_NO_LOOP_REGION;
	}


	hResult = Elysia::Audio::GetInstance()->audioInformation_[fileKey].sourceVoice->SubmitSourceBuffer(&buffer);
	assert(SUCCEEDED(hResult));

	//波形データの再生
	hResult = audioInformation_[fileKey].sourceVoice->Start(0);
	assert(SUCCEEDED(hResult));
}

void Elysia::Audio::PlayMP3(const uint32_t& audioHandle, const uint32_t& loopCount) {
	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	//同じものが再生されていたら一旦止める
	HRESULT hResult = audioInformation_[fileKey].sourceVoice->Stop();
	assert(SUCCEEDED(hResult));

	hResult = audioInformation_[fileKey].sourceVoice->FlushSourceBuffers();
	assert(SUCCEEDED(hResult));

	//bufferの設定
	XAUDIO2_BUFFER buffer{};
	buffer.pAudioData = Audio::GetInstance()->audioInformation_[fileKey].mediaData.data();
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.AudioBytes = sizeof(BYTE) * static_cast<UINT32>(Elysia::Audio::GetInstance()->audioInformation_[fileKey].mediaData.size());
	//ここでループ回数を設定
	//1回多くなっているので-1してあげた方が良いかも
	//1でfalseの場合と同じ
	buffer.LoopCount = loopCount - 1u;

	hResult = Elysia::Audio::GetInstance()->audioInformation_[fileKey].sourceVoice->SubmitSourceBuffer(&buffer);
	assert(SUCCEEDED(hResult));

	//波形データの再生
	hResult = audioInformation_[fileKey].sourceVoice->Start(0);
	assert(SUCCEEDED(hResult));
}

void Elysia::Audio::PlayWave(const uint32_t& audioHandle, const bool& isLoop) {
	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	//同じものが再生されていたら一旦止める
	HRESULT hResult = audioInformation_[fileKey].sourceVoice->Stop();
	assert(SUCCEEDED(hResult));

	hResult = audioInformation_[fileKey].sourceVoice->FlushSourceBuffers();
	assert(SUCCEEDED(hResult));
	//再生する波形データの設定
	XAUDIO2_BUFFER buffer{};
	buffer.pAudioData = audioInformation_[fileKey].soundData.pBuffer;
	buffer.AudioBytes = audioInformation_[fileKey].soundData.bufferSize;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	if (isLoop == true) {
		//ずっとループさせたいならLoopCountにXAUDIO2_LOOP_INFINITEをいれよう
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	if (isLoop == false) {
		buffer.LoopCount = XAUDIO2_NO_LOOP_REGION;
	}


	//Buffer登録
	hResult = audioInformation_[fileKey].sourceVoice->SubmitSourceBuffer(&buffer);
	assert(SUCCEEDED(hResult));
	//波形データの再生
	hResult = audioInformation_[fileKey].sourceVoice->Start();
	assert(SUCCEEDED(hResult));
}

//ループ回数設定版
void Elysia::Audio::PlayWave(const uint32_t& audioHandle, const uint32_t& loopCount) {
	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	//同じものが再生されていたら一旦止める
	HRESULT hResult = audioInformation_[fileKey].sourceVoice->Stop();
	assert(SUCCEEDED(hResult));

	//再生する波形データの設定
	XAUDIO2_BUFFER buffer{};
	buffer.pAudioData = audioInformation_[fileKey].soundData.pBuffer;
	buffer.AudioBytes = audioInformation_[fileKey].soundData.bufferSize;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	//ここでループ回数を設定
	//1回多くなっているので-1してあげた方が良い
	buffer.LoopCount = loopCount - 1;

	//Buffer登録
	hResult = audioInformation_[fileKey].sourceVoice->SubmitSourceBuffer(&buffer);
	assert(SUCCEEDED(hResult));
	//波形データの再生
	hResult = audioInformation_[fileKey].sourceVoice->Start();
	assert(SUCCEEDED(hResult));
}

void Elysia::Audio::PauseWave(const uint32_t& audioHandle) {
	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	//いきなり停止させて残響とかのエフェクトも停止させたら違和感ある
	//だからXAUDIO2_PLAY_TAILSを入れて余韻も残す
	HRESULT hResult = audioInformation_[fileKey].sourceVoice->Stop(XAUDIO2_PLAY_TAILS);
	assert(SUCCEEDED(hResult));
}

void Elysia::Audio::ResumeWave(const uint32_t& audioHandle) {
	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	//波形データの再生
	HRESULT hResult = audioInformation_[fileKey].sourceVoice->Start(0);
	assert(SUCCEEDED(hResult));
}

void Elysia::Audio::Stop(const uint32_t& audioHandle) {
	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	HRESULT hResult = audioInformation_[fileKey].sourceVoice->Stop();
	assert(SUCCEEDED(hResult));
}

#pragma endregion


#pragma region ループ
void Elysia::Audio::ExitLoop(const uint32_t& audioHandle) {

	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);
	//ExitLoop関数でループを抜ける
	HRESULT hResult = audioInformation_[fileKey].sourceVoice->ExitLoop();
	assert(SUCCEEDED(hResult));
}


void Elysia::Audio::AfterLoopPlayWave(const uint32_t& audioHandle, const float_t& second) {
	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	//後半ループするよ
	//再生する波形データの設定
	XAUDIO2_BUFFER buffer{};
	buffer.pAudioData = audioInformation_[fileKey].soundData.pBuffer;
	buffer.AudioBytes = audioInformation_[fileKey].soundData.bufferSize;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	//ここでループ回数を設定
	buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

	//長いので新しく変数を作って分かりやすくする
	int32_t samplingRate = Elysia::Audio::GetInstance()->audioInformation_[fileKey].soundData.wfex.nSamplesPerSec;

	//ここでループしたい位置を設定してあげる
	buffer.LoopBegin = uint32_t(second * samplingRate);

	//Buffer登録
	HRESULT hResult = audioInformation_[fileKey].sourceVoice->SubmitSourceBuffer(&buffer);
	assert(SUCCEEDED(hResult));

	//波形データの再生
	hResult = audioInformation_[fileKey].sourceVoice->Start(0);
	assert(SUCCEEDED(hResult));

}


void Elysia::Audio::BeforeLoopPlayWave(const uint32_t& audioHandle, const float_t& lengthSecond) {
	//こっちは前半でループ

	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	//再生する波形データの設定
	XAUDIO2_BUFFER buffer{};
	buffer.pAudioData = audioInformation_[fileKey].soundData.pBuffer;
	buffer.AudioBytes = audioInformation_[fileKey].soundData.bufferSize;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	//ここでループ回数を設定
	buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

	//長いので新しく変数を作って分かりやすくする
	int32_t samplingRate = Elysia::Audio::GetInstance()->audioInformation_[fileKey].soundData.wfex.nSamplesPerSec;

	//ここでループしたい位置を設定してあげる
	buffer.LoopBegin = 0;
	buffer.LoopLength = static_cast<uint32_t>(lengthSecond * samplingRate);

	//Buffer登録
	HRESULT hResult = audioInformation_[fileKey].sourceVoice->SubmitSourceBuffer(&buffer);
	assert(SUCCEEDED(hResult));

	//波形データの再生
	hResult = audioInformation_[fileKey].sourceVoice->Start(0);
	assert(SUCCEEDED(hResult));

}


void Elysia::Audio::PartlyLoopPlayWave(const uint32_t& audioHandle, const float_t& start, const float_t& lengthSecond) {
	//別名サスティンループというらしい
	//シンセとかにあるサスティンと関係があるのかな
	//こっちは前半でループ

	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);


	//再生する波形データの設定
	XAUDIO2_BUFFER buffer{};
	buffer.pAudioData = audioInformation_[fileKey].soundData.pBuffer;
	buffer.AudioBytes = audioInformation_[fileKey].soundData.bufferSize;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	//ここでループ回数を設定
	buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

	//長いので新しく変数を作って分かりやすくする
	int32_t samplingRate = Elysia::Audio::GetInstance()->audioInformation_[fileKey].soundData.wfex.nSamplesPerSec;

	//ここでループしたい位置を設定してあげる
	buffer.LoopBegin = static_cast<uint32_t>(start * samplingRate);
	buffer.LoopLength = static_cast<uint32_t>(lengthSecond * samplingRate);

	//Buffer登録
	HRESULT hResult = audioInformation_[fileKey].sourceVoice->SubmitSourceBuffer(&buffer);
	assert(SUCCEEDED(hResult));

	//波形データの再生
	hResult = audioInformation_[fileKey].sourceVoice->Start(0);
	assert(SUCCEEDED(hResult));

}

#pragma endregion

void Elysia::Audio::ChangeVolume(const uint32_t& audioHandle, const float_t& volume) {

	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	HRESULT hResult = audioInformation_[fileKey].sourceVoice->SetVolume(volume);
	assert(SUCCEEDED(hResult));
}

void Elysia::Audio::ChangeFrequencyRatio(const uint32_t& audioHandle, float_t& ratio_) {

	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	HRESULT hResult = audioInformation_[fileKey].sourceVoice->SetFrequencyRatio(ratio_);
	assert(SUCCEEDED(hResult));
}

void Elysia::Audio::ChangePitch(const uint32_t& audioHandle, const int32_t& scale) {

	float_t ratio = 1.0f;

	//入力された値がプラスだった場合
	if (scale >= 0) {
		//プラスのピッチの方を探す
		for (int32_t i = 0; i < SCALE_AMOUNT_; i++) {
			if (scale == i) {
				ratio = SEMITONE_RATIO_[i];
				break;
			}

		}
		//12以上は上がらなかった
		if (scale > 12) {
			ratio = SEMITONE_RATIO_[12];
		}
	}
	//入力された値がマイナスだった場合
	else if (scale < 0) {
		//マイナスのピッチの方を探す
		for (int32_t i = 0; i < SCALE_AMOUNT_; i++) {
			if (scale == -i) {
				ratio = MINUS_SEMITONE_RATION[i];
				break;
			}

		}
		//-12以上は下がらなかった
		if (scale < -12) {
			ratio = MINUS_SEMITONE_RATION[12];
		}
	}

	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	//比率の設定
	HRESULT hResult = audioInformation_[fileKey].sourceVoice->SetFrequencyRatio(ratio);
	assert(SUCCEEDED(hResult));
}

void Elysia::Audio::SetPan(const uint32_t& audioHandle, const float_t& pan) {

	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	//左右のスピーカー間の目的のパンに基づき送信レベルを計算
	audioInformation_[fileKey].left = 0.5f - pan / 2.0f;
	audioInformation_[fileKey].right = 0.5f + pan / 2.0f;
	switch (dwChannelMask_)
	{
	case SPEAKER_MONO:
		outputMatrix_[0] = 1.0f;
		break;
	case SPEAKER_STEREO:
	case SPEAKER_2POINT1:
	case SPEAKER_SURROUND:
		outputMatrix_[1] = audioInformation_[fileKey].left;
		outputMatrix_[2] = audioInformation_[fileKey].right;

		break;
	case SPEAKER_QUAD:
		outputMatrix_[0] = audioInformation_[fileKey].left;
		outputMatrix_[1] = audioInformation_[fileKey].right;
		outputMatrix_[2] = audioInformation_[fileKey].left;
		outputMatrix_[3] = audioInformation_[fileKey].right;
		break;
	case SPEAKER_4POINT1:
		outputMatrix_[0] = audioInformation_[fileKey].left;
		outputMatrix_[1] = audioInformation_[fileKey].right;
		outputMatrix_[3] = audioInformation_[fileKey].left;
		outputMatrix_[4] = audioInformation_[fileKey].right;
		break;
	case SPEAKER_5POINT1:
	case SPEAKER_7POINT1:
	case SPEAKER_5POINT1_SURROUND:
		outputMatrix_[0] = audioInformation_[fileKey].left;
		outputMatrix_[1] = audioInformation_[fileKey].right;
		outputMatrix_[4] = audioInformation_[fileKey].left;
		outputMatrix_[5] = audioInformation_[fileKey].right;
		break;
	case SPEAKER_7POINT1_SURROUND:
		outputMatrix_[0] = audioInformation_[fileKey].left;
		outputMatrix_[1] = audioInformation_[fileKey].right;
		outputMatrix_[4] = audioInformation_[fileKey].left;
		outputMatrix_[5] = audioInformation_[fileKey].right;
		outputMatrix_[6] = audioInformation_[fileKey].left;
		outputMatrix_[7] = audioInformation_[fileKey].right;
		break;
	}

	//詳細の取得
	XAUDIO2_VOICE_DETAILS voiceDetails;
	audioInformation_[fileKey].sourceVoice->GetVoiceDetails(&voiceDetails);

	//マスターの詳細を取得
	XAUDIO2_VOICE_DETAILS masterVoiiceDetails;
	masterVoice_->GetVoiceDetails(&masterVoiiceDetails);

	//OutPutMatrixに設定
	HRESULT hResult = audioInformation_[fileKey].sourceVoice->SetOutputMatrix(
		NULL, voiceDetails.InputChannels,
		masterVoiiceDetails.InputChannels,
		outputMatrix_);
	assert(SUCCEEDED(hResult));

}

void Elysia::Audio::SetLowPassFilter(const uint32_t& audioHandle, float_t& cutOff) {
	//いきなり効果アリにすると違和感あるよね
	//LowPassは最初「1.0f」にした方が良いかも
	cutOff = max(cutOff, 1.0f);
	cutOff = min(cutOff, 0.0f);

	//パラメータの詳細設定
	XAUDIO2_FILTER_PARAMETERS filterParams = {
		.Type = XAUDIO2_FILTER_TYPE::LowPassFilter,
		.Frequency = cutOff,
		.OneOverQ = 1.4142f,
	};

	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	//パラメータの設定
	HRESULT hResult = audioInformation_[fileKey].sourceVoice->SetFilterParameters(&filterParams);
	assert(SUCCEEDED(hResult));

}

void Elysia::Audio::SetLowPassFilter(const uint32_t& audioHandle, float_t& cutOff, const float_t& oneOverQ) {
	//いきなり効果アリにすると違和感あるよね
	//LowPassは最初「1.0f」にした方が良いかも
	cutOff = max(cutOff, 1.0f);
	cutOff = min(cutOff, 0.0f);

	//パラメータの詳細設定
	XAUDIO2_FILTER_PARAMETERS filterParams = {
		.Type = XAUDIO2_FILTER_TYPE::LowPassFilter,
		.Frequency = cutOff,
		.OneOverQ = oneOverQ,
	};

	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);


	//パラメータの設定
	HRESULT hResult = audioInformation_[fileKey].sourceVoice->SetFilterParameters(&filterParams);
	assert(SUCCEEDED(hResult));
}

void Elysia::Audio::SetHighPassFilter(const uint32_t& audioHandle, float_t& cutOff) {
	//いきなり効果アリにすると違和感あるよね
	//HighPassは最初「0.0f」にした方が良いかも

	cutOff = max(cutOff, 1.0f);
	cutOff = min(cutOff, 0.0f);

	//パラメータの詳細設定
	XAUDIO2_FILTER_PARAMETERS filterParams = {
		.Type = XAUDIO2_FILTER_TYPE::HighPassFilter,
		.Frequency = cutOff,
		.OneOverQ = 1.4142f,
	};

	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);


	//パラメータの設定
	HRESULT hResult = audioInformation_[fileKey].sourceVoice->SetFilterParameters(&filterParams);
	assert(SUCCEEDED(hResult));
}

void Elysia::Audio::SetHighPassFilter(const uint32_t& audioHandle, float_t& cutOff, const float_t& oneOverQ) {

	cutOff = max(cutOff, 1.0f);
	cutOff = min(cutOff, 0.0f);

	//パラメータの詳細設定
	XAUDIO2_FILTER_PARAMETERS filterParams = {
		.Type = XAUDIO2_FILTER_TYPE::HighPassFilter,
		.Frequency = cutOff,
		.OneOverQ = oneOverQ,
	};

	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);


	//パラメータの設定
	HRESULT hResult = audioInformation_[fileKey].sourceVoice->SetFilterParameters(&filterParams);
	assert(SUCCEEDED(hResult));
}

void Elysia::Audio::SetBandPassFilter(const uint32_t& audioHandle, float_t& cutOff) {

	cutOff = max(cutOff, 1.0f);
	cutOff = min(cutOff, 0.0f);

	XAUDIO2_FILTER_PARAMETERS FilterParams;
	FilterParams.Type = XAUDIO2_FILTER_TYPE::BandPassFilter;
	//0.5f
	FilterParams.Frequency = cutOff;
	FilterParams.OneOverQ = 1.0f;

	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	HRESULT hResult = audioInformation_[fileKey].sourceVoice->SetFilterParameters(&FilterParams);
	assert(SUCCEEDED(hResult));
}

void Elysia::Audio::SetBandPassFilter(const uint32_t& audioHandle, float_t& cutOff, const float_t& oneOverQ) {

	cutOff = max(cutOff, 1.0f);
	cutOff = min(cutOff, 0.0f);


	XAUDIO2_FILTER_PARAMETERS FilterParams;
	FilterParams.Type = XAUDIO2_FILTER_TYPE::NotchFilter;;
	//0.5f
	FilterParams.Frequency = cutOff;
	FilterParams.OneOverQ = oneOverQ;

	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	//パラメーターの設定
	HRESULT hResult = audioInformation_[fileKey].sourceVoice->SetFilterParameters(&FilterParams);
	assert(SUCCEEDED(hResult));
}

void Elysia::Audio::SetNotchFilter(const uint32_t& audioHandle, float_t& cutOff) {
	cutOff = max(cutOff, 1.0f);
	cutOff = min(cutOff, 0.0f);


	XAUDIO2_FILTER_PARAMETERS FilterParams;
	FilterParams.Type = XAUDIO2_FILTER_TYPE::BandPassFilter;
	FilterParams.Frequency = cutOff;
	FilterParams.OneOverQ = 1.0f;

	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	//パラメーターの設定
	HRESULT hResult = audioInformation_[fileKey].sourceVoice->SetFilterParameters(&FilterParams);
	assert(SUCCEEDED(hResult));
}

void Elysia::Audio::SetNotchFilter(const uint32_t& audioHandle, float_t& cutOff, const float_t& oneOverQ) {

	cutOff = max(cutOff, 1.0f);
	cutOff = min(cutOff, 0.0f);


	XAUDIO2_FILTER_PARAMETERS FilterParams;
	FilterParams.Type = XAUDIO2_FILTER_TYPE::BandPassFilter;
	FilterParams.Frequency = cutOff;
	FilterParams.OneOverQ = oneOverQ;

	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	//パラメーターの設定
	HRESULT hResult = audioInformation_[fileKey].sourceVoice->SetFilterParameters(&FilterParams);
	assert(SUCCEEDED(hResult));
}


int32_t Elysia::Audio::GetBitPerSample(const uint32_t& audioHandle) {
	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	//bitの数を返す
	return audioInformation_[fileKey].soundData.wfex.wBitsPerSample;
}

float_t Elysia::Audio::GetPlayCurrentTime(const uint32_t& audioHandle) {
	std::string fileKey = GetAudioInformationKey(audioHandle);

	
	if (audioInformation_[fileKey].extension == "wave") {
		//ボイスの状態を取得
		XAUDIO2_VOICE_STATE state;
		audioInformation_[fileKey].sourceVoice->GetState(&state);

		
		//サンプリングレートを取得
		const uint32_t sampleRate = audioInformation_[fileKey].soundData.wfex.nSamplesPerSec;

		// 0は✕
		if (sampleRate == 0.0f) {
			return 0.0f;
		}

		//秒を計算し返す
		return static_cast<float_t>(state.SamplesPlayed) / static_cast<float_t>(sampleRate);

	}
	else if (audioInformation_[fileKey].extension == "mp3") {

		//ボイスの状態を取得
		XAUDIO2_VOICE_STATE state;
		audioInformation_[fileKey].sourceVoice->GetState(&state);

		IMFMediaType* pAudioType = nullptr;

		HRESULT hr = audioInformation_[fileKey].sourceReader->GetCurrentMediaType(
			(DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,
			&pAudioType
		);

		UINT32 sampleRate = 0;
		if (SUCCEEDED(hr)) {
			// 2. MF_MT_AUDIO_SAMPLES_PER_SECOND 属性でサンプリングレートを取得
			hr = pAudioType->GetUINT32(
				MF_MT_AUDIO_SAMPLES_PER_SECOND,
				&sampleRate
			);
			pAudioType->Release();
		}
		// エラーまたは未設定の場合は 0 を返す
		if (sampleRate == 0) {
			return 0.0f;
		};
		return static_cast<float_t>(state.SamplesPlayed) / static_cast<float_t>(sampleRate);
	
	}

	return 0.0f;
}

void Elysia::Audio::SendChannels(const uint32_t& audioHandle, const uint32_t& channelNumber) {
	XAUDIO2_SEND_DESCRIPTOR send = { 0, Audio::GetInstance()->submixVoice_[0] };
	XAUDIO2_VOICE_SENDS sendlist = { channelNumber, &send };

	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	HRESULT hResult = audioInformation_[fileKey].sourceVoice->SetOutputVoices(&sendlist);
	assert(SUCCEEDED(hResult));
}

//エフェクトの効果を無効にする
void Elysia::Audio::OffEffect(const uint32_t& audioHandle) {

	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	HRESULT hResult = audioInformation_[fileKey].sourceVoice->DisableEffect(0);
	assert(SUCCEEDED(hResult));
}

//エフェクトの効果を有効にする
void Elysia::Audio::OnEffect(const uint32_t& audioHandle) {

	//ファイルキーの取得
	std::string fileKey = GetAudioInformationKey(audioHandle);

	HRESULT hResult = audioInformation_[fileKey].sourceVoice->EnableEffect(0);
	assert(SUCCEEDED(hResult));
}

#pragma endregion



//解放
void Elysia::Audio::Finalize() {

	//あるもの全部消す
	for (std::map<std::string, AudioInformation>::iterator it = audioInformation_.begin(); it != audioInformation_.end(); ++it) {
		if ((*it).second.sourceVoice != nullptr) {
			(*it).second.sourceVoice->DestroyVoice();
			delete[](*it).second.soundData.pBuffer;
		}
	}

	//残りを消す
	audioInformation_.clear();

	//XAudio2の解放
	xAudio2_.Reset();
}

