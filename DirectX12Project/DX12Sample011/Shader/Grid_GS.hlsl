// GLOBALS //
cbuffer gs_global			: register(b0)
{
	column_major matrix matWorldTranspose;		// ���[���h�s��̓]�u�s��
	column_major matrix matViewTranspose;		// �r���[�s��̓]�u�s��
	column_major matrix matProjectionTranspose;	// �v���W�F�N�V�����s��̓]�u�s��
};
// TYPEDEFS //
struct VS_OUTPUT
{
	float4 Position				: POSITION;		// ���_�̈ʒu
	float4 Color				: COLOR0;		// �F
};

struct GS_OUTPUT
{
	float4 Position				: SV_POSITION;	// ���_�̈ʒu
	float4 Color				: COLOR0;		// �F
};
// FUNCTION //
[maxvertexcount(2)]
void GS(line VS_OUTPUT Input[2], inout LineStream<GS_OUTPUT> Stream)
{
	GS_OUTPUT Output = (GS_OUTPUT)0;

	for (uint i = 0; i < 2; i++)
	{
		// ���[���h���W�ϊ�
		float4 WorldPosition = mul(float4(Input[i].Position.xyz, 1.0), matWorldTranspose);
		// �r���[���W�ϊ�
		Output.Position = mul(WorldPosition, matViewTranspose);
		// ���e�ϊ�
		Output.Position = mul(Output.Position, matProjectionTranspose);
		// �F
		Output.Color = Input[i].Color;

		Stream.Append(Output);
	}
	Stream.RestartStrip();
}