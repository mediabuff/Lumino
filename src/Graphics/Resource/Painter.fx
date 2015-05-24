
float4x4	g_worldMatrix;
float4x4	g_viewProjMatrix;

texture		g_texture;
sampler2D	g_texSampler = sampler_state
{
	texture = <g_texture>;
	MINFILTER = NONE;
	MAGFILTER = NONE;
};

struct VS_OUTPUT
{
	float4 Pos			: POSITION;
	float4 Color		: COLOR0;
	float4 UVOffset		: TEXCOORD0;
	float2 UVTileUnit	: TEXCOORD1;
};

//-------------------------------------------------------------------------
// vertex shader
//-------------------------------------------------------------------------
VS_OUTPUT vsBasic(
	float3 inPos		: POSITION,
	float4 inColor		: COLOR0,
	float4 inUVOffset	: TEXCOORD0,
	float2 inUVTileUnit	: TEXCOORD1)
{
	VS_OUTPUT o;
	o.Pos			= mul(float4(inPos, 1.0f), g_worldMatrix);
	o.Pos			= mul(o.Pos, g_viewProjMatrix);
	o.Color			= inColor;
	o.UVOffset		= inUVOffset;
	o.UVTileUnit	= inUVTileUnit;
	return o;
}

//-------------------------------------------------------------------------
// pixel shader
//-------------------------------------------------------------------------
float4 psBasic(
	float4 inColor			: COLOR0,
	float4 inUVOffset		: TEXCOORD0,
	float2 inUVTileUnit		: TEXCOORD1) : COLOR0
{
	// 転送元左上 UV
	float2 uvUpperLeft = inUVOffset.xy;//float2(inUVOffset.x, inUVOffset.y);
	
	// 転送元矩形の幅 UV
	float2 uvWidth = inUVOffset.zw ;//uvUpperLeft - float2(inUVOffset.zw / inUVTileUnit);
	
	// 1つの四角形の中のどこにいるのか (0.0〜1.0)
	float2 uvRatio = fmod(inUVTileUnit, 1.0);
	
	float2 uv = lerp(uvUpperLeft, uvUpperLeft + uvWidth, uvRatio);

	
	
    return tex2D(g_texSampler, uv) * inColor;
}

//-------------------------------------------------------------------------
// technique
//-------------------------------------------------------------------------
technique MainDraw
{
	pass P0
	{
		VertexShader = compile vs_2_0 vsBasic();
		PixelShader	 = compile ps_2_0 psBasic();
	}
}
