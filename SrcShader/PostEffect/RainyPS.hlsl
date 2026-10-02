//==============================================================
// rain_ps.hlsl
// 雨が降っているように見えるピクセルシェーダー
// - 背景テクスチャ(BgTex)の上に、疑似乱数によるストリーク(雨筋)を重ねる
// - 手前層/奥層の2レイヤーを重ねて奥行き感を出す
//==============================================================
#include "../Common/Pixel/PixelShader2DHeader.hlsli"

// ----- 定数バッファ -----
cbuffer RainCB : register(b0)
{
    float2 Resolution; // 画面解像度 (px)
    float Time; // 経過時間 (秒)
    float Intensity; // 雨の強さ 0.0～1.0
    
    float speed; // 雨の速さ
    float3 dummy;
};

// 0～1の疑似乱数 (整数座標から生成)
float Hash21(float2 p)
{
    p = frac(p * float2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return frac(p.x * p.y);
}

// 1レイヤー分の雨筋を計算する
// columns : 縦の列数(密度)
// speed   : 落下速度
// streak  : 1本の筋の長さ(0～1、大きいほど長い)
// widthPx : 筋の太さ(ピクセル)
float RainLayer(float2 uv, float columns, float speed, float streakLen, float widthPx)
{
    // 列インデックス
    float colF = (uv.x * columns);
    float colId = floor(colF);
    float colFrac = frac(colF);

    // 列ごとにランダムな横位置オフセット・速度・位相をつける
    float rnd = Hash21(float2(colId, 0.0));
    float xOffset = (rnd - 0.5) * (1.0 / columns) * 0.6;
    float colSpeed = speed * (0.6 + rnd * 0.8);
    float phase = Hash21(float2(colId, 1.0));

    // 縦方向にループするスクロール座標 (0～1)
    float y = frac(uv.y + phase - Time * colSpeed);

    // 筋の中心からの距離(y方向、上端が濃く下に伸びるイメージ)
    float streakMask = smoothstep(streakLen, streakLen * 0.5, y);

    // 横方向の太さ判定
    float distX = abs(colFrac - 0.5 + xOffset * columns);
    float pxWidth = widthPx / Resolution.x * columns;
    float lineMask = smoothstep(pxWidth, 0.0, distX);

    // 列ごとに出現/非表示をランダムに間引く(まばらな雨粒感)
    float visible = step(0.35, Hash21(float2(colId, floor(Time * 0.5 + phase * 10.0))));

    return (lineMask * streakMask * visible);
}

float4 main(PS_INPUT input) : SV_TARGET
{
    
    float2 uv = input.uv;

    // 背景をサンプリング
    //float4 bg = BgTex.Sample(BgSmp, uv);

    // 奥のレイヤー(細く速く、密度高め、暗め)
    float back = RainLayer(uv, 140.0, 1.6, 0.55, 1.0) * 0.5;

    // 手前のレイヤー(太く速く、密度低め、明るめ)
    float front = RainLayer(uv * float2(1.0, 1.0) + float2(0.37, 0.0), 60.0, 2.4, 0.35, 2.0);
    
    float rain = saturate(back + front) * Intensity;

    // 雨筋の色(わずかに青白い)
    float3 rainColor = float3(0.75, 0.82, 0.9);

    // 画面全体をわずかに暗く・青みがからせて雨天感を出す
    //float3 tinted = lerp(bg.rgb, bg.rgb * float3(0.75, 0.8, 0.9), Intensity * 0.5);

    // 雨筋を加算合成
    float3 finalColor = (rainColor * rain);

    return float4(saturate(finalColor), input.diffuse.a);
}
