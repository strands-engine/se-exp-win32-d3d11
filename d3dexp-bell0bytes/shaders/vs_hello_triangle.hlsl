struct VSInput
{
    float3 position : POSITION; 
};

struct VSOutput
{
    float4 position : SV_POSITION;
};


VSOutput main( VSInput vin )
{
    VSOutput vout;
    
    vout.position = float4(vin.position, 1.0f);
    
	return vout;
}