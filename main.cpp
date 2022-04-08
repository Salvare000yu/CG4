#include <vector>
#include <string>
#include <fstream>

#include <vector>

#include <xaudio2.h>
#pragma comment(lib,"xaudio2.lib")

#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"

#include "Object3d.h"
#include "Model.h"

#include "SpriteCommon.h"
#include "Sprite.h"

#include "DebugText.h"

using namespace Microsoft::WRL;

//ポインタ系ここ
Input* input = nullptr;
WinApp* winApp = nullptr;
DirectXCommon* dxCommon = nullptr;

LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    // メッセージで分岐
    switch (msg) {
        case WM_DESTROY: // ウィンドウが破棄された
            PostQuitMessage(0); // OSに対して、アプリの終了を伝える
            return 0;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam); // 標準の処理を行う
}



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

class XAudio2VoiceCallback : public IXAudio2VoiceCallback
{
public:
    // ボイス処理パスの開始時
    STDMETHOD_(void, OnVoiceProcessingPassStart) (THIS_ UINT32 BytesRequired) {};
    // ボイス処理パスの終了時
    STDMETHOD_(void, OnVoiceProcessingPassEnd) (THIS) {};
    // バッファストリームの再生が終了した時
    STDMETHOD_(void, OnStreamEnd) (THIS) {};
    // バッファの使用開始時
    STDMETHOD_(void, OnBufferStart) (THIS_ void* pBufferContext) {};
    // バッファの末尾に達した時
    STDMETHOD_(void, OnBufferEnd) (THIS_ void* pBufferContext) {};
    // 再生がループ位置に達した時
    STDMETHOD_(void, OnLoopEnd) (THIS_ void* pBufferContext) {};
    // ボイスの実行エラー時
    STDMETHOD_(void, OnVoiceError) (THIS_ void* pBufferContext, HRESULT Error) {};
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

// 音声読み込み
SoundData SoundLoadWave(const char* filename)
{
    // ファイル入力ストリームのインスタンス
    std::ifstream file;
    // .wavファイルをバイナリモードで開く
    file.open(filename, std::ios_base::binary);
    // ファイルオープン失敗を検出する
    assert(file.is_open());

    // RIFFヘッダーの読み込み
    RiffHeader riff;
    file.read((char*)&riff, sizeof(riff));
    // ファイルがRIFFかチェック
    if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
        assert(0);
    }
    // タイプがWAVEかチェック
    if (strncmp(riff.type, "WAVE", 4) != 0) {
        assert(0);
    }

    // Formatチャンクの読み込み
    FormatChunk format = {};
    // チャンクヘッダーの確認
    file.read((char*)&format, sizeof(ChunkHeader));
    if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
        assert(0);
    }
    // チャンク本体の読み込み
    assert(format.chunk.size <= sizeof(format.fmt));
    file.read((char*)&format.fmt, format.chunk.size);

    // Dataチャンクの読み込み
    ChunkHeader data;
    file.read((char*)&data, sizeof(data));
    // JUNKチャンクを検出した場合
    if (strncmp(data.id, "JUNK ", 4) == 0) {
        // 読み取り位置をJUNKチャンクの終わりまで進める
        file.seekg(data.size, std::ios_base::cur);
        // 再読み込み
        file.read((char*)&data, sizeof(data));
    }

    if (strncmp(data.id, "data ", 4) != 0) {
        assert(0);
    }

    // Dataチャンクのデータ部（波形データ）の読み込み
    char* pBuffer = new char[data.size];
    file.read(pBuffer, data.size);

    // Waveファイルを閉じる
    file.close();

    // returnする為の音声データ
    SoundData soundData = {};

    soundData.wfex = format.fmt;
    soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
    soundData.bufferSize = data.size;

    return soundData;
}

// 音声データ解放
void SoundUnload(SoundData* soundData)
{
    // バッファのメモリを解放
    delete[] soundData->pBuffer;

    soundData->pBuffer = 0;
    soundData->bufferSize = 0;
    soundData->wfex = {};
}

// 音声再生
void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData) {

    HRESULT result;

    // 波形フォーマットを元にSourceVoiceの生成
    IXAudio2SourceVoice* pSourceVoice = nullptr;
    result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
    assert(SUCCEEDED(result));

    // 再生する波形データの設定
    XAUDIO2_BUFFER buf{};
    buf.pAudioData = soundData.pBuffer;
    buf.AudioBytes = soundData.bufferSize;
    buf.Flags = XAUDIO2_END_OF_STREAM;

    // 波形データの再生
    result = pSourceVoice->SubmitSourceBuffer(&buf);
    result = pSourceVoice->Start();
}


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

    //windowsAPI初期化
    winApp = new WinApp();
    winApp->Initialize();

    MSG msg{};  // メッセージ
#pragma endregion WindowsAPI初期化

#pragma region DirectX初期化処理
    // DirectX初期化処理　ここから
    HRESULT result;
    //DirectXの初期化
    dxCommon = new DirectXCommon();
    dxCommon->Initialize(winApp);

    // スプライト共通部分初期化
    SpriteCommon* spriteCommon=new SpriteCommon();
    spriteCommon->Initialize(dxCommon->GetDevice(),dxCommon->GetCmdList(),winApp->window_width,winApp->window_height);

    //const int SPRITES_NUM = 1;
    //Sprite sprites[SPRITES_NUM];

    //入力の初期化
    input = new Input();
    input->Initialize(winApp);

    ComPtr<IXAudio2> xAudio2;
    IXAudio2MasteringVoice* masterVoice;
    XAudio2VoiceCallback voiceCallback;

    // XAudioエンジンのインスタンスを生成
    result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    assert(SUCCEEDED(result));

    // マスターボイスを生成
    result = xAudio2->CreateMasteringVoice(&masterVoice);
    assert(SUCCEEDED(result));

    // 音声読み込み
    SoundData soundData1 = SoundLoadWave("Resources/Alarm01.wav");

    // 音声再生
    //SoundPlayWave(xAudio2.Get(), soundData1);

    //3dオブジェクト静的初期化
    Object3d::StaticInitialize(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_height);

    // DirectX初期化処理　ここまで
#pragma endregion DirectX初期化処理

#pragma region 描画初期化処理

    //---objからモデルデータ読み込み---
    Model* model_1 = Model::LoadFromOBJ("ground"); 
    Model* model_2 = Model::LoadFromOBJ("triangle_mat");
    //Model* model_3 = Model::LoadFromOBJ("chr_sword");
    //---3dオブジェクト生成---
    Object3d* object3d_1 = Object3d::Create();
    Object3d* object3d_2 = Object3d::Create();
    Object3d* object3d_3 = Object3d::Create();
    //---3dオブジェクトに3dモデルを紐づける---
    object3d_1->SetModel(model_1);
    object3d_2->SetModel(model_2);
    object3d_3->SetModel(model_2);

    object3d_2->SetScale({ 20.0f, 20.0f, 20.0f });
    object3d_3->SetScale({ 30.0f, 30.0f, 30.0f });

    object3d_2->SetPosition({ 5,-1,5 });
    object3d_3->SetPosition({ -5,-1,5 });

    // 3Dオブジェクトの数
    //const int OBJECT_NUM = 2;

    //Object3d object3ds[OBJECT_NUM];
    
    // スプライト共通テクスチャ読み込み
    spriteCommon->LoadTexture( 0, L"Resources/texture.png");
    spriteCommon->LoadTexture( 1, L"Resources/house.png");

    std::vector<Sprite*> sprites;

    // スプライトの生成
    for (int i = 0; i < 1; i++)
    {
        int texNumber = 1;
        Sprite* sprite = Sprite::Create(spriteCommon, texNumber, { 0,0 }, false, false);

        // スプライトの座標変更
        sprite->SetPosition({ (float)(80),(float)(20),0 });
        //sprite->SetRotation((float)(rand() % 360));
        sprite->SetSize({ (float)(200), (float)(200) });

        sprite->TransferVertexBuffer();

        sprites.push_back(sprite);
    }

    // デバッグテキスト
    DebugText* debugText=nullptr;
    debugText = new DebugText();

    // デバッグテキスト用のテクスチャ番号を指定
    const int debugTextTexNumber = 2;
    // デバッグテキスト用のテクスチャ読み込み
    spriteCommon->LoadTexture(debugTextTexNumber, L"Resources/debugfont.png");
    // デバッグテキスト初期化
    debugText->Initialize(spriteCommon,debugTextTexNumber);

#pragma endregion 描画初期化処理

    int counter = 0; // アニメーションの経過時間カウンター

    while (true)  // ゲームループ
    {
#pragma region ウィンドウメッセージ処理
        //// メッセージがある？
        //if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        //    TranslateMessage(&msg); // キー入力メッセージの処理
        //    DispatchMessage(&msg); // プロシージャにメッセージを送る
        //}

        //// ✖ボタンで終了メッセージが来たらゲームループを抜ける
        //if (msg.message == WM_QUIT) {
        //    break;
        //}
            //windowsのメッセージ処理
        if (winApp->ProcessMessage()) {
            //ゲームループ抜ける
            break;
        }
#pragma endregion ウィンドウメッセージ処理

#pragma region DirectX毎フレーム処理
        // DirectX毎フレーム処理　ここから

        input->Update();

        if (input->PushKey(DIK_0)) // 数字の0キーが押されていたら
        {
            OutputDebugStringA("Hit 0\n");  // 出力ウィンドウに「Hit 0」と表示
        }        

        float clearColor[] = { 0.1f,0.25f, 0.5f,0.0f }; // 青っぽい色

        if (input->PushKey(DIK_SPACE))     // スペースキーが押されていたら
        {
            // 画面クリアカラーの数値を書き換える
            clearColor[1] = 1.0f;
        }

        // 座標操作
        if (input->PushKey(DIK_UP) || input->PushKey(DIK_DOWN) || input->PushKey(DIK_RIGHT) || input->PushKey(DIK_LEFT))
        {

        }


        if (input->PushKey(DIK_D) || input->PushKey(DIK_A))
        {

        }

        //3dobj
        object3d_1->Update();
        object3d_2->Update();
        object3d_3->Update();

        //スプライト更新
        for (auto& sprite : sprites) {
            sprite->Update();
        }

        // DirectX毎フレーム処理　ここまで
#pragma endregion DirectX毎フレーム処理

#pragma region グラフィックスコマンド
 
        //描画前処理
        dxCommon->PreDraw();

        Object3d::PreDraw(dxCommon->GetCmdList());

        object3d_1->Draw();
        object3d_2->Draw();
        object3d_3->Draw();

        Object3d::PostDraw();

        // ４．描画コマンドここから

        //for (int i = 0; i < _countof(object3ds); i++)
        //{
        //    DrawObject3d(&object3ds[i], dxCommon->GetCmdList(), basicDescHeap.Get(), vbView, ibView,
        //        CD3DX12_GPU_DESCRIPTOR_HANDLE(basicDescHeap->GetGPUDescriptorHandleForHeapStart(), constantBufferNum, dxCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)),
        //        indices, _countof(indices));
        //}

        //// スプライト共通コマンド
        spriteCommon->PreDraw();
        //SpriteCommonBeginDraw(spriteCommon, dxCommon->GetCmdList());
        //// スプライト描画
        for (auto& sprite : sprites) {
            sprite->Draw();
        }

        debugText->Print("Hello,DirectX!!", 200, 100);
        debugText->Print("nihon kougakuin!", 200,200, 2.0f);

        // デバッグテキスト描画
        debugText->DrawAll();

        // ４．描画コマンドここまで

        //描画後処理
        dxCommon->PostDraw();
    }

    //デバッグテキスト解放
    delete debugText;

    //スプライト解放
    for (auto& sprite : sprites) {
        delete sprite;
    }

    sprites.clear();
    //sprite共通解放
    delete spriteCommon;
    //3dオブジェクト解放
    delete object3d_1;
    delete object3d_2;
    delete object3d_3;
    delete model_1;
    delete model_2;

    //DirectX解放
    delete dxCommon;
    //入力解放
    delete input;
    // XAudio2解放
    xAudio2.Reset();
    // 音声データ解放
    SoundUnload(&soundData1);

    //windowsAPIの終了処理
    winApp->Finalize();

    //windowsAPI解放
    delete winApp;

	return 0;
}