// GLOBALS //
cbuffer ps_global			: register(b1)
{
	float4 MaterialDiffuse	: packoffset(c0);		// �g�U���ˌ�
	float4 MaterialAmbient	: packoffset(c1);		// ����
	float4 MaterialSpecular	: packoffset(c2);		// ����(w�̓p���[)
	float  UseTexture		: packoffset(c3.x);		// �e�N�X�`���[�̗L��
	float  AlphaValue		: packoffset(c3.y);		// �A���t�@�[�l
};

Texture2D Texture			: register(t0);			// �e�N�X�`��
SamplerState TextureSampler	: register(s0);			// �e�N�X�`���T���v��

// Light //
static float4 LightDiffuse = { 1.0, 1.0, 1.0, 0.0 };	// �g�U���ˌ�
static float4 LightAmbient = { 0.5, 0.5, 0.5, 0.0 };	// ����
static float4 LightSpecular = { 0.2, 0.2, 0.2, 0.0 };	// ����

// TYPEDEFS //
struct GS_OUTPUT
{
	float4 Position				: SV_POSITION;	// ���_�̈ʒu
	float2 TextureUV			: TEXCOORD0;	// �e�N�X�`���[UV�l
	float3 DirectionToView		: TEXCOORD1;	// �J���������x�N�g��
	float3 DirectionToLight		: TEXCOORD2;	// ���C�g�����x�N�g��
	float3 Normal				: TEXCOORD3;	// �@���x�N�g��
};

// FUNCTION //
float4 PS(GS_OUTPUT Input) : SV_TARGET
{
	float4 Color = float4(0.0, 0.0, 0.0, 0.0);

	// �g�U���ˌ��̊p�x�������v�Z
	float DiffuseAngle = dot(Input.Normal, Input.DirectionToLight);
	// �n�[�t�����o�[�g�ɕύX
	float HalfLambert = DiffuseAngle * 0.5 + 0.5;
	HalfLambert = clamp(HalfLambert * HalfLambert, 0.0, 1.0);

	// �g�U���ˌ� + ����
	Color.rgb = LightAmbient.rgb * MaterialAmbient.rgb;
	Color.rgb += LightDiffuse.rgb * MaterialDiffuse.rgb * HalfLambert;
	Color.a = MaterialDiffuse.a;

	// �e�N�X�`���[�K�p
	if (UseTexture > 0.0)
	{
		float4 TextureColor = Texture.Sample(TextureSampler, Input.TextureUV);
		Color.rgb *= TextureColor.rgb;
		Color.a = TextureColor.a;	// �e�N�X�`���[�̃A���t�@�l
	}

	// ���ˌ�
	float3 HalfVector = normalize(Input.DirectionToView + Input.DirectionToLight);
	float3 Specular = LightSpecular.rbg * saturate(pow(max(0.0, dot(HalfVector, normalize(Input.Normal))), MaterialSpecular.w));
	Color.rgb += Specular;

	// �A���t�@�[�l
	Color.a *= AlphaValue;

	return Color;
}