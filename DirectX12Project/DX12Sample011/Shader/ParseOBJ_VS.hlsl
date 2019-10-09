// GLOBALS //

// TYPEDEFS //
struct VS_INPUT
{
	float4 Position				: POSITION;		// ���_�̈ʒu
	float4 Normal				: NORMAL;		// �@��
	float2 TextureUV			: TEXCOORD0;	// �e�N�X�`���[UV�l
};

struct VS_OUTPUT
{
	float4 Position				: POSITION;		// ���_�̈ʒu
	float4 Normal				: NORMAL;		// �@��
	float2 TextureUV			: TEXCOORD0;	// �e�N�X�`���[UV�l
};
// FUNCTION //
VS_OUTPUT VS(VS_INPUT Input)
{
	VS_OUTPUT Output = (VS_OUTPUT)0;

	Output.Position = Input.Position;
	Output.Normal = Input.Normal;
	Output.TextureUV = Input.TextureUV;

	return Output;
}