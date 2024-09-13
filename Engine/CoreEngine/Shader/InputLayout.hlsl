struct VertexInput_Base
{
	float3 Pos : POSITION;	// NDC(local) 좌표
	float2 UV : TEXCOORD0;	// UV 좌표
	float4 Color : COLOR;	// 정점 컬러(대부분은 사용 안함)

	float3 Normal : NORMAL;	// 법선 벡터
	float3 Tangent : TANGENT;	// 접선 벡터
	float3 Binormal : BINORMAL;	// 이접 벡터
};

struct PixelInput_Base
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR0;
	float2 UV : TEXCOORD0;
	float3 LightDir : TEXCOORD1;
	float3 ViewDir : TEXCOORD2;

	float3 Normal : TEXCOORD3;
	float3 Tangent : TEXCOORD4;
	float3 Binormal : TEXCOORD5;
};