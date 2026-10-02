// VS/PS共通	
#include "../Common/VertexToPixelHeader.hlsli"

// IN
#include "../Common/Vertex/VertexInputType.hlsli"
#define VERTEX_INPUT DX_MV1_VERTEX_TYPE_NMAP_1FRAME

// OUT
#define VS_OUTPUT VertexToPixelLit
#include "../Common/Vertex/VertexShader3DHeader.hlsli"

// 定数バッファ：スロット7番目
cbuffer cbParam : register(b7)
{
    float3 g_camera_pos;
    float dummy1;
    
    float g_fog_start;
    float g_fog_end;
    float g_light_num;
    float dummy;
	
    float4 g_light_color;
	
    float4 g_light_pos[30];
}

VS_OUTPUT main(VS_INPUT VSInput)
{

	VS_OUTPUT ret;
	
	// 頂点座標変換 +++++++++++++++++++++++++++++++++++++( 開始 )
	float4 lLocalPosition;
	float4 lWorldPosition;
	float4 lViewPosition;

	// float3 → float4
	lLocalPosition.xyz = VSInput.pos;
	lLocalPosition.w = 1.0f;

	// ローカル座標をワールド座標に変換(剛体)
	lWorldPosition.w = 1.0f;
	lWorldPosition.xyz = mul(float4(VSInput.pos, 1.0f), g_base.localWorldMatrix);
	//lWorldPosition.xyz = mul(lLocalPosition, g_base.localWorldMatrix);
    ret.worldPos.xyz = lWorldPosition.xyz;

	// ワールド座標をビュー座標に変換
	lViewPosition.w = 1.0f;
	lViewPosition.xyz = mul(lWorldPosition, g_base.viewMatrix);
	ret.vwPos.xyz = lViewPosition.xyz;

	// ビュー座標を射影座標に変換
	ret.svPos = mul(lViewPosition, g_base.projectionMatrix);

	// 頂点座標変換 +++++++++++++++++++++++++++++++++++++( 終了 )

	
	// その他、ピクセルシェーダへ引継&初期化 ++++++++++++( 開始 )
	// UV座標
    ret.uv.x = VSInput.uv0.x;
    ret.uv.y = VSInput.uv0.y;
	
	// 法線
	ret.normal = normalize(
					mul(VSInput.norm, (float3x3) g_base.localWorldMatrix));
	
	// ディフューズカラー
	ret.diffuse = VSInput.diffuse;
	
	
	// ライト方向(ローカル)
	ret.lightDir = float3(0.0f, 0.0f, 0.0f);
	// ライトから見た座標
	ret.lightAtPos = float3(0.0f, 0.0f, 0.0f);
	

	// カメラとの距離
    float distance = length(lWorldPosition.xyz - g_camera_pos);
    
    // 距離の倍率(0.0～1.0)
    ret.fogFactor = saturate(((g_fog_end - distance) / (g_fog_end - g_fog_start)));
	
    float3 totalLight = float3(0.0f, 0.0f, 0.0f);
	
    for (int i = 0; i < (int) g_light_num; i++)
    {
        float3 lightPos = g_light_pos[i].xyz;
		
        float dis = length(lWorldPosition.xyz - g_light_pos[i].xyz);
		
        float radius = g_light_pos[i].w;
		
        float power = saturate((radius - dis) / radius);

        totalLight += (g_light_color.rgb * power);
    }
	
    ret.lightColor = totalLight;
	

	// その他、ピクセルシェーダへ引継&初期化 ++++++++++++( 終了 )
	

	// 出力パラメータを返す
	return ret;

}