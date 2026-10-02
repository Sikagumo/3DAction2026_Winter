// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"

// IN
#define PS_INPUT VertexToPixelLit

// PS
#include "../Common/Pixel/PixelShader3DHeader.hlsli"

// 定数バッファ：スロット4番目(b4と書く)
cbuffer cbParam : register(b4)
{
    float3 g_lightDir;
    float dummy;
    
    float4 g_lightColor;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 color = diffuseMapTexture.Sample(diffuseMapSampler, input.uv);

    // DirectionalLight
    float3 norm = normalize(input.normal);

    float3 length = normalize(-g_lightDir);

    float diffuse = saturate(dot(norm, length));

    color.rgb *= diffuse * g_lightColor.rgb;


    // ポイントライト
    color.rgb += input.lightColor;

    // フォグ加算
    color.rgb *= input.fogFactor;

    return color;
}