// GLOBALS //

// TYPEDEFS //
struct GS_OUTPUT
{
	float4 Position				: SV_POSITION;	// 頂点の位置
	float4 Color				: COLOR0;		// 色
};
// FUNCTION //
float4 PS(GS_OUTPUT Input) : SV_TARGET
{
	return Input.Color;
}