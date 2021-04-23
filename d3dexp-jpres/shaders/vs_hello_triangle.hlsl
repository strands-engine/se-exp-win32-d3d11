float4 main( float2 in_pos : POSITION ) : SV_POSITION
{
	return float4(in_pos, 0.0f, 1.0f);
}