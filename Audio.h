#pragma once

#include <xaudio2.h>
#include <cstdint>
#include <wrl.h>
#include <map>
#include <string>

/// <summary>
/// オーディオ
/// </summary>

class Audio
{

public:
    // チャンクヘッダ
    struct ChunkHeader
    {
        char id[4]; // チャンク毎のID
        int32_t size;  // チャンクサイズ
    };

    // RIFFヘッダチャンク
    struct RiffHeader
    {
        ChunkHeader chunk;   // "RIFF"
        char type[4]; // "WAVE"
    };

    // FMTチャンク
    struct FormatChunk
    {
        ChunkHeader chunk; // "fmt "
        WAVEFORMATEX fmt; // 波形フォーマット
    };

    // 音声データ
    struct SoundData
    {
        // 波形フォーマット
        WAVEFORMATEX wfex;
        // バッファの先頭アドレス
        BYTE* pBuffer;
        // バッファのサイズ
        unsigned int bufferSize;
    };

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize(const std::string& directoryPath="Resources/");

    /// <summary>
    /// 解放処理　終了
    /// </summary>
    void Finalize();

    /// <summary>
    /// 音声読み込み
    /// </summary>
    void LoadWave(const std::string& filename);

    /// <summary>
    /// サウンドデータ解放
    /// </summary>
    /// <param name="filename"></param>
    void Unload(SoundData* soundData);

    /// <summary>
    /// 音声再生
    /// </summary>
    /// <param name="filename">WAVファイル名</param>
    void PlayWave(const std::string& filename);

private:
    Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
    //サウンドデータ連想配列
    std::map<std::string, SoundData> soundDatas_;

    //サウンド格納ディレクトリ
    std::string directoryPath_;
};

