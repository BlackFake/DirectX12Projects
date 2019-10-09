// GLOBALS //

// TYPEDEFS //
struct VS_INPUT
{
	float4 Position				: POSITION;		// ���_�̈ʒu
	float4 Color				: COLOR0;		// �F
};

struct VS_OUTPUT
{
	float4 Position				: POSITION;		// ���_�̈ʒu
	float4 Color				: COLOR0;		// �F
};
// FUNCTION //
VS_OUTPUT VS(VS_INPUT Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;

	Output.Position = Input.Position;
	Output.Color = Input.Color;

	return Output;
}