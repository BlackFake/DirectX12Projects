// GLOBALS //

// TYPEDEFS //
struct GS_OUTPUT
{
	float4 Position				: SV_POSITION;	// ���_�̈ʒu
	float4 Color				: COLOR0;		// �F
};
// FUNCTION //
float4 PS(GS_OUTPUT Input) : SV_TARGET
{
	return Input.Color;
}