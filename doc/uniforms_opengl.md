# OpenGL Uniform Types

[1]: https://www.khronos.org/opengl/wiki/Uniform_Buffer_Object
[2]: https://www.khronos.org/opengl/wiki/Sampler_(GLSL)#Sampler_types

Name                                            | Uniform Type             
---------------                                 | ----                     
[UniformBuffer](#uniformbuffer)                 | [Uniform Buffer Object][1]
[Sampler1D](#sampler1d-sampler2d-sampler3d)     | [sampler1D][2]
[Sampler2D](#sampler1d-sampler2d-sampler3d)     | [sampler2D][2]
[Sampler3D](#sampler1d-sampler2d-sampler3d)     | [sampler3D][2]

### Usage

##### UniformBuffer:
```
layout (binding = 0) UniformBuffer someName {
    mat4 proj;
};

struct Data {
    vec4 v4;
    mat4 m4;
};

layout (binding = 1) UniformBuffer Data someName2;

void main() {
    vec4 test = someName.proj * someName2.m4 * someName2.v4;
}
```

##### Sampler1D, Sampler2D, Sampler3D:
```
layout (binding = 0) Sampler2D someName;

void main() {
    outColor = texture(someName, texCoords);
}
```