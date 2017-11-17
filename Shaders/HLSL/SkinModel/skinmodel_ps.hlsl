//
//hlsl shader for skinmodel
//


float4 g_colPtDiffuse : PointDiffuse;
float g_fDiffusePower : DirPower;
float4 g_colDiffuseFactor : DiffuseFactor = float4(1, 1, 1, 1);
float4 g_colEmissiveFactor : EmissiveFactor = float4(0, 0, 0, 0);

sampler2D g_DiffuseSampler : DiffuseMap
<
	string Texture = "g_DiffuseTexture";
	string MipFilter = "LINEAR";
	string MinFilter = "LINEAR";
	string MagFilter = "LINEAR";
	string AddressU = "WRAP";
	string AddressV = "WRAP";
>;

struct VS_OUTPUT
{
	float4 pos	            : POSITION0;
	float2 uvBase           : TEXCOORD0;
	float3 ViewDir          : TEXCOORD1;
	float3 LightDir         : TEXCOORD2;
	float4 tangent			: TEXCOORD3;
	float3 binormal			: TEXCOORD4;
	float2 highlightUV		: TEXCOORD5;
};



float4 ps_main(VS_OUTPUT Input) : COLOR0
{
	float3 Normal = float3(0, 0, 1);

	float3 LightDir = normalize(Input.LightDir);
	float4 colBaseTex = tex2D(g_DiffuseSampler, Input.uvBase);

	float fNDotL = dot(Normal, LightDir);
	fNDotL = saturate(fNDotL);

	float3 colDiffusePoint = 0.0f;

	float3 colDiffuse = (g_fDiffusePower * g_colDiffuse.rgb * fNDotL + colDiffusePoint);
	float3 colAmbient = (g_colAmbient.rgb);

	float3 ResultColor = (colDiffuse.rgb + colAmbient.rgb) * colBaseTex.rgb;

//	ResultColor.rgb = lerp(Input.fogParam.rgb, ResultColor.rgb, Input.fogParam.a);
	return float4(ResultColor, colBaseTex.a) * g_colDiffuseFactor + g_colEmissiveFactor;
}


