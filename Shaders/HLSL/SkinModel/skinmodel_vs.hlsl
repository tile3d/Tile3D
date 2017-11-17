//
//hlsl shader for skinmodel
//

float4x4 matProjection : register(c0);
float4x4 matViewInv    : register(c4);
float3 g_vLightDir     : register(c8);
//float3 g_vecEyePos     : register(c9);
float   g_fExtend	   : register(c9);;
float4x3 matBlend0[70] : register(c10);


struct VS_INPUT
{
	float3 pos          : POSITION0;
	float3 weight       : BLENDWEIGHT0;
	float4 blendindices : BLENDINDICES0;
	float3 normal       : NORMAL0;
	float2 uv           : TEXCOORD0;
	float4 tangent      : TANGENT0;
};


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


VS_OUTPUT vs_main(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
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

	output.ViewDir.x = dot(blendTangent.xyz, EyeDirection);
	output.ViewDir.y = dot(blendBinormal.xyz, EyeDirection);
	output.ViewDir.z = dot(blendNormal.xyz, EyeDirection);

	output.LightDir.x = dot(blendTangent.xyz, LightDirection);
	output.LightDir.y = dot(blendBinormal.xyz, LightDirection);
	output.LightDir.z = dot(blendNormal.xyz, LightDirection);

	output.tangent.xyz = blendTangent.xyz;
	output.tangent.w = input.tangent.w;

	output.binormal.xyz = blendBinormal.xyz;

	output.PtLightDir = float4(0, 1, 0, 0);

	// uv
	output.uvBase = input.uv;

	// Use view space normal as the addtional uv to sample highlight texture while rendering with GFX.
	output.highlightUV = blendNormal.xz * float2(0.5f, -0.5f) + 0.5f;

	float4 worldPos = mul(blendPos, matViewInv);

//	output.fogParam = ComputeCompositeFogSimple(worldPos.xyz - g_vecEyePos, worldPos.y);

	return output;
}

