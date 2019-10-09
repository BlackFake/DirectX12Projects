// GLOBALS //
cbuffer gs_global			: register(b0)
{
	column_major matrix matWorldTranspose;		// ���[���h�s��̓]�u�s��
	column_major matrix matViewTranspose;		// �r���[�s��̓]�u�s��
	column_major matrix matProjectionTranspose;	// �v���W�F�N�V�����s��̓]�u�s��
	column_major matrix matRotationInverseTranspose;// ��]�s��̋t�s��̓]�u�s��
	float4				ViewPosition;			// �J�����̈ʒu
	float4				DirectionalLight;		// ������
};
// TYPEDEFS //
struct VS_OUTPUT
{
	float4 Position				: POSITION;		// ���_�̈ʒu
	float4 Normal				: NORMAL;		// �@��
	float2 TextureUV			: TEXCOORD0;	// �e�N�X�`���[UV�l
};

struct GS_OUTPUT
{
	float4 Position				: SV_POSITION;	// ���_�̈ʒu
	float2 TextureUV			: TEXCOORD0;	// �e�N�X�`���[UV�l
	float3 DirectionToView		: TEXCOORD1;	// �J���������x�N�g��
	float3 DirectionToLight		: TEXCOORD2;	// ���C�g�����x�N�g��
	float3 Normal				: TEXCOORD3;	// �@���x�N�g��
};

// FUNCTION //
[maxvertexcount(3)]
void GS(triangle VS_OUTPUT Input[3], inout TriangleStream<GS_OUTPUT> Stream)
{
	GS_OUTPUT Output = (GS_OUTPUT)0;

	for (uint i = 0; i < 3; i++)
	{
		// ���[���h���W�ϊ�
		float4 WorldPosition = mul(float4(Input[i].Position.xyz, 1.0), matWorldTranspose);
		// �r���[���W�ϊ�
		Output.Position = mul(WorldPosition, matViewTranspose);
		// ���e�ϊ�
		Output.Position = mul(Output.Position, matProjectionTranspose);

		// �J���������x�N�g��
		float3 DirectionToViewPosition = normalize(ViewPosition.xyz - WorldPosition.xyz);
		DirectionToViewPosition = mul(float4(DirectionToViewPosition, 0.0), matRotationInverseTranspose).xyz;
		Output.DirectionToView = normalize(DirectionToViewPosition).xyz;

		// ���C�g�����x�N�g��
		float3 DirectionToLight = normalize(-DirectionalLight.xyz);
		DirectionToLight = mul(float4(DirectionToLight, 0.0), matRotationInverseTranspose).xyz;
		Output.DirectionToLight = normalize(DirectionToLight).xyz;

		// �@���x�N�g��
		Output.Normal = Input[i].Normal.xyz;

		// �e�N�X�`���[UV�l
		Output.TextureUV = Input[i].TextureUV;

		Stream.Append(Output);
	}
	Stream.RestartStrip();
}