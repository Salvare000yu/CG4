#pragma once

#include "PipelineSet.h"

#include <DirectXTex.h>

/// <summary>
/// スプライト共通部分
/// </summary>
class SpriteCommon
{
public:
    // テクスチャの最大枚数
    static const int spriteSRVCount = 512;

    /// <summary>
    /// 初期化：デバイス　コマンドリスト　ウィンドウ横幅縦幅
    /// </summary>
    void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,int window_width, int window_height);

    /// <summary>
    /// 描画前処理
    /// </summary>
    void PreDraw();

    /// <summary>
    /// テクスチャ読み込み
    /// </summary>
    /// <param name="texnumber"></param>
    /// <param name="filename"></param>
    void LoadTexture(UINT texnumber, const wchar_t* filename);

    /// <summary>
    /// ルートデスクリプタテーブルの設定
    /// </summary>
    /// <param name="rootparameterIndex">ルートパラメータ番号</param>
    /// <param name="texNumber">テクスチャ番号</param>
    void SetGraphicsRootDescriptorTable(UINT rootparameterIndex,UINT texNumber);

    const DirectX::XMMATRIX& GetMatProjection() { return matProjection_; }

    /// <summary>
    /// テクスチャ取得
    /// </summary>
    /// <param name="texnumber"></param>
    /// <returns></returns>
    ID3D12Resource* GetTexBuff(int texNumber);

    //
    ID3D12Device* GetDevice() { return dev; }

    ID3D12GraphicsCommandList* GetCommandList() { return commandList_; }


private:
    // パイプラインセット
    PipelineSet pipelineSet;
    // 射影行列
    DirectX::XMMATRIX matProjection_{};
    // テクスチャ用デスクリプタヒープの生成
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap;
    // テクスチャリソース（テクスチャバッファ）の配列
    Microsoft::WRL::ComPtr<ID3D12Resource> texBuff_[spriteSRVCount];

    //---解放しないからcomptr使わない
    //借りるデバイス
    ID3D12Device* dev=nullptr;
    //借りるコマンドリスト
    ID3D12GraphicsCommandList* commandList_=nullptr;
    //---

private:
    //スプライト用パイプライン生成
    void CreateGraphicsPipeline();


};

