#include <vector>
#include <string>

#include <vector>

#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"

#include "Object3d.h"
#include "Model.h"

#include "SpriteCommon.h"
#include "Sprite.h"

#include "DebugText.h"

#include "Audio.h"

using namespace Microsoft::WRL;

//ポインタ系ここ
Input* input = nullptr;
WinApp* winApp = nullptr;
DirectXCommon* dxCommon = nullptr;
Audio* audio = nullptr;
DebugText* debugText = nullptr;

LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    // メッセージで分岐
    switch (msg) {
        case WM_DESTROY: // ウィンドウが破棄された
            PostQuitMessage(0); // OSに対して、アプリの終了を伝える
            return 0;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam); // 標準の処理を行う
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

      // デバッグテキスト
    debugText = new DebugText();

    // デバッグテキスト用のテクスチャ番号を指定
    const int debugTextTexNumber = 2;
    // デバッグテキスト用のテクスチャ読み込み
    spriteCommon->LoadTexture(debugTextTexNumber, L"Resources/debugfont.png");
    // デバッグテキスト初期化
    debugText->Initialize(spriteCommon, debugTextTexNumber);

    //入力の初期化
    input = new Input();
    input->Initialize(winApp);

    //オーディオの初期化
    audio=new Audio();
    audio->Initialize();

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

    // 音声読み込み
    audio->LoadWave("Alarm01.wav");

    // 音声再生
    audio->PlayWave("Alarm01.wav");

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
    debugText->Finalize();
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
    //オーディオ解放
    audio->Finalize();
    delete audio;

    //DirectX解放
    delete dxCommon;
    //入力解放
    delete input;

    //windowsAPIの終了処理
    winApp->Finalize();

    //windowsAPI解放
    delete winApp;

	return 0;
}