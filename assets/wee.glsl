@ctype mat4 Matrix

@vs wee_vs
uniform vs_params {
    mat4 mvp;
    mat4 tm;
};

in vec4 position;
in vec2 texcoord0;
in vec4 color0;
in float psize;

out vec4 uv;
out vec4 color;

void main() {
    gl_Position = mvp * position;
    gl_PointSize = psize;
    uv = tm * vec4(texcoord0, 0.0, 1.0);
    color = color0;
}
@end

@fs wee_fs
uniform sampler2D tex;

in vec4 uv;
in vec4 color;

out vec4 frag_color;

void main() {
    frag_color = texture(tex, uv.xy) * color;
}
@end

@program wee wee_vs wee_fs
