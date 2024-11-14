#include "CommonConstantBuffers.hlslinc"
#include "InputLayout.hlslinc"

Texture2D g_DiffuseTexture : register(t0);
Texture2D g_NormalTexture : register(t1);
Texture2D g_AmbientOcclusionTexture : register(t2);
Texture2D g_RoughnessTexture : register(t3);
Texture2D g_MetallicTexture : register(t4);

Buffer<float4> g_BoneTransforms : register(t5);

SamplerState g_DiffuseSampler : register(s0);
SamplerState g_TextureSampler0 : register(s1);
SamplerState g_TextureSampler1 : register(s2);

/**
 * 버퍼로부터 본의 변환 행렬을 가져온다.
 */
float4x4 FetchBoneTransform(uint Bone)
{
	Bone *= 4;	// 4x4 행렬이므로 4를 곱해준다.
	float4 row1 = g_BoneTransforms.Load(Bone + 0);	// 4x4 행렬의 첫번째 행
	float4 row2 = g_BoneTransforms.Load(Bone + 1);	// 4x4 행렬의 두번째 행
	float4 row3 = g_BoneTransforms.Load(Bone + 2);	// 4x4 행렬의 세번째 행
	float4 row4 = g_BoneTransforms.Load(Bone + 3);	// 4x4 행렬의 네번째 행

	float4x4 Matrix = float4x4(row1, row2, row3, row4);	// 4x4 행렬 생성
	return Matrix;
}

PixelInput_Base VS(VertexIn_Base Input, uint InstanceID : SV_InstanceID)
{
	PixelInput_Base output;

	output.Color    = Input.Color;
	output.Pos      = float4(Input.Pos, 1.f);	// 로컬 좌표계
	float4 localPos = output.Pos;
	float3 normal;

	if (MeshFlags & FLAG_MESH_ANIMATED || MeshFlags & FLAG_MESH_SKINNED)
	{
		for (int i = 0; i < 4; ++i)
		{
			const uint  boneIndex = (uint)Input.BoneIndices[i];	// 본 인덱스
			const float weight    = Input.BoneWeights[i];	// 가중치

			float4x4 boneTransform = FetchBoneTransform(boneIndex);	// 본의 변환 행렬을 가져온다.
			output.Pos += (100 * weight *
				mul(localPos, boneTransform));	// local(원래 메시 좌표) * boneTransform(애니메이션 변환 행렬) * weight(가중치)

			if (i == 0)
			{
				output.Color.x = weight;
			}
			else if (i == 1)
			{
				output.Color.y = weight;
			}
			else if (i == 2)
			{
				output.Color.z = weight;
			}
			else if (i == 3)
			{
				output.Color.w = weight;
			}
			normal += mul(Input.Normal, (float3x3)boneTransform) *
					weight;	// normal(원래 메시 노말) * boneTransform(애니메이션 변환 행렬) * weight(가중치)
		}
	}
	else
	{
		normal = Input.Normal;
	}

	output.Pos     = mul(output.Pos, World);
	float3 viewDir = WorldCameraPos.xyz - output.Pos.xyz;

	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	float3 worldNormal   = mul(normal, (float3x3)World);
	float3 worldTangent  = mul(Input.Tangent, (float3x3)World);
	float3 worldBinormal = mul(Input.Binormal, (float3x3)World);

	output.UV      = Input.UV;
	output.ViewDir = normalize(viewDir);

	output.Normal   = normalize(worldNormal);
	output.Tangent  = normalize(worldTangent);
	output.Binormal = normalize(worldBinormal);

	return output;
}


float4 PS(PixelInput_Base Input) : SV_TARGET
{
	// if (MeshFlags & FLAG_MESH_SKINNED)
	// {
	// 	// ! FOR DEBUG !
	// 	// 가중치 표현 (가중치는 잘 적용이 됐는데 스케일 100을 곱해줘야하는 이유?)
	// 	return float4(Input.Color.rgb, 1);
	// }

	float3 diffuseColor = Diffuse.Color;

	float3 normalColor = normalize(Input.Normal /** 2.0f - 1.0f*/); // -1 ~ 1 사이로 정규화

	// 	R (Red): Ambient Occlusion (AO)
	//  G (Green): Roughness
	//  B (Blue): Metallic
	float ambientColor = AmbientOcclusion.Value;
	float roughness    = Roughness.Value;
	float metallic     = Metallic.Value;

	// Texture Map
	if (TextureUsageFlag & TEXTURE_USE_DIFFUSE)
	{
		diffuseColor = g_DiffuseTexture.Sample(g_DiffuseSampler, Input.UV);
	}
	if (TextureUsageFlag & TEXTURE_USE_NORMAL)
	{
		normalColor = g_NormalTexture.Sample(g_DiffuseSampler, Input.UV).rgb;
		normalColor = normalize(normalColor * 2.0f - 1.0f);
	}
	if (TextureUsageFlag & TEXTURE_USE_AMBIENTOCCLUSION)
	{
		ambientColor = g_AmbientOcclusionTexture.Sample(g_DiffuseSampler, Input.UV).r;
	}
	if (TextureUsageFlag & TEXTURE_USE_ROUGHNESS)
	{
		roughness = g_RoughnessTexture.Sample(g_DiffuseSampler, Input.UV).g;
	}
	if (TextureUsageFlag & TEXTURE_USE_METALLIC)
	{
		metallic = g_MetallicTexture.Sample(g_DiffuseSampler, Input.UV).b;
	}

	// 아래 값들은 이미 VS에서 정규화 되었지만 보간기에서 보간(선형 보간)된 값들이므로 다시 정규화
	float3 lightDir = normalize(DirectionalLightPos.xyz);
	float  NdotL    = saturate(dot(normalColor, lightDir));
	float3 diffuse  = NdotL * diffuseColor.rgb * DirectionalLightColor.rgb;
	float3 specular = 0;
	// return float4(NdotL , NdotL, NdotL, 1.f);
	// 난반사광이 없으면 애초에 반사되는 색상이 없음
	if (diffuse.x > 0)
	{
		// 노말과 라이트의 반사각
		float3 reflection = reflect(lightDir, normalColor);
		float3 viewDir    = normalize(Input.ViewDir);

		// Specular는 카메라뷰와 반사각의 내적값을 제곱(할수록 광 나는 부분이 줄어듦) 사용
		specular = saturate(dot(reflection, -viewDir));
		specular = pow(specular, 1 / roughness);
	}

	// 주변광 (없으면 반사광이 없는곳은 아무것도 보이지 않음)
	float3 ambient = /*Ambient.Color;*/ 0.2 * diffuseColor;
	//
	float3 finalColor = float3(diffuse + ambient + (specular * metallic));
	// finalColor.rgb    = lerp(finalColor.rgb, finalColor.rgb * metallic, metallic);

	return float4(finalColor, 1.f);

}
