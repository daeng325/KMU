///// main.cpp
///// OpenGL 3+, GLSL 1.20, GLEW, GLFW3

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <cassert>
#include <chrono>
#include "../common/vec.hpp"
#include "../common/transform.hpp"
#include "Camera.h"


// 이미지 로딩을 위해, stb 라이브러리를 이용함 (https://github.com/nothings/stb)
// 아래 두 줄을 include 하여 사용.
#define STB_IMAGE_IMPLEMENTATION
#include "../glTF/stb_image.h"

////////////////////////////////////////////////////////////////////////////////
/// 쉐이더 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLuint  program;              // 쉐이더 프로그램 객체의 레퍼런스 값

GLint   loc_a_position;       // attribute 변수 a_position 위치
GLint   loc_a_texcoord;       // attribute 변수 loc_a_texcoord 위치
GLint   loc_u_PVM;            // uniform 변수 u_PVM 위치
GLint   loc_u_texture_cats;   // uniform 변수 loc_u_texid_cats 위치
GLint   loc_u_texture_frame;  // uniform 변수 loc_u_texid_frame 위치

GLuint  texid_cats;           // GPU 메모리에서 texid_cats 위치
GLuint  texid_frame;          // GPU 메모리에서 texid_frame 위치

GLuint  position_buffer;      // GPU 메모리에서 position_buffer의 위치
GLuint  texcoord_buffer;      // GPU 메모리에서 texcoord_bufferd의 위치

void init();
GLuint create_shader_from_file(const std::string& filename, GLuint shader_type);
void init_shader_program();
void init_buffer_objects();
void init_texture_objects();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 변환 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
kmuvcl::math::mat4x4f     mat_model, mat_view, mat_proj;
kmuvcl::math::mat4x4f     mat_PVM;

float   g_angle = 0.0;
bool    g_is_animation = false;
std::chrono::time_point<std::chrono::system_clock> prev, curr;

void set_transform();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 렌더링 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
// per-vertex 3D positions (x, y, z)
GLfloat g_position[] = {
  // front
  -1.0f,  1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f,  1.0f,
  1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,
  // back
  1.0f,  1.0f, -1.0f,   1.0f, -1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,   1.0f,  1.0f, -1.0f,
  // top
  -1.0f,  1.0f, -1.0f,  -1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
  1.0f,  1.0f,  1.0f,   1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
  // bottom
  -1.0f, -1.0f,  1.0f,  -1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
  1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
  // right
  1.0f,  1.0f,  1.0f,   1.0f, -1.0f,  1.0f,   1.0f, -1.0f, -1.0f,
  1.0f, -1.0f, -1.0f,   1.0f,  1.0f, -1.0f,   1.0f,  1.0f,  1.0f,
  // left
  -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,
  -1.0f, -1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f,  1.0f, -1.0f,
};

// per-vertex texcoord (u,v)
GLfloat g_texcoord[] = {
  // front
  0.0f, 1.0f,   0.0f, 0.0f,   1.0f, 0.0f,
  1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f,
  // back
  0.0f, 1.0f,   0.0f, 0.0f,   1.0f, 0.0f,
  1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f,
  // top
  0.0f, 1.0f,   0.0f, 0.0f,   1.0f, 0.0f,
  1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f,
  // bottom
  0.0f, 1.0f,   0.0f, 0.0f,   1.0f, 0.0f,
  1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f,
  // right
  0.0f, 1.0f,   0.0f, 0.0f,   1.0f, 0.0f,
  1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f,
  // left
  0.0f, 1.0f,   0.0f, 0.0f,   1.0f, 0.0f,
  1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 1.0f,
};

void render_object();           // rendering 함수: 물체(삼각형)를 렌더링하는 함수.
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/// 카메라 및 뷰포트 관련 변수
////////////////////////////////////////////////////////////////////////////////
Camera  camera;
float   g_aspect = 1.0f;
////////////////////////////////////////////////////////////////////////////////

void init()
{
  glEnable(GL_DEPTH_TEST);

  camera.set_near(0.1f);
  camera.set_far(100.0f);

  prev = curr = std::chrono::system_clock::now();
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

  loc_a_position = glGetAttribLocation(program, "a_position");
  loc_a_texcoord = glGetAttribLocation(program, "a_texcoord");

  // TODO: loc_u_texture_cats, loc_u_texture_frame 세팅
  loc_u_texture_cats = glGetUniformLocation(program, "u_texture_cats");
  loc_u_texture_frame = glGetUniformLocation(program, "u_texture_frame");
}


void init_texture_objects()
{
  int width, height, channels;
  unsigned char* image;


  // 향후, 원점 위치를 좌*상*단에서 좌*하*단으로 이동시킨 효과가 나도록 영상을 로딩함.
  stbi_set_flip_vertically_on_load(true);

  // 파일로부터 영상을 이루는 픽셀들을 메인메모리로 로딩
  image = stbi_load("00_cats.jpg", &width, &height, &channels, STBI_rgb);

  // TODO: "00_cats.jpg"에 해당하는 텍스쳐 생성, 전송 및 텍스쳐-파라메터 세팅
  glGenTextures(1, &texid_cats);
  glBindTexture(GL_TEXTURE_2D, texid_cats);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


  // 메인메모리로 로딩된 영상데이터 메모리 해제
  stbi_image_free(image);


  // 파일로부터 영상을 이루는 픽셀들을 메인메모리로 로딩
  image = stbi_load("01_frame.jpg", &width, &height, &channels, STBI_rgb);  

  // TODO: "01_frame.jpg"에 해당하는 텍스쳐 생성, 전송 및 텍스쳐-파라메터 세팅
  glGenTextures(1, &texid_frame);
  glBindTexture(GL_TEXTURE_2D, texid_frame);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


  // 메인메모리로 로딩된 영상데이터 메모리 해제
  stbi_image_free(image);
}



void init_buffer_objects()
{
  glGenBuffers(1, &position_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_position), g_position, GL_STATIC_DRAW);

  // TODO: texcoord_buffer 생성 및 데이터 세팅
  glGenBuffers(1, &texcoord_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, texcoord_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_texcoord), g_texcoord, GL_STATIC_DRAW);
}

void set_transform()
{
  // set camera transformation
  kmuvcl::math::vec3f eye = camera.position();
  kmuvcl::math::vec3f up = camera.up_direction();
  kmuvcl::math::vec3f center = eye + camera.front_direction();

  // std::cout << "eye:    " << eye << std::endl;
  // std::cout << "up:     " << up << std::endl;
  // std::cout << "center: " << center << std::endl;

  mat_view = kmuvcl::math::lookAt(eye[0], eye[1], eye[2],
    center[0], center[1], center[2],
    up[0], up[1], up[2]);

  float n = camera.near();
  float f = camera.far();

  if (camera.mode() == Camera::kOrtho)
  {
    float l = camera.left();
    float r = camera.right();
    float b = camera.bottom();
    float t = camera.top();

    //std::cout << "(camera.mode() == Camera::kOrtho)" << std::endl;
    //std::cout << "(l, r, b, t, n, f): " << l << ", " << r << ", " << b << ", " << t << ", " << n << ", " << f << std::endl;

    mat_proj = kmuvcl::math::ortho(l, r, b, t, n, f);
  }
  else if (camera.mode() == Camera::kPerspective)
  {
    //std::cout << "(camera.mode() == Camera::kPerspective)" << std::endl;
    //std::cout << "(fov, aspect, n, f): " << camera.fovy() << ", " << g_aspect << ", " << n << ", " << f << std::endl;

    mat_proj = kmuvcl::math::perspective(camera.fovy(), g_aspect, n, f);
  }

  // set object transformation
  curr = std::chrono::system_clock::now();
  std::chrono::duration<float> elaped_seconds = (curr - prev);
  prev = curr;

  if (g_is_animation)
  {
    g_angle += 30.0f * elaped_seconds.count();
    if (g_angle > 25200.0f)
    {
      g_angle = 0.0f;
    }
  }


  mat_model = kmuvcl::math::rotate(g_angle*0.7f, 0.0f, 0.0f, 1.0f);
  mat_model = kmuvcl::math::rotate(g_angle*1.0f, 0.0f, 1.0f, 0.0f)*mat_model;
  mat_model = kmuvcl::math::rotate(g_angle*0.5f, 1.0f, 0.0f, 0.0f)*mat_model;
  mat_model = kmuvcl::math::translate(0.0f, 0.0f, -4.0f)*mat_model;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_P && action == GLFW_PRESS)
  {
    g_is_animation = !g_is_animation;
    std::cout << (g_is_animation ? "animation" : "no animation") << std::endl;
  }

  if (key == GLFW_KEY_A && action == GLFW_PRESS)
    camera.move_left(0.1f);
  if (key == GLFW_KEY_D && action == GLFW_PRESS)
    camera.move_right(0.1f);
  if (key == GLFW_KEY_W && action == GLFW_PRESS)
    camera.move_forward(0.1f);
  if (key == GLFW_KEY_S && action == GLFW_PRESS)
    camera.move_backward(0.1f);

  if (key == GLFW_KEY_Q && action == GLFW_PRESS)
  {
    camera.set_mode(camera.mode() == Camera::kOrtho ? Camera::kPerspective : Camera::kOrtho);
  }
}

void frambuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);

  g_aspect = (float)width / (float)height;
}


// object rendering: 현재 scene은 삼각형 하나로 구성되어 있음.
void render_object()
{
  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // 특정 쉐이더 프로그램 사용
  glUseProgram(program);

  // Setting Proj * View * Model
  mat_PVM = mat_proj * mat_view * mat_model;
  glUniformMatrix4fv(loc_u_PVM, 1, GL_FALSE, mat_PVM);

  // TODO: 쉐이더 텍스쳐 샘플러 세팅, 텍스쳐 활성화 및 바인딩
  glUniform1i(loc_u_texture_cats, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texid_cats);

  glUniform1i(loc_u_texture_frame, 1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, texid_frame);


  // 앞으로 언급하는 배열 버퍼(GL_ARRAY_BUFFER)는 position_buffer로 지정
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
  // 버텍스 쉐이더의 attribute 중 a_position 부분 활성화
  glEnableVertexAttribArray(loc_a_position);
  // 현재 배열 버퍼에 있는 데이터를 버텍스 쉐이더 a_position에 해당하는 attribute와 연결
  glVertexAttribPointer(loc_a_position, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


  // TODO: texcoord_buffer에 있는 데이터를 a_texcoord에 해당하는 attribute와 연결
  glBindBuffer(GL_ARRAY_BUFFER, texcoord_buffer);
  glEnableVertexAttribArray(loc_a_texcoord);
  glVertexAttribPointer(loc_a_texcoord, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // 삼각형 그리기
  glDrawArrays(GL_TRIANGLES, 0, 36);

  // 정점 attribute 배열 비활성화
  glDisableVertexAttribArray(loc_a_position);
  glDisableVertexAttribArray(loc_a_texcoord);
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
  window = glfwCreateWindow(500, 500, "Texture Mapping", NULL, NULL);
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

  init();
  init_shader_program();
  init_buffer_objects();
  init_texture_objects();

  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, frambuffer_size_callback);

  // Loop until the user closes the window
  while (!glfwWindowShouldClose(window))
  {
    // Poll for and process events
    glfwPollEvents();

    set_transform();
    render_object();

    // Swap front and back buffers
    glfwSwapBuffers(window);
  }

  glfwTerminate();

  return 0;
}
