#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <cassert>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../glTF/tiny_gltf.h"
#define BUFFER_OFFSET(i) ((char*)0 + (i))

#include "../common/transform.hpp"

namespace kmuvcl {
  namespace math {
    template <typename T>
    inline mat4x4f quat2mat(T x, T y, T z, T w)
    {
      T xx = x * x;
      T xy = x * y;
      T xz = x * z;
      T xw = x * w;

      T yy = y * y;
      T yz = y * z;
      T yw = y * w;

      T zz = z * z;
      T zw = z * w;

      mat4x4f mat_rot;
      mat_rot(0, 0) = 1.0f - 2.0f*(yy + zz);
      mat_rot(0, 1) = 2.0f*(xy - zw);
      mat_rot(0, 2) = 2.0f*(xz + yw);

      mat_rot(1, 0) = 2.0f*(xy + zw);
      mat_rot(1, 1) = 1.0f - 2.0f*(xx + zz);
      mat_rot(1, 2) = 2.0f*(yz - xw);

      mat_rot(2, 0) = 2.0f*(xz - yw);
      mat_rot(2, 1) = 2.0f*(yz + xw);
      mat_rot(2, 2) = 1.0f - 2.0f*(xx + yy);

      mat_rot(3, 3) = 1.0f;
      return mat_rot;
    }
  } // math
} // kmuvcl

////////////////////////////////////////////////////////////////////////////////
/// OpenGL state 초기화 관련 함수
////////////////////////////////////////////////////////////////////////////////
void init_state();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 쉐이더 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLuint  program;          // 쉐이더 프로그램 객체의 레퍼런스 값
GLint   loc_a_position;
GLint   loc_a_texcoord;

GLint   loc_u_PVM;
GLint   loc_u_texture;

GLuint create_shader_from_file(const std::string& filename, GLuint shader_type);
void init_shader_program();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 변환 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
kmuvcl::math::mat4x4f   mat_view, mat_proj;
kmuvcl::math::mat4x4f   mat_PVM;

void set_transform();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 렌더링 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
tinygltf::Model model;

GLuint position_buffer;
GLuint normal_buffer;
GLuint texcoord_buffer;
GLuint index_buffer;

GLuint texid;

bool load_model(tinygltf::Model &model, const std::string filename);
void init_buffer_objects();     // VBO init 함수: GPU의 VBO를 초기화하는 함수.
void init_texture_objects();

// scene의 노드들을 돌아가면서 그릴 수 있음.
void draw_scene();
// node를 그리는 함수.
void draw_node(const tinygltf::Node& node, kmuvcl::math::mat4f mat_view);
// node가 갖고 있는 mesh들을 그리는 함수.
void draw_mesh(const tinygltf::Mesh& mesh, const kmuvcl::math::mat4f& mat_model);
////////////////////////////////////////////////////////////////////////////////

void init_state()
{
  glEnable(GL_DEPTH_TEST);
}

// GLSL 파일을 읽어서 컴파일한 후 쉐이더 객체를 생성하는 함수
GLuint create_shader_from_file(const std::string& filename, GLuint shader_type)
{
  GLuint shader = 0;

  shader = glCreateShader(shader_type);

  std::ifstream shader_file(filename.c_str());
  std::string shader_string;

  shader_string.assign(
    (std::istreambuf_iterator<char>(shader_file)),
    std::istreambuf_iterator<char>());

  // Get rid of BOM in the head of shader_string
  // Because, some GLSL compiler (e.g., Mesa Shader compiler) cannot handle UTF-8 with BOM
  if (shader_string.compare(0, 3, "\xEF\xBB\xBF") == 0)  // Is the file marked as UTF-8?
  {
    std::cout << "Shader code (" << filename << ") is written in UTF-8 with BOM" << std::endl;
    std::cout << "  When we pass the shader code to GLSL compiler, we temporarily get rid of BOM" << std::endl;
    shader_string.erase(0, 3);                  // Now get rid of the BOM.
  }

  const GLchar* shader_src = shader_string.c_str();
  glShaderSource(shader, 1, (const GLchar * *)& shader_src, NULL);
  glCompileShader(shader);

  GLint is_compiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
  if (is_compiled != GL_TRUE)
  {
    std::cout << "Shader COMPILE error: " << std::endl;

    GLint buf_len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &buf_len);

    std::string log_string(1 + buf_len, '\0');
    glGetShaderInfoLog(shader, buf_len, 0, (GLchar *)log_string.c_str());

    std::cout << "error_log: " << log_string << std::endl;

    glDeleteShader(shader);
    shader = 0;
  }

  return shader;
}

// vertex shader와 fragment shader를 링크시켜 program을 생성하는 함수
void init_shader_program()
{
  GLuint vertex_shader
    = create_shader_from_file("./shader/vertex.glsl", GL_VERTEX_SHADER);

  std::cout << "vertex_shader id: " << vertex_shader << std::endl;
  assert(vertex_shader != 0);

  GLuint fragment_shader
    = create_shader_from_file("./shader/fragment.glsl", GL_FRAGMENT_SHADER);

  std::cout << "fragment_shader id: " << fragment_shader << std::endl;
  assert(fragment_shader != 0);

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  GLint is_linked;
  glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
  if (is_linked != GL_TRUE)
  {
    std::cout << "Shader LINK error: " << std::endl;

    GLint buf_len;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &buf_len);

    std::string log_string(1 + buf_len, '\0');
    glGetProgramInfoLog(program, buf_len, 0, (GLchar *)log_string.c_str());

    std::cout << "error_log: " << log_string << std::endl;

    glDeleteProgram(program);
    program = 0;
  }

  std::cout << "program id: " << program << std::endl;
  assert(program != 0);

  loc_u_PVM = glGetUniformLocation(program, "u_PVM");

  loc_u_texture = glGetUniformLocation(program, "u_texture");

  loc_a_position = glGetAttribLocation(program, "a_position");
  loc_a_texcoord = glGetAttribLocation(program, "a_texcoord");
}

bool load_model(tinygltf::Model &model, const std::string filename)
{
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;

  bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
  if (!warn.empty())
  {
    std::cout << "WARNING: " << warn << std::endl;
  }

  if (!err.empty())
  {
    std::cout << "ERROR: " << err << std::endl;
  }

  if (!res)
  {
    std::cout << "Failed to load glTF: " << filename << std::endl;
  }
  else
  {
    std::cout << "Loaded glTF: " << filename << std::endl;
  }

  std::cout << std::endl;

  return res;
}

void init_buffer_objects()
{
  const std::vector<tinygltf::Mesh>& meshes = model.meshes;
  const std::vector<tinygltf::Accessor>& accessors = model.accessors;
  const std::vector<tinygltf::BufferView>& bufferViews = model.bufferViews;
  const std::vector<tinygltf::Buffer>& buffers = model.buffers;

  for (const tinygltf::Mesh& mesh : meshes)
  {
    for (const tinygltf::Primitive& primitive : mesh.primitives)
    {
      const tinygltf::Accessor& accessor = accessors[primitive.indices];
      const tinygltf::BufferView& bufferView = bufferViews[accessor.bufferView];
      const tinygltf::Buffer& buffer = buffers[bufferView.buffer];

      glGenBuffers(1, &index_buffer);
      glBindBuffer(bufferView.target, index_buffer);
      glBufferData(bufferView.target, bufferView.byteLength,
        &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);

      for (const auto& attrib : primitive.attributes)
      {
        const tinygltf::Accessor& accessor = accessors[attrib.second];
        const tinygltf::BufferView& bufferView = bufferViews[accessor.bufferView];
        const tinygltf::Buffer& buffer = buffers[bufferView.buffer];

        if (attrib.first.compare("POSITION") == 0)
        {
          glGenBuffers(1, &position_buffer);
          glBindBuffer(bufferView.target, position_buffer);
          glBufferData(bufferView.target, bufferView.byteLength,
            &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
        }
        /*else if (attrib.first.compare("NORMAL") == 0)
        {
          glGenBuffers(1, &normal_buffer);
          glBindBuffer(bufferView.target, normal_buffer);
          glBufferData(bufferView.target, bufferView.byteLength,
            &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
        }*/
        else if (attrib.first.compare("TEXCOORD_0") == 0)
        {
          glGenBuffers(1, &texcoord_buffer);
          glBindBuffer(bufferView.target, texcoord_buffer);
          glBufferData(bufferView.target, bufferView.byteLength,
            &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
        }
      }
    }
  }
}

void init_texture_objects()
{
  // texture, image, sampler 들을 받아옴.
  const std::vector<tinygltf::Texture>& textures = model.textures;
  const std::vector<tinygltf::Image>& images = model.images;
  const std::vector<tinygltf::Sampler>& samplers = model.samplers;

  // texture들을 돌면서 texture를 initialize. (현재 하나밖에 없음)
  for (const tinygltf::Texture& texture : textures)
  {
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);

    const tinygltf::Image& image = images[texture.source]; // image
    const tinygltf::Sampler& sampler = samplers[texture.sampler]; // sampler

    // component가 몇 개인지 상황에 따라 처리.
    GLenum format = GL_RGBA;
    if (image.component == 1) {
      format = GL_RED; //luminance? 
    }
    else if (image.component == 2) {
      format = GL_RG;
    }
    else if (image.component == 3) {
      format = GL_RGB;
    }

    // bit type은 2가지. unsigned byte / unsigned short
    GLenum type = GL_UNSIGNED_BYTE;
    if (image.bits == 16) {
      type = GL_UNSIGNED_SHORT;
    }

    // Texture image 설정.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
      image.width, image.height, 0, format, type, &image.image[0]);

    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler.minFilter);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler.magFilter);

    // 보통 magfilter는 괜찮은데 minfilter같은 경우에 mipmap을 사용하려고 함. 
    // mipmap... GenerateMipmap을 써도 되는데 지금은 이런 식으로도 충분하다.
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler.wrapS);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler.wrapT);

    //glGenerateMipmap(GL_TEXTURE_2D);
  }
}

// glTF 파일에 view와 projection 정보들이 없으면 설정해줘야 한다. 
void set_transform()
{
  //mat_view.set_to_identity();
  mat_view = kmuvcl::math::translate(0.0f, 0.0f, -2.0f);

  //mat_proj.set_to_identity();
  float fovy = 70.0f;
  float aspectRatio = 1.0f;
  float znear = 0.01f;
  float zfar = 100.0f;

  mat_proj = kmuvcl::math::perspective(fovy, aspectRatio, znear, zfar);
}

// 그림을 그린다기보다는 Transform을 설정해주는 코드.
void draw_node(const tinygltf::Node& node, kmuvcl::math::mat4f mat_model)
{
  // node와 mesh를 받아옴.
  const std::vector<tinygltf::Node>& nodes = model.nodes;
  const std::vector<tinygltf::Mesh>& meshes = model.meshes;

  // 만약 node에 변환 정보가 있다면
  if (node.scale.size() == 3) {
    mat_model = mat_model * kmuvcl::math::scale<float>(
      node.scale[0], node.scale[1], node.scale[2]);
  }

  if (node.rotation.size() == 4) {
    mat_model = mat_model * kmuvcl::math::quat2mat(
      node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]);
  }

  if (node.translation.size() == 3) {
    mat_model = mat_model * kmuvcl::math::translate<float>(
      node.translation[0], node.translation[1], node.translation[2]);
  }

  // 그게 아니라 만약 matrix가 있다면
  if (node.matrix.size() == 16)
  {
    // matrix의 각 component를 받아와서 max_node에 넣음.
    kmuvcl::math::mat4f mat_node;
    mat_node(0, 0) = node.matrix[0];
    mat_node(0, 1) = node.matrix[1];
    mat_node(0, 2) = node.matrix[2];
    mat_node(0, 3) = node.matrix[3];

    mat_node(1, 0) = node.matrix[4];
    mat_node(1, 1) = node.matrix[5];
    mat_node(1, 2) = node.matrix[6];
    mat_node(1, 3) = node.matrix[7];

    mat_node(2, 0) = node.matrix[8];
    mat_node(2, 1) = node.matrix[9];
    mat_node(2, 2) = node.matrix[10];
    mat_node(2, 3) = node.matrix[11];

    mat_node(3, 0) = node.matrix[12];
    mat_node(3, 1) = node.matrix[13];
    mat_node(3, 2) = node.matrix[14];
    mat_node(3, 3) = node.matrix[15];

    // 기존의 mat_model에 곱해서 mat_model 업데이트
    mat_model = mat_model * mat_node;
  }

  // 만약 mesh에 해당하는 node라면
  if (node.mesh > -1)
  {
    // mesh들 중 하나를 받아서 draw_mesh를 호출.  설정된 mat_model을 넣어줌.
    draw_mesh(meshes[node.mesh], mat_model);
  }

  // node의 children node를 돈다.
  for (size_t i = 0; i < node.children.size(); ++i)
  {
    // 모든 node들을 그려줘야 한다. (recursive)
    draw_node(nodes[node.children[i]], mat_model);
  }
}

// mesh 정보와 mat_model 정보를 같이 받아온다.
void draw_mesh(const tinygltf::Mesh& mesh, const kmuvcl::math::mat4f& mat_model)
{
  const std::vector<tinygltf::Material>& materials = model.materials;
  const std::vector<tinygltf::Texture>& textures = model.textures;
  const std::vector<tinygltf::Accessor>& accessors = model.accessors;
  const std::vector<tinygltf::BufferView>& bufferViews = model.bufferViews;

  glUseProgram(program);

  mat_PVM = mat_proj * mat_view * mat_model;
  glUniformMatrix4fv(loc_u_PVM, 1, GL_FALSE, mat_PVM);
  
  // mesh에 있는 primitives 를 그릴 것이다.
  for (const tinygltf::Primitive& primitive : mesh.primitives)
  {
    // primitive가 material 정보를 갖고 있으면
    if (primitive.material > -1)
    {
      // material을 받아온 후 ,material.values <string, Parameter> pair를 parameter로 받아온다.
      const tinygltf::Material& material = materials[primitive.material];
      for (const std::pair<std::string, tinygltf::Parameter> parameter : material.values)
      {
        // parameter의 이름이 baseColorTexture이면
        if (parameter.first.compare("baseColorTexture") == 0)
        {
          // texture index가 있으면
          if (parameter.second.TextureIndex() > -1)
          {
            // 아까 설정했던 texture를 bind 해줘서 \
            이 이후에 그려지는 primitive가 이 texture에 영향을 받도록 한다.
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texid);

            glUniform1i(loc_u_texture, 0);
          }
        }
      }
    }

    // primitive의 attribute들을 받아다가 화면에 그려준다.
    for (const std::pair<std::string, int>& attrib : primitive.attributes)
    {
      const int accessor_index = attrib.second;
      const tinygltf::Accessor& accessor = accessors[accessor_index];
      const tinygltf::BufferView& bufferView = bufferViews[accessor.bufferView];      
      const int byteStride = accessor.ByteStride(bufferView);

      if (attrib.first.compare("POSITION") == 0)
      {
        glBindBuffer(bufferView.target, position_buffer);
        glEnableVertexAttribArray(loc_a_position);
        glVertexAttribPointer(loc_a_position,
          accessor.type, accessor.componentType,
          accessor.normalized ? GL_TRUE : GL_FALSE, byteStride,
          BUFFER_OFFSET(accessor.byteOffset));
      }
      /*else if (attrib.first.compare("NORMAL") == 0)
      {
        glBindBuffer(bufferView.target, normal_buffer);
        glEnableVertexAttribArray(loc_a_normal);
        glVertexAttribPointer(loc_a_normal,
          accessor.type, accessor.componentType,
          accessor.normalized ? GL_TRUE : GL_FALSE, byteStride,
          BUFFER_OFFSET(accessor.byteOffset));
      }*/
      else if (attrib.first.compare("TEXCOORD_0") == 0)
      {
        glBindBuffer(bufferView.target, texcoord_buffer);
        glEnableVertexAttribArray(loc_a_texcoord);
        glVertexAttribPointer(loc_a_texcoord,
          accessor.type, accessor.componentType,
          accessor.normalized ? GL_TRUE : GL_FALSE, byteStride,
          BUFFER_OFFSET(accessor.byteOffset));
      }
    }

    // primitive에 indices가 있는 경우에는 glDrawElements
    // primitive에 indices가 없는 경우에는 glDrawArrays  
    // 코드 만들어야 함!!
    const tinygltf::Accessor& index_accessor = accessors[primitive.indices];
    const tinygltf::BufferView& bufferView = bufferViews[index_accessor.bufferView];    

    glBindBuffer(bufferView.target, index_buffer);

    glDrawElements(primitive.mode,
      index_accessor.count,
      index_accessor.componentType,
      BUFFER_OFFSET(index_accessor.byteOffset));    

    // 정점 attribute 배열 비활성화
    glDisableVertexAttribArray(loc_a_texcoord);
    //glDisableVertexAttribArray(loc_a_normal);
    glDisableVertexAttribArray(loc_a_position);
  }
  glUseProgram(0);
}

void draw_scene()
{
  // node 받아옴.
  const std::vector<tinygltf::Node>& nodes = model.nodes;

  kmuvcl::math::mat4f mat_model;
  mat_model.set_to_identity();

  // scene들에 접근.
  for (const tinygltf::Scene& scene : model.scenes)
  {
    // scene에 있는 node들에 접근.
    for (size_t i = 0; i < scene.nodes.size(); ++i)
    {
      const tinygltf::Node& node = nodes[scene.nodes[i]];
      draw_node(node, mat_model); //identity로 설정한 mat_model을 넣어줌.
    }
  }
}

int main(void)
{
  GLFWwindow* window;

  // Initialize GLFW library
  if (!glfwInit())
    return -1;

  // Create a GLFW window containing a OpenGL context
  window = glfwCreateWindow(500, 500, "Hello Texture with glTF 2.0", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }

  // Make the current OpenGL context as one in the window
  glfwMakeContextCurrent(window);

  // Initialize GLEW library
  if (glewInit() != GLEW_OK)
    std::cout << "GLEW Init Error!" << std::endl;

  // Print out the OpenGL version supported by the graphics card in my PC
  std::cout << glGetString(GL_VERSION) << std::endl;

  init_state();
  init_shader_program();

  load_model(model, "BoxTextured/BoxTextured.gltf");

  // GPU의 VBO를 초기화하는 함수 호출
  init_buffer_objects();
  init_texture_objects();

  // Loop until the user closes the window
  while (!glfwWindowShouldClose(window))
  {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    set_transform();
    draw_scene();

    // Swap front and back buffers
    glfwSwapBuffers(window);

    // Poll for and process events
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}
