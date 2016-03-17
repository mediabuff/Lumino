//=============================================================================
// PrimitiveRendererForBlt (HLSL:DX9)
//=============================================================================
#ifdef LN_HLSL_DX9

float2          g_viewportSize   : VIEWPORTPIXELSIZE;
static float2   g_viewportOffset = (float2(0.5, 0.5) / g_viewportSize);

float       gBlurPower;         // �u���[�̋���
float4      gBlurColor;         // �u���[�̐F
float4x4    gBlurMatrix;        // �u���[�C���[�W�̍��W�ϊ��s��

texture		gSecondaryTexture;
sampler2D	g_texSampler = sampler_state
{
	texture = <gSecondaryTexture>;
    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = NONE;
    AddressU  = CLAMP;
    AddressV  = CLAMP;
};

struct VS_OUTPUT
{
	float4 pos		: POSITION;
	float2 texUV	: TEXCOORD0;
};

//-------------------------------------------------------------------------
// vertex shader
//-------------------------------------------------------------------------
VS_OUTPUT vsBasic(
	float3 inPos	: POSITION,
	float2 inUV		: TEXCOORD0)
{
    VS_OUTPUT o;
    o.pos = mul( float4( inPos, 1.0f ), gBlurMatrix );
    o.texUV = inUV;// + g_viewportOffset;
    return o;
}

//-------------------------------------------------------------------------
// pixel shader
//-------------------------------------------------------------------------
float4 psBasic(
	float2 inUV		: TEXCOORD0) : COLOR0
{
    float4 out_color = tex2D( g_texSampler, inUV );
    out_color.a *= gBlurPower;
    out_color *= gBlurColor;
    
    // �u���[����ۂ�����
    //out_color.rgb *= float3(0, 0, 1);
    
    // �O���قǓ����ɂȂ�悤�ɂ���
    //float len = length( in_uv_ - 0.5 );
    //color.a *= 1.0 - len;
    
    return out_color;
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
#endif

//=============================================================================
// PrimitiveRendererForBlt (GLSL)
//=============================================================================
#ifdef LN_GLSL_VERTEX

attribute vec3	ln_Vertex0;
attribute vec4	ln_Color0;
attribute vec2	ln_MultiTexCoord0;

varying vec4	v_Color;
varying vec2	v_TexUV;

void main()
{
	gl_Position = vec4(ln_Vertex0, 1.0);
	v_Color = ln_Color0;
	v_TexUV = ln_MultiTexCoord0;
}
#endif

#ifdef LN_GLSL_FRAGMENT

uniform sampler2D	g_texture;
varying vec4		v_Color;
varying vec2		v_TexUV;

void main()
{
	vec4 color = texture2D(g_texture, v_TexUV);
	gl_FragColor = color * v_Color;
}

#endif
