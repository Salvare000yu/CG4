#include <windows.h>
#include <wrl.h>
#define DIRECTINPUT_VERSION 0x0800//directinputバージョン指定
#include <dinput.h>
#include "WinApp.h"

#pragma once
class Input
{
public:

	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public://メンバ関数

	//初期化
	void Initialize(WinApp* WinApp);
	//更新
	void Update();

/// <summary>
/// キーの押下チェック
/// </summary>
/// 	キー番号
/// 	押されているか
	bool PushKey(BYTE keyNumber);

	/// <summary>
/// キーのトリガー
/// </summary>
/// 	キー番号
/// 	トリガーか
	bool TriggerKey(BYTE keyNumber);

private://メンバ変数
	//キーボードのデバイス
	ComPtr<IDirectInputDevice8> devkeyboard;

	//全キーの配置
	BYTE key[256] = {};

	//前回の全キーの配置
	BYTE keyPre[256] = {};

	//directinputのインスタンス
	ComPtr<IDirectInput8>dinput;

	//WindowsAPI
	WinApp* winApp = nullptr;
};

