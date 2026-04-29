#include <metal_stdlib>
using namespace metal;

struct BoardVertex {
    float2 position;
    float4 color;
};

struct RasterVertex {
    float4 position [[position]];
    float4 color;
};

vertex RasterVertex board_vertex(const device BoardVertex *vertices [[buffer(0)]],
                                 uint vertex_id [[vertex_id]]) {
    RasterVertex out;
    out.position = float4(vertices[vertex_id].position, 0.0, 1.0);
    out.color = vertices[vertex_id].color;
    return out;
}

fragment float4 board_fragment(RasterVertex in [[stage_in]]) {
    return in.color;
}

