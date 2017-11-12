//
//hlsl shader for skinmodel
//

float3 g_vLightDir     : LightDirInView;
float4x4 matProjection : register(c2);
float3 g_vecEyePos     : EyePos;
float4 g_vPtAtten      : PointAttenRange;
float3 g_vPtLightPos   : PointPosInView;
float4x4 matViewInv    : ViewInverse;
float   g_fExtend : PosExtendToNormal;
float4x3 matBlend0[70]   : register(c10);

float4 g_colDiffuse : Diffuse = { 1.0f, 1.0f,1.0f,1.0f };
float4 g_colAmbient : Ambient0 = { 0.0f, 0.0f, 0.0f, 0.0f };
float4 g_colPtDiffuse : PointDiffuse;
float g_fDiffusePower : DirPower;
float4 g_colDiffuseFactor : DiffuseFactor = float4(1, 1, 1, 1);
float4 g_colEmissiveFactor : EmissiveFactor = float4(0, 0, 0, 0);

struct VS_INPUT
{
	float3 pos          : POSITION0;
	float3 weight       : BLENDWEIGHT0;
	float4 blendindices : BLENDINDICES0;
	float3 normal       : NORMAL0;
	float4 tangent      : TANGENT0;
	float2 uv           : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 pos	            : POSITION0;
	float2 uvBase           : TEXCOORD0;
	float3 ViewDir          : TEXCOORD1;
	float3 LightDir         : TEXCOORD2;
	float4 PtLightDir       : TEXCOORD3;
	float4 tangent			: TEXCOORD4;
	float3 binormal			: TEXCOORD5;
	float4 fogParam			: TEXCOORD6;
	float2 highlightUV		: TEXCOORD7;
};


sampler2D g_DiffuseSampler : DiffuseMap
<
	string Texture = "g_DiffuseTexture";
	string MipFilter = "LINEAR";
	string MinFilter = "LINEAR";
	string MagFilter = "LINEAR";
	string AddressU = "WRAP";
	string AddressV = "WRAP";
>;

VS_OUTPUT vs_main(VS_INPUT input)
{
	VS_OUTPUT output;
	float4 r0;


	int4 blendindices = input.blendindices.zyxw * 255.01;
	float4 pos = float4(input.pos, 1);
	float4 blendweight = float4(input.weight.xyz, 0);

	blendweight.w = 1.0 - blendweight.x - blendweight.y - blendweight.z;

	float4 blendPos;
	float4 blendNormal;
	float4 blendTangent;
	float4 blendBinormal;

	blendPos.xyz =
		mul(pos, (float4x3)matBlend0[blendindices.x]) * blendweight.x +
		mul(pos, (float4x3)matBlend0[blendindices.y]) * blendweight.y +
		mul(pos, (float4x3)matBlend0[blendindices.z]) * blendweight.z +
		mul(pos, (float4x3)matBlend0[blendindices.w]) * blendweight.w;
	blendPos.w = 1;

	blendNormal.xyz =
		mul(input.normal, (float3x3)matBlend0[blendindices.x]) * blendweight.x +
		mul(input.normal, (float3x3)matBlend0[blendindices.y]) * blendweight.y +
		mul(input.normal, (float3x3)matBlend0[blendindices.z]) * blendweight.z +
		mul(input.normal, (float3x3)matBlend0[blendindices.w]) * blendweight.w;
	blendNormal.xyz = normalize(blendNormal.xyz);
	blendNormal.w = 1;

#	ifdef TANGENT_AS_SRC
	blendTangent.xyz =
		mul(input.tangent.xyz, (float3x3)matBlend0[blendindices.x]) * blendweight.x +
		mul(input.tangent.xyz, (float3x3)matBlend0[blendindices.y]) * blendweight.y +
		mul(input.tangent.xyz, (float3x3)matBlend0[blendindices.z]) * blendweight.z +
		mul(input.tangent.xyz, (float3x3)matBlend0[blendindices.w]) * blendweight.w;

	blendTangent.xyz = normalize(blendTangent.xyz);
	blendTangent.w = 1;

	blendBinormal.xyz = cross(blendNormal.xyz, blendTangent.xyz) * input.tangent.w;

	blendPos.xyz += blendNormal.xyz * g_fExtend;

	output.pos = mul(blendPos, matProjection);
	output.pos.z += g_fExtend / output.pos.w;

	float3 LightDirection = -g_vLightDir.xyz;
	float3 EyeDirection = normalize(-blendPos.xyz);

#ifdef POINT_LIGHT_ENABLE
	float3 vDeltaPtLight = g_vPtLightPos - blendPos.xyz;
#endif // POINT_LIGHT_ENABLE

	output.ViewDir.x = dot(blendTangent.xyz, EyeDirection);
	output.ViewDir.y = dot(blendBinormal.xyz, EyeDirection);
	output.ViewDir.z = dot(blendNormal.xyz, EyeDirection);

	output.LightDir.x = dot(blendTangent.xyz, LightDirection);
	output.LightDir.y = dot(blendBinormal.xyz, LightDirection);
	output.LightDir.z = dot(blendNormal.xyz, LightDirection);

	output.tangent.xyz = blendTangent.xyz;
	output.tangent.w = input.tangent.w;
	output.binormal.xyz = blendBinormal.xyz;

#ifdef POINT_LIGHT_ENABLE
	output.PtLightDir.x = dot(blendTangent.xyz, vDeltaPtLight);
	output.PtLightDir.y = dot(blendBinormal.xyz, vDeltaPtLight);
	output.PtLightDir.z = dot(blendNormal.xyz, vDeltaPtLight);

	float d = length(vDeltaPtLight);
	float e = (d / g_vPtAtten.z) * g_vPtAtten.y;
	output.PtLightDir.w = g_vPtAtten.x / exp(e * e);
#else
	output.PtLightDir = float4(0, 1, 0, 0);
#endif // POINT_LIGHT_ENABLE

	// uv
	output.uvBase = input.uv;

	// Use view space normal as the addtional uv to sample highlight texture while rendering with GFX.
	output.highlightUV = blendNormal.xz * float2(0.5f, -0.5f) + 0.5f;

	float4 worldPos = mul(blendPos, matViewInv);
	// Он
//	output.fogParam = ComputeCompositeFogSimple(worldPos.xyz - g_vecEyePos, worldPos.y);

	return output;
}

float4 ps_main(VS_OUTPUT Input) : COLOR0
{
	float3 Normal = float3(0, 0, 1);

	float3 LightDir = normalize(Input.LightDir);
	float4 colBaseTex = tex2D(g_DiffuseSampler, Input.uvBase);

	float fNDotL = dot(Normal, LightDir);
	fNDotL = saturate(fNDotL);

	float3 colDiffusePoint = 0.0f;

#ifdef POINT_LIGHT_ENABLE
	float3 PtLightDir = normalize(Input.PtLightDir.rgb);
	float  fPtNDotL = dot(Normal, PtLightDir);
	fPtNDotL = saturate(fPtNDotL);
	colDiffusePoint = g_colPtDiffuse.rgb * fPtNDotL * Input.PtLightDir.w;
#endif

	float3 colDiffuse = (g_fDiffusePower * g_colDiffuse.rgb * fNDotL + colDiffusePoint);
	float3 colAmbient = (g_colAmbient.rgb);

	float3 ResultColor = (colDiffuse.rgb + colAmbient.rgb) * colBaseTex.rgb;

//	ResultColor.rgb = lerp(Input.fogParam.rgb, ResultColor.rgb, Input.fogParam.a);
	return float4(ResultColor, colBaseTex.a) * g_colDiffuseFactor + g_colEmissiveFactor;
}

technique Default
{
	pass P0
	{
		VertexShader = compile vs_2_0 vs_main();
		PixelShader = compile ps_2_0 ps_main();
	};
};

