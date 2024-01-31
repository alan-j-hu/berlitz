@group(0) @binding(0) var texture: texture_2d<f32>;

struct Vertex {
  @location(0) pos: vec3f,
}

struct Pipe {
  @builtin(position) pos: vec4f,
};

@vertex
fn vs_main(vertex: Vertex) -> Pipe {
  var out: Pipe;
  out.pos = vec4f(vertex.pos, 1.0);
  return out;
}

@fragment
fn fs_main(pipe: Pipe) -> @location(0) vec4f {
  return textureLoad(texture, vec2i(pipe.pos.xy), 0);
}
