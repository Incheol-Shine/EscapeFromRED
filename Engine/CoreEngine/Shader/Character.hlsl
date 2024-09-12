//#define HALF_VECTOR
struct VS_INPUT
{
	float3 Pos : POSITION;
	float3 Norm : NORMAL;
	float4 Col : COLOR;    // COLOR0 ~ COLOR1
	float2 Tex : TEXCOORD0; // TEXCOORD0 ~ TEXCOORD15

	float4 Idx : INDEX;
	float4 Weight : WEIGHT;
	float3 Tan : TANGENT;
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION; // 3 -> 4
	float3 Norm : NORMAL;
	float4 Col : COLOR0;    // COLOR0 ~ COLOR1
	float2 Tex : TEXCOORD0; // TEXCOORD0 ~ TEXCOORD15
	float3 r : TEXCOORD1;
	float3 l : TEXCOORD2;
	float3 e : TEXCOORD3;
	float4 Shadow : TEXCOORD4;
};

// 상수버퍼(단위:레지스터 단위(float4)로 할당되어야 한다.)
cbuffer cb0 : register(b0)
{
	// 1개의 레지스터(x,y,z,w)
	matrix g_matWorld : packoffset(c0);
	matrix g_matView : packoffset(c4);
	matrix g_matProj : packoffset(c8);
	matrix g_matNormal : packoffset(c12);
	float4 Color0 : packoffset(c16);
	float  TimerX : packoffset(c17.x); // Timer.x, Timer.y, Timer.z, Timer.w	
};

cbuffer cb1 : register(b1)
{
	float4 vLightDir : packoffset(c0);
	float4 vLightPos : packoffset(c1);
	float4 vEyeDir : packoffset(c2);
	float4 vEyePos : packoffset(c3);
	matrix g_matLight: packoffset(c4);
};

cbuffer cb2 : register(b2)
{
	float4x4 g_matBoneWorld[255]; //65535 / 4
}

VS_OUTPUT VS(VS_INPUT v)
{
	VS_OUTPUT pOut   = (VS_OUTPUT)0;
	uint      iIndex = v.Col.w;
	float4    vLocal = float4(v.Pos.xyz, 1.0f);// float4(v.p.x, v.p.y, v.p.z, 1.0f);
	//float4 vWorld = mul(vLocal, g_matBoneWorld[iIndex]);
	float4 vWorld  = 0;
	float3 vNormal = 0;
	for (int iBone = 0; iBone < 4; iBone++)
	{
		uint iBoneIndex = v.Idx[iBone];
		vWorld += mul(vLocal, g_matBoneWorld[iBoneIndex]) * v.Weight[iBone];
		vNormal += mul(v.Norm, (float3x3)g_matBoneWorld[iBoneIndex]) * v.Weight[iBone];
	}

	vWorld      = mul(vWorld, g_matWorld);
	vNormal     = mul(vNormal, (float3x3)g_matNormal);
	pOut.Shadow = mul(vWorld, g_matLight);

	float4 vView = mul(vWorld, g_matView);
	float4 vProj = mul(vView, g_matProj);
	pOut.Pos     = vProj;
	pOut.Norm    = normalize(vNormal);
	pOut.Tex     = v.Tan * 10;
	pOut.Col     = v.Col;

	float3 T = normalize(mul(v.Tan, (float3x3)g_matNormal));
	float3 B = normalize(cross(vNormal, T));

	float3x3 tanMat = {
		T.x, B.x, vNormal.x,
		T.y, B.y, vNormal.y,
		T.z, B.z, vNormal.z
	};
	pOut.l = normalize(mul(-vLightDir.xyz, tanMat));
	pOut.e = normalize(mul(vEyeDir.xyz, tanMat));
	pOut.r = reflect(vEyeDir.xyz, pOut.Norm);
	return pOut;
}

VS_OUTPUT VSColor(VS_INPUT v)
{
	VS_OUTPUT pOut   = (VS_OUTPUT)0;
	float4    vLocal = float4(v.Pos.xyz, 1.0f);// float4(v.p.x, v.p.y, v.p.z, 1.0f);
	float4    vWorld = mul(vLocal, g_matWorld);
	float4    vView  = mul(vWorld, g_matView);
	float4    vProj  = mul(vView, g_matProj);
	pOut.Pos         = vProj;
	pOut.Col         = v.Col;
	pOut.r           = normalize(vLocal.xyz);
	return pOut;
}

Texture2D   g_txColor : register(t0);
Texture2D   g_txMask : register(t1);
TextureCube g_txCubeMap : register(t3);
Texture2D   g_txNormalMap : register(t4);
Texture2D   g_txShadowMap : register(t5);

SamplerState g_Sample : register(s0);
SamplerState g_SampleClamp : register(s1);

float Specular(float3 vNormal)
{
	// Specular Lighting
	float fPower = 0.0f;
#ifndef HALF_VECTOR
	float3 R = reflect(vLightDir, vNormal);
	fPower   = pow(saturate(dot(R, -vEyeDir)), 5.0f);
#else
	float3 vHalf = normalize(-vLightDir + -vEyeDir);
	fPower = pow(saturate(dot(vNormal, vHalf)), 5.0f);
#endif
	float4 specular = float4(fPower, fPower, fPower, 1.0f);
	return fPower;
}

float Diffuse(float3 vNormal)
{
	float fIntensity = max(0,
						   dot(vNormal, normalize(-vLightDir)));
	//float4 diffuse = fIntensity;
	return fIntensity;
}

struct PBUFFER_OUTPUT
{
	float4 color0 : SV_TARGET0;
	float4 color1 : SV_TARGET1;
};

PBUFFER_OUTPUT PSMRT(VS_OUTPUT input) : SV_TARGET
{
	PBUFFER_OUTPUT output;
	//텍스처에서 t좌표에 해당하는 컬러값(픽셀) 반환
	float4 color  = g_txColor.Sample(g_Sample, input.Tex);
	float4 shadow = g_txShadowMap.Sample(g_SampleClamp, input.Shadow.xy / input.Shadow.w);
	uint   id     = shadow * 255.0f;
	if (id != (uint)TimerX && shadow.r > 0.0f)
	{
		color = color * float4(0.5f, 0.5f, 0.5f, 1);
	}
	else
	{
		color = color;
	}
	float4 normal = g_txNormalMap.Sample(g_Sample, input.Tex);
	normal        = normalize((normal - 0.5f) * 2.0f);
	float  fDot   = saturate(dot(normal.xyz, input.l));
	float4 final  = color * float4(fDot, fDot, fDot, 1);
	// 0 ~ 1 -> 0.0f ~ 0.5f
	float2 uv = float2(fDot, 0.5f);
	//float4 mask = g_txMask.Sample(g_Sample, uv);
	// 소스알파(1) = 마스크이미지의 검정색부분은 불투명된다.
	// 소스알파(0) = 마스크이미지의 흰색부분은   투명된다.
	//final = color*mask;// *mask;// *Color0;	
	//final = final * Specular(normal);
	//final.a = 1.0f;
	/* 알파테스팅 (완전 투명과 완전 불투명 일 때 사용)
	 장점 : 순서를 구분하기 어려운 오브젝트 랜더링시
			정렬된 상태와 유사하게  랜더링된다.*/
	//if (final.a < 0.5f)
	//{
	//	discard;
	//}
	float4 vCube = g_txCubeMap.Sample(g_Sample, input.r);
	final        = lerp(final, vCube, 0.5f);
	final.a      = 1.0f;

	output.color0  = final;
	float3 vNormal = input.Norm * 0.5f + 0.5f;
	// 필수->알파블랜딩 =OFF;
	output.color1 = float4(vNormal, input.Col.a);
	return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	//텍스처에서 t좌표에 해당하는 컬러값(픽셀) 반환
	float4 color  = g_txColor.Sample(g_Sample, input.Tex);
	float4 normal = g_txNormalMap.Sample(g_Sample, input.Tex);
	normal        = normalize((normal - 0.5f) * 2.0f);
	float  fDot   = saturate(dot(normal.xyz, input.l));
	float4 final  = color * float4(fDot, fDot, fDot, 1);
	// 0 ~ 1 -> 0.0f ~ 0.5f
	float2 uv = float2(fDot, 0.5f);
	//float4 mask = g_txMask.Sample(g_Sample, uv);
	// 소스알파(1) = 마스크이미지의 검정색부분은 불투명된다.
	// 소스알파(0) = 마스크이미지의 흰색부분은   투명된다.
	//final = color*mask;// *mask;// *Color0;	
	final   = final * Specular(normal);
	final.a = 1.0f;
	/* 알파테스팅 (완전 투명과 완전 불투명 일 때 사용)
	 장점 : 순서를 구분하기 어려운 오브젝트 랜더링시
			정렬된 상태와 유사하게  랜더링된다.*/
	//if (final.a < 0.5f)
	//{
	//	discard;
	//}
	float4 vCube = g_txCubeMap.Sample(g_Sample, input.r);
	final        = lerp(final, vCube, 0.5f);
	final.a      = 1.0f;
	return float4(fDot, fDot, fDot, 1);
}

float4 PSAlphaBlend(VS_OUTPUT input) : SV_TARGET
{
	float4 color = g_txColor.Sample(g_Sample, input.Tex);
	float4 final = color * input.Col;
	final.a      = color.a;
	return final;
}

float4 PSColor(VS_OUTPUT input) : SV_TARGET
{
	float4 vColor = input.Col;
	vColor.a      = 0.5f * input.Col.a;
	return vColor;
}
