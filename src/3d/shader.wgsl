@group(0) @binding(0) var tex_sampler: sampler;
@group(1) @binding(0) var texture: texture_2d<f32>;

struct Vertex {
  @location(0) pos: vec3f,
  @location(1) tex_coord: vec2f,
}

struct Pipe {
  @builtin(position) pos: vec4f,
  @location(0) tex_coord: vec2f,
};

@vertex
fn vs_main(vertex: Vertex) -> Pipe {
  var out: Pipe;
  out.pos = vec4f(vertex.pos, 1.0);
  out.tex_coord = vertex.tex_coord;
  return out;
}

@fragment
fn fs_main(pipe: Pipe) -> @location(0) vec4f {
  return textureSample(texture, tex_sampler, pipe.tex_coord);
}
