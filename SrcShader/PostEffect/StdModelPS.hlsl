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
   
    
    // 鏡面反射の色
    float3 g_specular_color;
    
    // 鏡面反射の強さ(0.0～1.0)
    float g_speculer_pow;
   
    
    // 描画地点(カメラ)からの位置
    float3 g_view_position;
    float dummy2;
}

float4 main(PS_INPUT PSInput) : SV_TARGET
{   
    float3 diffuseCol = PSInput.diffuse.rgb;
    
	// テクスチャーの色を取得
	float4 color = diffuseMapTexture.Sample(diffuseMapSampler, PSInput.uv);
    
	if (color.a < 0.01f)
    {
        discard;
    }
    
    /* ランバート反射 */
    float lightDot = dot(PSInput.normal, -g_light_direction);
    float3 rgb = (color.rgb * lightDot);
    rgb += g_ambient_color.rgb;
    
    // フォグカラー乗算
    rgb *= PSInput.fogFactor;
    
    // ポイントライトカラー加算
    rgb += PSInput.lightColor;
    
    return float4(rgb, color.a);
   
}
