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

#define USE_BUNNY
#ifdef USE_BUNNY
#include "Bunny.hpp"
#else
#include "Teapot.hpp"
#endif

////////////////////////////////////////////////////////////////////////////////
/// 쉐이더 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLuint  program;          // 쉐이더 프로그램 객체의 레퍼런스 값
GLint   loc_a_position;   // attribute 변수 a_position 위치
GLint   loc_a_normal;      // attribute 변수 a_normal 위치
GLint   loc_u_PVM;        // uniform 변수 u_PVM 위치
GLint   loc_u_M;          // uniform 변수 u_M 위치


GLint   loc_u_view_position_wc;       // uniform 변수 u_view_position_wc 위치

GLint   loc_u_light_position_wc;      // uniform 변수 u_light_position_wc 위치 
GLint   loc_u_light_ambient;          // uniform 변수 u_light_ambient 위치
GLint   loc_u_light_diffuse;          // uniform 변수 u_light_diffuse 위치
GLint   loc_u_light_specular;         // uniform 변수 u_light_specular 위치

GLint   loc_u_material_ambient;       // uniform 변수 u_material_ambient 위치
GLint   loc_u_material_diffuse;       // uniform 변수 u_material_diffuse 위치
GLint   loc_u_material_specular;      // uniform 변수 u_material_specular 위치
GLint   loc_u_material_shininess;     // uniform 변수 u_material_shininess 위치



GLuint  position_buffer;  // GPU 메모리에서 position_buffer의 위치
GLuint  normal_buffer;     // GPU 메모리에서 normal_buffer의 위치

GLuint create_shader_from_file(const std::string& filename, GLuint shader_type);
void init_shader_program();
void init_buffer_objects();
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
/// 렌더링 관련 함수
////////////////////////////////////////////////////////////////////////////////
void render_object();           // rendering 함수: 물체를 렌더링하는 함수.
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
    glGetShaderInfoLog(shader, buf_len, 0, (GLchar *) log_string.c_str());

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
    glGetProgramInfoLog(program, buf_len, 0, (GLchar *) log_string.c_str());

    std::cout << "error_log: " << log_string << std::endl;

    glDeleteProgram(program);
    program = 0;
  }

  std::cout << "program id: " << program << std::endl;
  assert(program != 0);

  loc_u_PVM = glGetUniformLocation(program, "u_PVM");  
  loc_u_M = glGetUniformLocation(program, "u_M");


  loc_u_view_position_wc = glGetUniformLocation(program, "u_view_position_wc");

  loc_u_light_position_wc = glGetUniformLocation(program, "u_light_position_wc");
  loc_u_light_ambient = glGetUniformLocation(program, "u_light_ambient");
  loc_u_light_diffuse = glGetUniformLocation(program, "u_light_diffuse");
  loc_u_light_specular = glGetUniformLocation(program, "u_light_specular");

  loc_u_material_ambient = glGetUniformLocation(program, "u_material_ambient");
  loc_u_material_diffuse = glGetUniformLocation(program, "u_material_diffuse");
  loc_u_material_specular = glGetUniformLocation(program, "u_material_specular");
  loc_u_material_shininess = glGetUniformLocation(program, "u_material_shininess");


  loc_a_position = glGetAttribLocation(program, "a_position");
  loc_a_normal = glGetAttribLocation(program, "a_normal");
}

void init_buffer_objects()
{
  glGenBuffers(1, &position_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
#ifdef USE_BUNNY
  glBufferData(GL_ARRAY_BUFFER, sizeof(bunny::position), bunny::position, GL_STATIC_DRAW);
#else
  glBufferData(GL_ARRAY_BUFFER, sizeof(teapot::position), teapot::position, GL_STATIC_DRAW);
#endif

  glGenBuffers(1, &normal_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
#ifdef USE_BUNNY
  glBufferData(GL_ARRAY_BUFFER, sizeof(bunny::normal), bunny::normal, GL_STATIC_DRAW);
#else
  glBufferData(GL_ARRAY_BUFFER, sizeof(teapot::normal), teapot::normal, GL_STATIC_DRAW);
#endif
}

void set_transform()
{
  // set camera transformation
  kmuvcl::math::vec3f eye = camera.position();
  kmuvcl::math::vec3f up = camera.up_direction();
  kmuvcl::math::vec3f center = eye + camera.front_direction();

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

    mat_proj = kmuvcl::math::ortho(l, r, b, t, n, f);
  }
  else if (camera.mode() == Camera::kPerspective)
  {
    mat_proj = kmuvcl::math::perspective(camera.fovy(), g_aspect, n, f);
  }

  // set object transformation
  mat_model = kmuvcl::math::rotate(g_angle*1.0f, 0.0f, 1.0f, 0.0f);
  mat_model = kmuvcl::math::translate(0.0f, 0.0f, -1.0f) * mat_model;
}


void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
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

void frambuffer_size_callback(GLFWwindow * window, int width, int height)
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

  glUniformMatrix4fv(loc_u_M, 1, GL_FALSE, mat_model);

  glUniform3fv(loc_u_view_position_wc, 1, camera.position());

  glUniform3f(loc_u_light_position_wc, 3.0f, 5.0f, 10.0f);
  glUniform4f(loc_u_light_ambient, 1.0f, 1.0f, 1.0f, 1.0f);
  glUniform4f(loc_u_light_diffuse, 1.0f, 1.0f, 1.0f, 1.0f);
  glUniform4f(loc_u_light_specular, 1.0f, 1.0f, 1.0f, 1.0f);

  glUniform4f(loc_u_material_ambient, 0.0f, 0.0f, 0.0f, 1.0f);
  glUniform4f(loc_u_material_diffuse, 1.0f, 0.0f, 0.0f, 1.0f);
  glUniform4f(loc_u_material_specular, 1.0f, 1.0f, 1.0f, 1.0f);
  glUniform1f(loc_u_material_shininess, 100.0f);


  // 앞으로 언급하는 배열 버퍼(GL_ARRAY_BUFFER)는 position_buffer로 지정
  glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
  // 버텍스 쉐이더의 attribute 중 a_position 부분 활성화
  glEnableVertexAttribArray(loc_a_position);
  // 현재 배열 버퍼에 있는 데이터를 버텍스 쉐이더 a_position에 해당하는 attribute와 연결
  glVertexAttribPointer(loc_a_position, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  // 앞으로 언급하는 배열 버퍼(GL_ARRAY_BUFFER)는 normal_buffer로 지정
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
  // 버텍스 쉐이더의 attribute 중 a_normal 부분 활성화
  glEnableVertexAttribArray(loc_a_normal);
  // 현재 배열 버퍼에 있는 데이터를 버텍스 쉐이더 a_normal에 해당하는 attribute와 연결
  glVertexAttribPointer(loc_a_normal, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

#ifdef USE_BUNNY
  // 스탠포드 토끼 그리기
  glDrawArrays(GL_TRIANGLES, 0, bunny::num_position);
#else
  // 유타 찻주전자 그리기
  glDrawArrays(GL_TRIANGLES, 0, teapot::num_position);
#endif

  // 정점 attribute 배열 비활성화
  glDisableVertexAttribArray(loc_a_position);
  glDisableVertexAttribArray(loc_a_normal);
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
  window = glfwCreateWindow(500, 500, "Hello Triangle", NULL, NULL);
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

  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, frambuffer_size_callback);

  prev = curr = std::chrono::system_clock::now();

  // Loop until the user closes the window
  while (!glfwWindowShouldClose(window))
  {
    // Poll for and process events
    glfwPollEvents();

    set_transform();
    render_object();

    curr = std::chrono::system_clock::now();
    std::chrono::duration<float> elaped_seconds = (curr - prev);
    prev = curr;

    if (g_is_animation)
    {
      g_angle += 30.0f * elaped_seconds.count();
      if (g_angle > 360.0f)
        g_angle = 0.0f;
    }

    // Swap front and back buffers
    glfwSwapBuffers(window);
  }

  glfwTerminate();

  return 0;
}
