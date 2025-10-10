#pragma once
/**
 * @file AudioStruct.h
 * @brief オーディオに関する構造体が入っている
 * @author 茂木翼
 */

#include <cstdint>
#include <xaudio2.h>
#include <string>
#include <mfreadwrite.h>


//チャンク...データの塊みたいなもの

/// <summary>
/// チャンクヘッダ
/// </summary>
struct ChunkHeader {
	//チャンク毎のID
	char id[4];
	//チャンクサイズ
	int32_t size;
};

/// <summary>
/// FMTチャンク
/// </summary>
struct FormatChunk {
	//fmt
	ChunkHeader chunk;
	//波形フォーマット
	WAVEFORMATEX fmt;

};

/// <summary>
/// RIFFヘッダー
/// </summary>
struct RiffHeader {
	//RIFF
	ChunkHeader chunk;
	//WAVE
	char type[4];
};

/// <summary>
/// 音声データ
/// </summary>
struct SoundData {
	//波形フォーマット
	WAVEFORMATEX wfex;

	//バッファの先頭アドレス
	BYTE* pBuffer;

	//バッファのサイズ
	int32_t bufferSize;
};

/// <summary>
/// オーディオ情報
/// </summary>
struct AudioInformation {

	//読み込んだオーディオの名前
	std::string fileName;

	//拡張子
	std::string extension;

	//サウンドデータ
	SoundData soundData = {};

	//サウンドボイス
	IXAudio2SourceVoice* sourceVoice;

	//ソースリーダー
	IMFSourceReader* sourceReader;

	//メディアデータ
	std::vector<BYTE> mediaData;

	//ハンドル
	uint32_t handle;

	//Pan用
	float_t left;
	float_t right;

};