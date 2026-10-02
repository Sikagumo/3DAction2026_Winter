// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"

// IN
#define PS_INPUT VertexToPixelLit

// PS
#include "../Common/Pixel/PixelShader3DHeader.hlsli"

// 定数バッファ：スロット4番目(b4と書く)
cbuffer cbParam : register(b4)
{
    // 拡散光の色
    float4 g_diff_color;
    
    // 環境光の色
    float4 g_ambient_color;
    
    // 鏡面反射の色
    float4 g_specular_color;
    
    
    // 光の方向
    float3 g_light_direction;
    
    // 鏡面反射の強さ(0.0～1.0)
    float g_specular_pow;
    
    
    // 描画地点(カメラ)からの位置
    float3 g_camera_position;
    
    float dummy;
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
    
    // 拡散光の強さ
    float difDot = dot(norm, -g_light_direction);
   
    // 頂点からのカメラ方向
    float3 toEye = normalize(g_camera_position - PSInput.worldPos);
    
    
    // 反射ベクトル
    float3 reflectDir = normalize(g_light_direction + (PSInput.normal * 2.0f));
    
    // 反射光の強さ(反対側が負の値のため絶対値で返す)
    float refDot = abs(dot(reflectDir, toEye));
    
    // 反射光を絞る
    float refDotEx = pow(refDot, g_specular_pow);

    // 拡散光
    float3 diffuse = color.rgb * (difDot * g_diff_color.rgb);
    
    // 色の合成
    float3 rgb = diffuse + (refDotEx * g_specular_color.rgb) + g_ambient_color.rgb;
    
    
    // フォグカラー乗算
    rgb *= PSInput.fogFactor;
    
    // ポイントライトカラー加算
    float3 lightColor = float3(0.0f, 0.0f, 0.0f);
    rgb += lightColor * PSInput.lightColor;
    
	// 関数の戻り値がラスタライザに渡される
    return float4(rgb, color.a);

}
