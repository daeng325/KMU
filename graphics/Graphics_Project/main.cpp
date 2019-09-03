#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <cassert>
#include <chrono>

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
    const float MATH_PI = 3.14159265358979323846f;

    template <typename T>
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
/// OpenGL state 초기화 관련 함수
////////////////////////////////////////////////////////////////////////////////
void init_state();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 쉐이더 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLuint  program;          // 쉐이더 프로그램 객체의 레퍼런스 값
GLint   loc_a_position;
GLint   loc_a_normal;
GLint   loc_a_texcoord;
GLint   loc_a_color;

GLint   loc_u_PVM;
GLint   loc_u_M;

GLint   loc_u_view_position_wc;
GLint   loc_u_light_position_wc;

GLint   loc_u_light_ambient;
GLint   loc_u_light_diffuse;
GLint   loc_u_light_specular;

GLint   loc_u_material_ambient;
GLint   loc_u_material_specular;
GLint   loc_u_material_shininess;

GLint   loc_u_diffuse_texture;
GLint   loc_u_color;

//shader_flag 0은 color, 1은 texture 정보가 있으면 true이다. 거기에 따라서 shader구성이 변한다.
bool shader_flag[10]={false,};

std::string vertex_init="#version 120// GLSL 1.20\nuniform mat4 u_PVM;\nattribute vec3 a_position;\nuniform mat4 u_M;\nattribute vec2 a_texcoord;\nvarying vec3 v_normal_wc;\nvarying vec3 v_position_wc;\n";
std::string yes_normal_VI="attribute vec3 a_normal;\n";
std::string vertex_code="void main(){\n\tgl_Position=u_PVM*vec4(a_position,1.0f);\n\tv_position_wc = (u_M * vec4(a_position, 1)).xyz;\n";
std::string no_normal_VC="\tv_normal_wc=normalize((u_M * vec4(1.0f,1.0f,1.0f,0.0f)).xyz);\n";
std::string yes_normal_VC="\tv_normal_wc=normalize((u_M * vec4(a_normal, 0)).xyz);\n";

std::string frag_init="#version 120// GLSL 1.20\nvarying vec3 v_normal_wc;\nvarying vec3 v_position_wc;\nuniform vec4 u_material_ambient;\nuniform vec4 u_material_specular;\nuniform float u_material_shininess;\nuniform vec3 u_view_position_wc;\nuniform vec3 u_light_position_wc;\nuniform vec4 u_light_ambient;\nuniform vec4 u_light_diffuse;\nuniform vec4 u_light_specular;\n";
std::string no_texture_FFI="\tvec4 material_diffuse = u_diffuse_texture;\n";
std::string yes_texture_FFI="\tvec4 material_diffuse = texture2D(u_diffuse_texture, v_texcoord);\n";
std::string frag_init_first="vec4 calc_color()\n{\n\tvec4 color = vec4(0.0);\n\tvec3 n_wc = normalize(v_normal_wc);\n\tvec3 l_wc = normalize(u_light_position_wc - v_position_wc);\n\tvec3 r_wc = reflect(-l_wc, n_wc);\n\tvec3 v_wc = u_view_position_wc;\n\tcolor += (u_light_ambient * u_material_ambient);\n";
std::string frag_init_last="\tfloat ndotl = max(0.0, dot(n_wc, l_wc));\n\tcolor += (ndotl * u_light_diffuse * material_diffuse);\n\tfloat rdotv = max(0.0, dot(r_wc, v_wc) );\n\tcolor += (pow(rdotv, u_material_shininess) * u_light_specular * u_material_specular);\n\treturn color;\n}\n";
std::string frag_code="void main()\n{\n\tvec4 tmp_color=calc_color();\n";
std::string no_color_FC="\ttmp_color+=vec4(1.0f, 0.0f, 0.0f, 1.0f);\n";

std::string no_texture_FI="uniform vec4 u_diffuse_texture;\n";
std::string yes_texture_FI="uniform sampler2D u_diffuse_texture;\nvarying vec2 v_texcoord;\n";
std::string color_VI = "attribute vec3 a_color;\nvarying vec3 v_color;\n";
std::string color_VC = "\tv_color = a_color;\n";
std::string color_FI = "varying vec3 v_color;\n";
std::string color_FC = "\ttmp_color += vec4(v_color,1.0f);\n";
std::string texture_VI = "varying vec2 v_texcoord;\n";
std::string texture_VC = "\tv_texcoord = a_texcoord;\n";

std::string factor_FI = "uniform vec4 u_color;\n";
std::string factor_FC = "\ttmp_color += u_color;\n";
std::string texcrood_factor_FC = "\ttmp_color += vec4(tmp_color[0]*u_color[0],tmp_color[1]*u_color[1],tmp_color[2]*u_color[2],tmp_color[3]*u_color[3]);\n";


void init_shader_code(const std::string& code, const std::string& filename);
void init_code();
GLuint create_shader_from_file(const std::string& filename, GLuint shader_type);
void init_shader_program();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 변환 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
kmuvcl::math::mat4x4f   mat_view, mat_proj;
kmuvcl::math::mat4x4f   mat_PVM;

float   g_angle = 0.0;
bool    g_is_animation = false;
std::chrono::time_point<std::chrono::system_clock> prev, curr;

void set_transform();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// GLFW 콜백 함수
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/// 렌더링 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
tinygltf::Model model;

GLuint position_buffer;
GLuint color_buffer;
GLuint normal_buffer;
GLuint texcoord_buffer;
GLuint index_buffer;

GLuint diffuse_texid;

kmuvcl::math::vec3f view_position_wc;

kmuvcl::math::vec3f light_position_wc = kmuvcl::math::vec3f(0.0f, 5.0f, 10.0f);

kmuvcl::math::vec4f light_ambient = kmuvcl::math::vec4f(0.02f, 0.0f, 0.01f, 1.0f);
kmuvcl::math::vec4f light_diffuse = kmuvcl::math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);
kmuvcl::math::vec4f light_specular = kmuvcl::math::vec4f(0.07f, 0.07f, 0.07f, 1.0f);

kmuvcl::math::vec4f material_ambient = kmuvcl::math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);
kmuvcl::math::vec4f material_specular = kmuvcl::math::vec4f(0.3f, 0.3f, 0.3f, 1.0f);
kmuvcl::math::vec4f diffuse_texture = kmuvcl::math::vec4f(0.3f, 0.3f, 0.3f, 1.0f);

float               material_shininess = 1.0f;

kmuvcl::math::vec4f color_tmp;

bool load_model(tinygltf::Model &model, const std::string filename);
void init_buffer_objects();     // VBO init 함수: GPU의 VBO를 초기화하는 함수.
void init_texture_objects();

void draw_scene();
void draw_node(const tinygltf::Node& node, kmuvcl::math::mat4f mat_view);
void draw_mesh(const tinygltf::Mesh& mesh, const kmuvcl::math::mat4f& mat_model);
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 플래그, 인덱스 변수
////////////////////////////////////////////////////////////////////////////////
int camera_index = 0;
float m_translate_x=0, m_translate_y=0, m_translate_z=0;
float c_translate_x=0, c_translate_y=0, c_translate_z=0;
////////////////////////////////////////////////////////////////////////////////

void init_state()
{
  glEnable(GL_DEPTH_TEST);
  
  prev = curr = std::chrono::system_clock::now();
}

void init_code(){
	vertex_init += shader_flag[0] ? color_VI : "";
	vertex_init += shader_flag[1] ? texture_VI : "";
	vertex_init += shader_flag[3] ? yes_normal_VI : "";
	
	vertex_code += shader_flag[3] ? yes_normal_VC : no_normal_VC;
	vertex_code += shader_flag[1] ? texture_VC : "";
	vertex_code += shader_flag[0] ? color_VC : "";
	
	frag_init += shader_flag[1] ? yes_texture_FI : no_texture_FI;
	frag_init += shader_flag[0] ? color_FI : "";
	frag_init += shader_flag[2] ? factor_FI : "";
	frag_init += frag_init_first;
	frag_init += shader_flag[1] ? yes_texture_FFI : no_texture_FFI;
	frag_init += frag_init_last;
	
	if(!shader_flag[0] && !shader_flag[1] && !shader_flag[2])
		frag_code += no_color_FC;
	if(shader_flag[0])
		frag_code += color_FC;
	if(shader_flag[1] && shader_flag[2])
		frag_code += texcrood_factor_FC;
	else if(shader_flag[2])
		frag_code += factor_FC;
	frag_code += "\tgl_FragColor = tmp_color;\n";
  vertex_code+="}";
  frag_code+="}";
}

void init_shader_code(const std::string& code, const std::string& filename){
  std::ofstream out(filename);
  if (out.is_open()) {
    out << code;
  } else {
    out << "파일을 찾을 수 없습니다!" << std::endl;
  }
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
    = create_shader_from_file("./shader/vertex1.glsl", GL_VERTEX_SHADER);

  std::cout << "vertex_shader id: " << vertex_shader << std::endl;
  assert(vertex_shader != 0);

  GLuint fragment_shader
    = create_shader_from_file("./shader/fragment1.glsl", GL_FRAGMENT_SHADER);

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
  loc_u_M = glGetUniformLocation(program, "u_M");
  loc_a_position = glGetAttribLocation(program, "a_position");

  loc_u_view_position_wc = glGetUniformLocation(program, "u_view_position_wc");
  loc_u_light_position_wc = glGetUniformLocation(program, "u_light_position_wc");
  loc_u_light_ambient = glGetUniformLocation(program, "u_light_ambient");
  loc_u_light_diffuse = glGetUniformLocation(program, "u_light_diffuse");
  loc_u_light_specular = glGetUniformLocation(program, "u_light_specular");
  loc_u_material_ambient = glGetUniformLocation(program, "u_material_ambient");
  loc_u_material_specular = glGetUniformLocation(program, "u_material_specular");
  loc_u_material_shininess = glGetUniformLocation(program, "u_material_shininess");
  loc_u_diffuse_texture = glGetUniformLocation(program, "u_diffuse_texture");
  
  if(shader_flag[0])
    loc_a_color = glGetAttribLocation(program, "a_color");
  if(shader_flag[1])
  	loc_a_texcoord = glGetAttribLocation(program, "a_texcoord");
  if(shader_flag[2])
    loc_u_color = glGetUniformLocation(program, "u_color");
  if(shader_flag[3])
	  loc_a_normal = glGetAttribLocation(program, "a_normal");
  	
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
  const std::vector<tinygltf::Material>& materials = model.materials;
  const std::vector<tinygltf::Accessor>& accessors = model.accessors;
  const std::vector<tinygltf::BufferView>& bufferViews = model.bufferViews;
  const std::vector<tinygltf::Buffer>& buffers = model.buffers;

  for (const tinygltf::Mesh& mesh : meshes)
  {
    for (const tinygltf::Primitive& primitive : mesh.primitives)
    {
      if(primitive.indices!=-1)
      {
        const tinygltf::Accessor& accessor = accessors[primitive.indices];
        const tinygltf::BufferView& bufferView = bufferViews[accessor.bufferView];
        const tinygltf::Buffer& buffer = buffers[bufferView.buffer];
        glGenBuffers(1, &index_buffer);
        glBindBuffer(bufferView.target, index_buffer);
        glBufferData(bufferView.target, bufferView.byteLength,
          &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
      }
      if (primitive.material > -1)
      {
        const tinygltf::Material& material = materials[primitive.material];
        for (const std::pair<std::string, tinygltf::Parameter> parameter : material.values)
        {
          if(parameter.first.compare("baseColorFactor")==0)
          {
            shader_flag[2]=true;
          }
        }
      }

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
        else if (attrib.first.compare("NORMAL") == 0)
        {
        	shader_flag[3]=true;
          glGenBuffers(1, &normal_buffer);
          glBindBuffer(bufferView.target, normal_buffer);
          glBufferData(bufferView.target, bufferView.byteLength,
            &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
        }
        else if (attrib.first.compare("TEXCOORD_0") == 0)
        {
          shader_flag[1]=true;
          glGenBuffers(1, &texcoord_buffer);
          glBindBuffer(bufferView.target, texcoord_buffer);
          glBufferData(bufferView.target, bufferView.byteLength,
            &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
        }
        else if (attrib.first.compare("COLOR_0") == 0)
        {
          shader_flag[0]=true;
          glGenBuffers(1, &color_buffer);
          glBindBuffer(bufferView.target, color_buffer);
          glBufferData(bufferView.target, bufferView.byteLength,
            &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
        }
      }
    }
  }
}

void init_texture_objects()
{
  const std::vector<tinygltf::Texture>& textures = model.textures;
  if(textures.empty()) return;
  const std::vector<tinygltf::Image>& images = model.images;
  const std::vector<tinygltf::Sampler>& samplers = model.samplers;

  for (const tinygltf::Texture& texture : textures)
  {
    glGenTextures(1, &diffuse_texid);
    glBindTexture(GL_TEXTURE_2D, diffuse_texid);

    const tinygltf::Image& image = images[texture.source];
    const tinygltf::Sampler& sampler = samplers[texture.sampler];

    GLenum format = GL_RGBA;
    if (image.component == 1) {
      format = GL_RED;
    }
    else if (image.component == 2) {
      format = GL_RG;
    }
    else if (image.component == 3) {
      format = GL_RGB;
    }

    GLenum type = GL_UNSIGNED_BYTE;
    if (image.bits == 16) {
      type = GL_UNSIGNED_SHORT;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
      image.width, image.height, 0, format, type, &image.image[0]);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler.minFilter);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler.magFilter);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler.wrapS);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler.wrapT);

    glGenerateMipmap(GL_TEXTURE_2D);
  }
}

void set_transform()
{
  mat_view.set_to_identity();
  //mat_proj.set_to_identity();

  float fovy = 70.0f;
  float aspectRatio = 1.0f;
  float znear = 0.01f;
  float zfar = 100.0f;
  bool proj_flag = false;
  bool view_flag = false;

  const std::vector<tinygltf::Node>& nodes = model.nodes;
  const std::vector<tinygltf::Camera>& cameras = model.cameras;
  if(cameras.size()>camera_index){
    const tinygltf::Camera& camera = cameras[camera_index];
    if (camera.type.compare("perspective") == 0)
    {
      proj_flag = true;
      fovy = kmuvcl::math::rad2deg(camera.perspective.yfov);
      aspectRatio = camera.perspective.aspectRatio;
      znear = camera.perspective.znear;
      zfar = camera.perspective.zfar;

      /*std::cout << "(camera.mode() == Camera::kPerspective)" << std::endl;
      std::cout << "(fovy, aspect, n, f): " << fovy << ", " << aspectRatio << ", " << znear << ", " << zfar << std::endl;*/
      mat_proj = kmuvcl::math::perspective(fovy, aspectRatio, znear, zfar);
    }
    else if(camera.type.compare("orthographic") == 0)
    {
      proj_flag = true;
      float xmag = camera.orthographic.xmag;
      float ymag = camera.orthographic.ymag;
      float znear = camera.orthographic.znear;
      float zfar = camera.orthographic.zfar;

      /*std::cout << "(camera.mode() == Camera::kOrtho)" << std::endl;
      std::cout << "(xmag, ymag, n, f): " << xmag << ", " << ymag << ", " << znear << ", " << zfar << std::endl;*/
      mat_proj = kmuvcl::math::ortho(-xmag, xmag, -ymag, ymag, znear, zfar);
    }

    for (const tinygltf::Node& node : nodes)
    {
      if (node.camera == camera_index)
      {
        mat_view.set_to_identity();
        
        if (node.scale.size() == 3) {
          view_flag = true;
          mat_view = mat_view*kmuvcl::math::scale<float>(
                1.0f / node.scale[0], 1.0f / node.scale[1], 1.0f / node.scale[2]);
        }

        if (node.rotation.size() == 4) {
          view_flag = true;
          mat_view = mat_view*kmuvcl::math::quat2mat(
                node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]).transpose();
        }

        if (node.translation.size() == 3) {
          view_flag = true;
          mat_view = mat_view*kmuvcl::math::translate<float>(
                -node.translation[0]-c_translate_x, -node.translation[1]-c_translate_y, -node.translation[2]-c_translate_z);
        }      
      }
    }
  }
  if(!proj_flag) mat_proj = kmuvcl::math::perspective(fovy, aspectRatio, znear, zfar);
  if(!view_flag) mat_view = kmuvcl::math::translate(-c_translate_x, -c_translate_y, -c_translate_z-2.0f);
}


void draw_node(const tinygltf::Node& node, kmuvcl::math::mat4f mat_model)
{
  const std::vector<tinygltf::Node>& nodes = model.nodes;
  const std::vector<tinygltf::Mesh>& meshes = model.meshes;

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

  if (node.matrix.size() == 16)
  {
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

    mat_model = mat_model * mat_node;
  }

  if (node.mesh > -1)
  {
    draw_mesh(meshes[node.mesh], mat_model);
  }

  for (size_t i = 0; i < node.children.size(); ++i)
  {
    draw_node(nodes[node.children[i]], mat_model);
  }
}

void draw_mesh(const tinygltf::Mesh& mesh, const kmuvcl::math::mat4f& mat_model)
{
  const std::vector<tinygltf::Material>& materials = model.materials;
  const std::vector<tinygltf::Texture>& textures = model.textures;
  const std::vector<tinygltf::Accessor>& accessors = model.accessors;
  const std::vector<tinygltf::BufferView>& bufferViews = model.bufferViews;
  const std::vector<tinygltf::Buffer>& buffers = model.buffers;

  glUseProgram(program);
  mat_PVM = mat_proj * mat_view* kmuvcl::math::translate<float>(m_translate_x, m_translate_y, m_translate_z) * mat_model;
  glUniformMatrix4fv(loc_u_PVM, 1, GL_FALSE, mat_PVM);
  glUniformMatrix4fv(loc_u_M, 1, GL_FALSE, mat_model);

  view_position_wc[0] = mat_view(0, 3);
  view_position_wc[1] = mat_view(1, 3);
  view_position_wc[2] = mat_view(2, 3);
  glUniform3fv(loc_u_view_position_wc, 1, view_position_wc);
  glUniform3fv(loc_u_light_position_wc, 1, light_position_wc);

  glUniform4fv(loc_u_light_ambient, 1, light_ambient);
  glUniform4fv(loc_u_light_diffuse, 1, light_diffuse);
  glUniform4fv(loc_u_light_specular, 1, light_specular);
  
  glUniform4fv(loc_u_material_ambient, 1, material_ambient);
  glUniform4fv(loc_u_material_specular, 1, material_specular);
  glUniform1f(loc_u_material_shininess, material_shininess);
  if(!shader_flag[1])
    glUniform4fv(loc_u_diffuse_texture, 1, diffuse_texture);


  for (const tinygltf::Primitive& primitive : mesh.primitives)
  {
    if (primitive.material > -1)
    {
      const tinygltf::Material& material = materials[primitive.material];
      for (const std::pair<std::string, tinygltf::Parameter> parameter : material.values)
      {
        if (parameter.first.compare("baseColorTexture") == 0)
        {
          if (parameter.second.TextureIndex() > -1)
          {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuse_texid);

            glUniform1i(loc_u_diffuse_texture, 0);
          }
        }
		    if (parameter.first.compare("baseColorFactor") == 0)
        {
          color_tmp[0] = parameter.second.number_array[0];
          color_tmp[1] = parameter.second.number_array[1];
          color_tmp[2] = parameter.second.number_array[2];
          color_tmp[3] = parameter.second.number_array[3];
          glUniform4fv(loc_u_color, 1, color_tmp);
        }
      }
    }
    int count=-1;

    for (const std::pair<std::string, int>& attrib : primitive.attributes)
    {
      const int accessor_index = attrib.second;
      const tinygltf::Accessor& accessor = accessors[accessor_index];

      int bufferView_index = accessor.bufferView;
      const tinygltf::BufferView& bufferView = bufferViews[bufferView_index];
      const tinygltf::Buffer& buffer = buffers[bufferView.buffer];
      const int byteStride = accessor.ByteStride(bufferView);
      count = accessor.count;

      if (attrib.first.compare("POSITION") == 0)
      {
        glBindBuffer(bufferView.target, position_buffer);
        glEnableVertexAttribArray(loc_a_position);
        glVertexAttribPointer(loc_a_position,
          accessor.type, accessor.componentType,
          accessor.normalized ? GL_TRUE : GL_FALSE, byteStride,
          BUFFER_OFFSET(accessor.byteOffset));
      }
      else if (attrib.first.compare("NORMAL") == 0)
      {
        glBindBuffer(bufferView.target, normal_buffer);
        glEnableVertexAttribArray(loc_a_normal);
        glVertexAttribPointer(loc_a_normal,
          accessor.type, accessor.componentType,
          accessor.normalized ? GL_TRUE : GL_FALSE, byteStride,
          BUFFER_OFFSET(accessor.byteOffset));
      }
      else if (attrib.first.compare("TEXCOORD_0") == 0)
      {
        glBindBuffer(bufferView.target, texcoord_buffer);
        glEnableVertexAttribArray(loc_a_texcoord);
        glVertexAttribPointer(loc_a_texcoord,
          accessor.type, accessor.componentType,
          accessor.normalized ? GL_TRUE : GL_FALSE, byteStride,
          BUFFER_OFFSET(accessor.byteOffset));
      }
      else if (attrib.first.compare("COLOR_0") == 0)
      {
        glBindBuffer(bufferView.target, color_buffer);
        glEnableVertexAttribArray(loc_a_color);
        glVertexAttribPointer(loc_a_color,
          accessor.type, accessor.componentType,
          accessor.normalized ? GL_TRUE : GL_FALSE, 0,
          BUFFER_OFFSET(accessor.byteOffset));
      }
    }
    if(primitive.indices!=-1)
    {
      const tinygltf::Accessor& index_accessor = accessors[primitive.indices];

      int bufferView_index = index_accessor.bufferView;
      const tinygltf::BufferView& bufferView = bufferViews[bufferView_index];
      const tinygltf::Buffer& buffer = buffers[bufferView.buffer];

      glBindBuffer(bufferView.target, index_buffer);

      glDrawElements(primitive.mode,
        index_accessor.count,
        index_accessor.componentType,
        BUFFER_OFFSET(index_accessor.byteOffset));    
    }
    else
    {
      glDrawArrays(primitive.mode, 0, count);
    }
    // 정점 attribute 배열 비활성화
    glDisableVertexAttribArray(loc_a_position);
    if(shader_flag[0])
      glDisableVertexAttribArray(loc_a_color);
    if(shader_flag[1])
      glDisableVertexAttribArray(loc_a_texcoord);
    if(shader_flag[3])
      glDisableVertexAttribArray(loc_a_normal);
  }
  glUseProgram(0);
}

void draw_scene()
{
  const std::vector<tinygltf::Node>& nodes = model.nodes;

  kmuvcl::math::mat4f mat_model;
  mat_model.set_to_identity();
  
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

  for (const tinygltf::Scene& scene : model.scenes)
  {
    for (size_t i = 0; i < scene.nodes.size(); ++i)
    {
      const tinygltf::Node& node = nodes[scene.nodes[i]];
      draw_node(node, mat_model);
    }
  }

}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		camera_index = camera_index == 0 ? 1 : 0;
		if(!camera_index)
			printf("Perspective Camera!");
		else
			printf("Orthographic Camera!");
	}
	if (key == GLFW_KEY_A){
		m_translate_x -= 0.1f;
		std::printf("model's x : %f   y : %f   z : %f\n",m_translate_x,m_translate_y,m_translate_z);
	}
	if (key == GLFW_KEY_D){
		m_translate_x += 0.1f;
		std::printf("model's x : %f   y : %f   z : %f\n",m_translate_x,m_translate_y,m_translate_z);
	}
	if (key == GLFW_KEY_W){
		m_translate_y += 0.1f;
		std::printf("model's x : %f   y : %f   z : %f\n",m_translate_x,m_translate_y,m_translate_z);
	}
	if (key == GLFW_KEY_S){
		m_translate_y -= 0.1f;
		std::printf("model's x : %f   y : %f   z : %f\n",m_translate_x,m_translate_y,m_translate_z);
	}
	if (key == GLFW_KEY_Q){
		m_translate_z += 0.1f;
		std::printf("model's x : %f   y : %f   z : %f\n",m_translate_x,m_translate_y,m_translate_z);
	}
	if (key == GLFW_KEY_E){
		m_translate_z -= 0.1f;
		std::printf("model's x : %f   y : %f   z : %f\n",m_translate_x,m_translate_y,m_translate_z);
	}

	if (key == GLFW_KEY_F){
		light_position_wc[0] -= 0.1f;
		std::cout<< "light's " << light_position_wc << std::endl;
	}
	if (key == GLFW_KEY_H){
		light_position_wc[0] += 0.1f;
		std::cout<< "light's " << light_position_wc << std::endl;
	}
	if (key == GLFW_KEY_T){
		light_position_wc[1] += 0.1f;
		std::cout<< "light's " << light_position_wc << std::endl;
	}
	if (key == GLFW_KEY_G){
		light_position_wc[1] -= 0.1f;
		std::cout<< "light's " << light_position_wc << std::endl;
	}
	if (key == GLFW_KEY_R){
		light_position_wc[2] += 0.1f;
		std::cout<< "light's " << light_position_wc << std::endl;
	}
	if (key == GLFW_KEY_Y){
		light_position_wc[2] -= 0.1f;
		std::cout<< "light's " << light_position_wc << std::endl;
	}

	if (key == GLFW_KEY_J){
		c_translate_x -= 0.1f;
		std::printf("camera's x : %f   y : %f   z : %f\n",c_translate_x,c_translate_y,c_translate_z);
	}
	if (key == GLFW_KEY_L){
		c_translate_x += 0.1f;
		std::printf("camera's x : %f   y : %f   z : %f\n",c_translate_x,c_translate_y,c_translate_z);
	}
	if (key == GLFW_KEY_I){
		c_translate_y += 0.1f;
		std::printf("camera's x : %f   y : %f   z : %f\n",c_translate_x,c_translate_y,c_translate_z);
	}
	if (key == GLFW_KEY_K){
		c_translate_y -= 0.1f;
		std::printf("camera's x : %f   y : %f   z : %f\n",c_translate_x,c_translate_y,c_translate_z);
	}
	if (key == GLFW_KEY_U){
		c_translate_z += 0.1f;
		std::printf("camera's x : %f   y : %f   z : %f\n",c_translate_x,c_translate_y,c_translate_z);
	}
	if (key == GLFW_KEY_O){
		c_translate_z -= 0.1f;
		std::printf("camera's x : %f   y : %f   z : %f\n",c_translate_x,c_translate_y,c_translate_z);
	}
	
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
  {
    g_is_animation = !g_is_animation;
    std::cout << (g_is_animation ? "animation" : "no animation") << std::endl;
  }
}

int main(int argc, char * argv[])
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
  std::string tmp = argv[1];
  tmp = "test_models/" + tmp;
  load_model(model, tmp);

  // GPU의 VBO를 초기화하는 함수 호출
  init_buffer_objects();
  init_texture_objects();
  init_code();
  init_shader_code(vertex_init+vertex_code, "./shader/vertex1.glsl");
  init_shader_code(frag_init+frag_code, "./shader/fragment1.glsl");
  init_shader_program();
  glfwSetKeyCallback(window, key_callback);
  std::cout<<position_buffer<<std::endl;
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
