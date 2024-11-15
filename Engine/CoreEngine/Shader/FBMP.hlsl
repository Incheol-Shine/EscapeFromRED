// MousePicking을 위한 프레임 버퍼
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};

cbuffer ColorIDConstantBuffer : register(b1)
{
	float4 color;
};

struct VertexIn_Base
{
	float3 Pos : POSITION;
};

struct PixelInput
{
	float4 Pos : SV_POSITION;
};

PixelInput VS(VertexIn_Base Input)
{
	PixelInput output;

	output.Pos = float4(Input.Pos, 1.f);

	output.Pos = mul(output.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	return output;
}



float4 PS() : SV_TARGET
{
	return color.rgba;
}
