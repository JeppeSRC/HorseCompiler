# TheHolyShadingLanguage

### Table Of Content

- [Introduction](#introduction)
- [Data Types](#data-types)
- [Pipeline Layout](#pipeline-layout)
- [Builtin](#builtin)
-- [Variables](#variables)
-- [Functions](#functions)
- [Example](#example)

## Introduction

TheHolyShadingLanguage is a shading language that compiles to [SPIR-V](https://www.khronos.org/registry/spir-v/) for use in [Vulkan](https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html) or [OpenGL](https://www.khronos.org/opengl/?/) (Since version 4.6)

Currently supported stages are [Vertex](https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#shaders-vertex) and [Fragment](https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#shaders-fragment) shaders.

## Data Types

#### Integers:
- byte  (8 bit)
- short (16 bit)
- int   (32 bit)
- long  (64 bit)

All integers are signed by default but can be set explicitly as signed or unsigned.
```
signed int var;
unsigned long var2;
```

#### Floating-points:
- float (32 bit)
- double (64 bit)

#### Vectors:
- vec2
- dvec2
- vec3
- dvec3
- vec4
- dvec4

#### Matrices:
- mat3
- dmat3
- mat4
- dmat4

Vector and Matrix types prefixed with `d` are 64 bit floats and those without the prefix are 32 bit floats.

##### NOTE:
> All 64 bit types are currently unsupported

## Pipeline Layout

For a shader stage to function you need to define variables and associate those variables to the input data.

#### Input And Output Layout:

```
layout (location = <N>, component = <M>) in <type> <name>
layout (location = <N>) out <type> <name>
```

Where `N` is the shader binding location number and `M` the component. component is optional and can be completely omitted.


Read more about vertex attributes: [Vulkan](https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#fxvertex-attrib), [OpenGL](https://www.khronos.org/opengl/wiki/Layout_Qualifier_(GLSL)#Interface_layouts)

#### Uniforms

##### Vulkan
```
layout (binding = <N>, set = <M>) <uniform_type> <type> <name>
```

Where `N` is the binding within [DescriptorSet](https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#descriptorsets-sets) `M`. `set` is optional and can be completely omitted, it will default to 0.

List of supported [Uniform Types](uniforms_vulkan.md)

##### OpenGL
```
layout (binding = <N>) <uniform_type> <type> <name>
```

Where `N` is the location number of the uniform.

List of supported [Uniform Types](uniforms_opengl.md)

## Builtin

#### Variables

Name            | Type          | Shader
---             | ----          | -----
Position        | Output        | Vertex
PointSize       | Input         | Vertex
VertexID        | Input         | Vertex
InstanceID      | Input         | Vertex
FragCoord       | Input         | Fragment
FragDepth       | Output        | Fragment

##### Usage:
```
<in/out> <type> <name> = <builtin_name>;

out vec4 pos = Position;
in vec4 fragCoord = FragCoord;
```

More info in [SPIR-V Documentation](https://www.khronos.org/registry/spir-v/specs/unified1/SPIRV.html#_a_id_builtin_a_builtin)

#### Functions

[SPIR-V Extended Instructions](https://www.khronos.org/registry/spir-v/specs/1.0/GLSL.std.450.html) for a list of builtin functions. 

## Example

Simple example with a vertex and fragment stage that renders a textured model.

### Vertex Shader:
```
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec4 color;

layout (location = 0) out vec2 texCoordToFrag;
layout (location = 1) out vec4 colorToFrag;

layout (binding = 0) UniformBuffer data {
    mat4 model;
    mat4 view;
    mat4 proj;
};

layout (Position) out vec4 pos;

void main() {
    pos = data.proj * data.view * data.model * vec4(position, 1);

    colorToFrag = color;
    texCoordToFrag = texCoord;
}

```

### Fragment Shader

```
layout (location = 0) in vec2 texCoord;
layout (location = 1) in vec4 color;

layout (location = 0) out vec4 outColor;

layout (binding = 1) sampler2D sampler;

void main() {
    outColor = texture(sampler, texCoord) * color;
}

```