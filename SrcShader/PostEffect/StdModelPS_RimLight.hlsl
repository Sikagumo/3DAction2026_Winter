// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"

// IN
#define PS_INPUT VertexToPixelLit

// PS
#include "../Common/Pixel/PixelShader3DHeader.hlsli"

// 定数バッファ：スロット4番目(b4と書く)
cbuffer cbParam : register(b4)
{
     // 光の方向
    float3 g_light_direction;
    float dummy1;
    
    // 環境光の色
    float4 g_ambient_color;
    
    // 拡散光の色
    float4 g_diff_color;
    
    
    // リムの色
    float3 g_rim_color;
    
    // リムの強さ
    float g_rim_pow;
    
    
    // 描画地点(カメラ)からの位置
    float3 g_camera_position;
    float dummy2;
}

float4 main(PS_INPUT PSInput) : SV_TARGET
{
	// テクスチャーの色を取得
    float4 color = diffuseMapTexture.Sample(diffuseMapSampler, PSInput.uv);
    
    if (color.a < 0.01f)
    {
        discard;
    }
    
    // 法線
    float3 norm = PSInput.normal;
   
    
    // 描画位置
    float3 toEye = normalize(PSInput.worldPos - g_camera_position);
    
    // 拡散光の強さ
    //float difDot = dot(norm, -toEye);
    float difDot = dot(norm, -g_light_direction);
    
    // リムの強さ
    float rimDot = dot(norm, toEye);
    
    // (視線方向と一致か逆が1.0)
    rimDot = abs(rimDot);
    
    // (視線方向と直交が1.0)
    rimDot = (1.0f - rimDot);
    
    // リムの強さ
    rimDot = pow(rimDot, g_rim_pow);

    
    // 拡散光
    float3 diffuse = color.rgb * (difDot * g_diff_color).rgb;
    
    // 色の合成
    float3 rgb = diffuse + (rimDot * g_rim_color) + g_ambient_color.rgb;
    
    // フォグカラー乗算
    rgb *= PSInput.fogFactor;
    
    // ポイントライトカラー加算
    float3 lightColor = float3(0.0f, 0.0f, 0.0f);
    rgb += lightColor * PSInput.lightColor;
    
    return float4(rgb, color.a);
}
