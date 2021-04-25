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
    float4 position : POSITION;
    float4 colour : COLOUR;
};

// output struct
struct vs_output
{
    float4 position : SV_POSITION;
    float4 colour : COLOUR;
};


vs_output main(vs_input vin)
{
    vs_output vout;
    
    // adjust w position coordinate
    vin.position.w = 1.0f;
    
    // perform WVP transformation for position
    vout.position = mul(mul(mul(vin.position, world), view), projection);
    
    // copy colour data as is
    vout.colour = vin.colour;
    
    return vout;
}