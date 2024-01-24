#pragma once

#include <string_view>

namespace prof
{

    static constexpr std::string_view vertex_shader = R"glsl(
#version 330 core

struct section
{
    float start;
    float duration;
    int   layer;
};

layout(location = 0) in vec2 _vertex_position;
uniform vec2 zoom;
uniform vec2 _screen_size;
uniform vec2 _screen_offset;
flat out int _instanceID;
// uniform float time_offset;
uniform section _sections[ 256 ];

void main()
{
    float   time_offset     = 0;
    vec2    pos             = _vertex_position;
    section current_section = _sections[ gl_InstanceID ];
    pos.x *= current_section.duration * zoom.x;
    pos.x += (current_section.start - time_offset) * zoom.x;
    pos.y *= zoom.y;
    pos.y += current_section.layer * zoom.y;
    pos.x *= _screen_size.y / _screen_size.x;
    gl_Position = vec4((pos - 0.5) * 2.0, 0.0, 1.0);
    _instanceID = gl_InstanceID;
}
	)glsl";

    static constexpr std::string_view fragment_shader = R"glsl(
#version 330 core
out vec4    _fragment_color;
flat in int _instanceID;

float rand(vec2 co) { return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453); }

vec3 hsl2rgb(in vec3 c)
{
    vec3 rgb = clamp(abs(mod(c.x * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
    return c.z + c.y * (rgb - 0.5) * (1.0 - abs(2.0 * c.z - 1.0));
}

void main() { _fragment_color = vec4(hsl2rgb(vec3(rand(vec2(_instanceID, 256 - _instanceID)), 1.0, 0.5)), 1.0); }
	)glsl";

} // namespace prof
