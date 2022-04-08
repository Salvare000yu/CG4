#include "Input.h"
//#include <wrl.h>

//#define DIRECTINPUT_VERSION 0x0800//directinputバージョン指定
//#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//using namespace Microsoft::WRL;

void Input::Initialize(WinApp* winApp)
{

    //借りてきたWinAppのインスタンスを記録
    this->winApp = winApp;

    HRESULT result;
    //BYTE key[256] = {};

    //ComPtr<IDirectInput8> dinput = nullptr;
    // 
    //directinputインスタンス生成
    result = DirectInput8Create(
       winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);//hInstanceの前にw.
   // ComPtr<IDirectInputDevice8> devkeyboard = nullptr;
    //キーボードのデバイス生成
    result = dinput->CreateDevice(GUID_SysKeyboard, &devkeyboard, NULL);
    //入力データ形式セット
    result = devkeyboard->SetDataFormat(&c_dfDIKeyboard); // 標準形式
    //排他制御レベルセット
    result = devkeyboard->SetCooperativeLevel(
        winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);

}

void Input::Update()
{
    HRESULT result;

    //前回のキー入力保存
    memcpy(keyPre, key, sizeof(key));

    //キーボード情報取得開始
    result = devkeyboard->Acquire();
    //全キー入力情報取得
    result = devkeyboard->GetDeviceState(sizeof(key), key);
}

bool Input::PushKey(BYTE keyNumber)
{
    //指定キーを押していればTRUE
    if (key[keyNumber]) {
        return true;
    }
    //そうでなければ返す
    return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{

    //トリガー処理
    // ---前回0,今回0でなければトリガー---
    if (!keyPre[keyNumber] && key[keyNumber]) {
        return true;
    }
    return false;
}
