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
    // quaternion을 rotation matrix로 바꿔주는 함수.
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

    const float MATH_PI = 3.14159265358979323846f;

    template <typename T>
    // glTF함수에서는 OpenGL과는 다르게 각도 값들이 radian으로 되어있다.
    // degree <-> radian 변환함수.
    inline T rad2deg(T deg)
    {
      T rad = deg * (180.0f / MATH_PI);
      return rad;
    }

    template <typename T>
    inline T deg2rad(T rad)
    {
      T deg = rad * (MATH_PI / 180.0f);
      return deg;
    }
  } // math
} // kmuvcl

////////////////////////////////////////////////////////////////////////////////
/// 쉐이더 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLuint  program;          // 쉐이더 프로그램 객체의 레퍼런스 값
GLint   loc_a_position;   // attribute 변수 a_position 위치
GLint   loc_u_PVM; // uniform 변수 u_PVM 위치

GLuint  position_buffer;  // GPU 메모리에서 position_buffer의 위치
GLuint  index_buffer;     

GLuint create_shader_from_file(const std::string& filename, GLuint shader_type);
void init_shader_program();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 렌더링 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
tinygltf::Model model;

bool load_model(tinygltf::Model &model, const std::string filename);
void init_buffer_objects();     // VBO init 함수: GPU의 VBO를 초기화하는 함수.

void render_object();           // rendering 함수: 물체(삼각형)를 렌더링하는 함수.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 변환 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
kmuvcl::math::mat4x4f     mat_model, mat_view, mat_proj;
kmuvcl::math::mat4x4f     mat_PVM;

void set_transform();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 카메라 관련 변수
////////////////////////////////////////////////////////////////////////////////
int camera_index = 0;
////////////////////////////////////////////////////////////////////////////////


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

    loc_a_position = glGetAttribLocation(program, "a_position");    
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

  // mesh 접근.
  for (const tinygltf::Mesh& mesh : meshes)
  {
    // primitive 접근.
    for (const tinygltf::Primitive& primitive : mesh.primitives)
    {
      // primitive를 그리는 indices 에 대한 accessor 를 이용해 bufferView, buffer에 접근.
      const tinygltf::Accessor& accessor = accessors[primitive.indices];
      const tinygltf::BufferView& bufferView = bufferViews[accessor.bufferView];
      const tinygltf::Buffer& buffer = buffers[bufferView.buffer];

      // index에 대한 buffer object를 만든다.
      glGenBuffers(1, &index_buffer);
      glBindBuffer(bufferView.target, index_buffer);
      glBufferData(bufferView.target, bufferView.byteLength,
          &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);


      // 각 attribute에 접근.
      for (const auto& attrib : primitive.attributes)
      {
        const tinygltf::Accessor& accessor = accessors[attrib.second];
        const tinygltf::BufferView& bufferView = bufferViews[accessor.bufferView];
        const tinygltf::Buffer& buffer = buffers[bufferView.buffer];

        // 아까 POSITION밖에 없었으므로 이것만.
        // general한 다양한 glTF파일을 열 수 있는 프로그램을 짜려면 상황에 따라서 \
        다양한 shader 코드를 불러온다든지, 데이터를 어떻게 처리할지에 대해서 코드를 짜야 한다.
        if (attrib.first.compare("POSITION") == 0)
        {
          glGenBuffers(1, &position_buffer);
          glBindBuffer(bufferView.target, position_buffer);
          glBufferData(bufferView.target, bufferView.byteLength,
              &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
        }
      }
    }
  }
}

//mat_proj & mat_view 설정.
void set_transform()
{
  // node와 camera를 받아옴.
	const std::vector<tinygltf::Node>& nodes = model.nodes;
  const std::vector<tinygltf::Camera>& cameras = model.cameras;

  // Projection Matrix 설정.
  const tinygltf::Camera& camera = cameras[camera_index];
  
  if (camera.type.compare("perspective") == 0) // camera_index가 가리키는 Camera가 perspective이면
  {
    // yfov 는 radian값이라서 degree로 바꿔줌.
    float fovy = kmuvcl::math::rad2deg(camera.perspective.yfov);
    float aspectRatio = camera.perspective.aspectRatio;
    float znear = camera.perspective.znear;
    float zfar = camera.perspective.zfar;

    /*std::cout << "(camera.mode() == Camera::kPerspective)" << std::endl;
    std::cout << "(fovy, aspect, n, f): " << fovy << ", " << aspectRatio << ", " << znear << ", " << zfar << std::endl;*/
    mat_proj = kmuvcl::math::perspective(fovy, aspectRatio, znear, zfar);
  }
  else // (camera.type.compare("orthographic") == 0)
  {
    float xmag = camera.orthographic.xmag; // right
    float ymag = camera.orthographic.ymag; // top
    float znear = camera.orthographic.znear;
    float zfar = camera.orthographic.zfar;

    /*std::cout << "(camera.mode() == Camera::kOrtho)" << std::endl;
    std::cout << "(xmag, ymag, n, f): " << xmag << ", " << ymag << ", " << znear << ", " << zfar << std::endl;*/
    mat_proj = kmuvcl::math::ortho(-xmag, xmag, -ymag, ymag, znear, zfar);
  }

  // View Matrix 설정. (적용할 때는 V의 inverse를 써야 함)
  
  // node에 접근.
  for (const tinygltf::Node& node : nodes)
  {
    // node 가 camera에 대한 node인 경우, 그 camera가 camera_index와 동일하다면 \
    이 node의 transform 정보를 받아온다.
    if (node.camera == camera_index)
    {
      mat_view.set_to_identity();
      // node.scale 은 값이 3개이다. (x,y,z) \  값이 있다면 size가 3일 것임. 
      // inverse를 만들어주기 위해 각각 만들 때 inverse 형식으로 만들겠다.
      if (node.scale.size() == 3) {
        mat_view = mat_view*kmuvcl::math::scale<float>(
              1.0f / node.scale[0], 1.0f / node.scale[1], 1.0f / node.scale[2]);
      }

      if (node.rotation.size() == 4) {
        mat_view = mat_view*kmuvcl::math::quat2mat(
              node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]).transpose();
      }

      if (node.translation.size() == 3) {
        mat_view = mat_view*kmuvcl::math::translate<float>(
              -node.translation[0], -node.translation[1], -node.translation[2]);
      }      
    }
  }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  // Q를 누를 때마다 0번과 1번 카메라를 바꿔주도록.
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		camera_index = camera_index == 0 ? 1 : 0;
    std::cout << "camera_index : " << camera_index << std::endl;
	}
}

// object rendering: 현재 scene은 삼각형 하나로 구성되어 있음.
void render_object()
{
  // 특정 쉐이더 프로그램 사용
  glUseProgram(program);
  
  const std::vector<tinygltf::Node>& nodes = model.nodes;
  const std::vector<tinygltf::Mesh>& meshes = model.meshes;
  const std::vector<tinygltf::Accessor>& accessors = model.accessors;
  const std::vector<tinygltf::BufferView>& bufferViews = model.bufferViews;
  
  for (const tinygltf::Node& node : nodes)
  {
    // camera에 대한 transform도 있었듯이, model mesh에 대한 transform도 있다.
    // mat_model 설정.
    if (node.mesh > -1)
    {
      mat_model.set_to_identity();
      
      if (node.translation.size() == 3) {
          mat_model = mat_model*kmuvcl::math::translate<float>(
              node.translation[0], node.translation[1], node.translation[2]);
      }

      if (node.rotation.size() == 4) {
          mat_model = mat_model*kmuvcl::math::quat2mat(
              node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]);
      }

      if (node.scale.size() == 3) {
          mat_model = mat_model*kmuvcl::math::scale<float>(
              node.scale[0], node.scale[1], node.scale[2]);
      }

      mat_PVM = mat_proj * mat_view * mat_model;
      glUniformMatrix4fv(loc_u_PVM, 1, GL_FALSE, mat_PVM);

      const tinygltf::Mesh& mesh = meshes[node.mesh];

      for (const tinygltf::Primitive& primitive : mesh.primitives)
      {
          for (const auto& attrib : primitive.attributes)
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
          }

          // primitive가 DrawElement를 쓴다면 index 정보가 있다. 
          // index에 해당하는 accessor를 받아와서 bufferview를 받아옴. (target이 필요해서)
          const tinygltf::Accessor& index_accessor = accessors[primitive.indices];
          const tinygltf::BufferView& bufferView = bufferViews[index_accessor.bufferView];
          
          glBindBuffer(bufferView.target, index_buffer);

          // primitive.mode : Triangle
          // index_accessor.count : 6  // 점은 4개지만 그리는 데 참여하는 것은 6개.
          glDrawElements(primitive.mode,
              index_accessor.count,
              index_accessor.componentType,
              BUFFER_OFFSET(index_accessor.byteOffset));
          
          // 정점 attribute 배열 비활성화
          glDisableVertexAttribArray(loc_a_position);
      }
    }
  }

  // 쉐이더 프로그램 사용해제
  glUseProgram(0);
}

int main(void)
{
    GLFWwindow* window;

    // Initialize GLFW library
    if (!glfwInit())
        return -1;

    // Create a GLFW window containing a OpenGL context
    window = glfwCreateWindow(500, 500, "Hello Cameras with glTF 2.0", NULL, NULL);
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

    init_shader_program();

    load_model(model, "Cameras.gltf");

    // GPU의 VBO를 초기화하는 함수 호출
    init_buffer_objects();

    glfwSetKeyCallback(window, key_callback);
	
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        set_transform();
        render_object();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}