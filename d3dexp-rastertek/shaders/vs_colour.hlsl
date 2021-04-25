// globals
cbuffer mx_buffer
{
    matrix world;
    matrix view;
    matrix projection;
};

// input struct
struct vs_input
{
    float3 position : POSITION;
    float4 colour   : COLOUR;
};

// output struct
struct vs_output
{
    float4 colour   : COLOUR;
    float4 position : SV_POSITION;
};


vs_output main(vs_input vin)
{
    vs_output vout;
    
    // adjust w position coordinate
    vout.position = float4(vin.position, 1.0f);
    
    // perform WVP transformation for position
    vout.position = mul(vout.position, world);
    vout.position = mul(vout.position, view);
    vout.position = mul(vout.position, projection);
    
    // copy colour data as is
    vout.colour = vin.colour;
    
    return vout;
}