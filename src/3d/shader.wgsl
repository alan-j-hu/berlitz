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
  return vec4f(1.0, 0.0, 1.0, 1.0);
}
