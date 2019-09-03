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

#include "../glTF/tiny_gltf.h" //여기 있는 함수들을 이용해 mesh들을 불러온다.
#define BUFFER_OFFSET(i) ((char*)0 + (i))

////////////////////////////////////////////////////////////////////////////////
/// 쉐이더 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLuint  program;          // 쉐이더 프로그램 객체의 레퍼런스 값
GLint   loc_a_position;   // attribute 변수 a_position 위치
GLint   loc_a_color;      // attribute 변수 a_color 위치

GLuint  position_buffer;  // GPU 메모리에서 position_buffer의 위치
GLuint  color_buffer;     // GPU 메모리에서 color_buffer의 위치

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

//모델에 데이터를 채워 넣어주는 함수 (model, gltf_filename)
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

    loc_a_position = glGetAttribLocation(program, "a_position");
    loc_a_color = glGetAttribLocation(program, "a_color");
}

void init_buffer_objects()
{
    // glTF파일은 각 요소들이 리스트로 구성되어 있다. 
    const std::vector<tinygltf::Mesh>& meshes = model.meshes;
    const std::vector<tinygltf::Accessor>& accessors = model.accessors;
    const std::vector<tinygltf::BufferView>& bufferViews = model.bufferViews;
    const std::vector<tinygltf::Buffer>& buffers = model.buffers;

    // 각 mesh를 접근한다.
    for (size_t i = 0; i < meshes.size(); ++i)
    {
        const tinygltf::Mesh& mesh = meshes[i]; // 현재 mesh에 있는 mesh는 단 하나임.

        // mesh 이름이 Triangle인 것을 가져옴.
        if (mesh.name.compare("Triangle") == 0)
        {
            // mesh에 있는 primitive 접근.
            for (size_t j = 0; j < mesh.primitives.size(); ++j)
            {
                const tinygltf::Primitive& primitive = mesh.primitives[j];
                
                // primitive 안에는 attributes들이 있다. 
                // attribute는 position, normal, texture coordinate, color 
                // 있을 수도 있고 없을 수도 있기 때문에 map 구조로.
                for (std::map<std::string, int>::const_iterator it = primitive.attributes.cbegin();
                    it != primitive.attributes.cend();
                    ++it)
                {
                    // attribute 들의 pair (이름, accessor에 대한 index)
                    const std::pair<std::string, int>& attrib = *it; 

                    // accessor 가져옴.
                    const int accessor_index = attrib.second;
                    const tinygltf::Accessor& accessor = accessors[accessor_index];

                    // bufferView 에 대한 index (accessor.bufferView)
                    // bufferView_index가 가리키는 bufferView를 가져옴.
                    // bufferView에서 특정 buffer를 가져옴.
                    const tinygltf::BufferView& bufferView = bufferViews[accessor.bufferView];
                    const tinygltf::Buffer& buffer = buffers[bufferView.buffer];

                    // attribute의 이름이 POSITION일 때
                    if (attrib.first.compare("POSITION") == 0)
                    {
                        //bufferView.target : position정보이냐, index의 정보이냐?
                        glGenBuffers(1, &position_buffer);
                        glBindBuffer(bufferView.target, position_buffer);
                        glBufferData(bufferView.target, bufferView.byteLength,
                            &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
                    }
                    // attribute의 이름이 COLOR_0일 때 
                    else if (attrib.first.compare("COLOR_0") == 0)
                    {
                        glGenBuffers(1, &color_buffer);
                        glBindBuffer(bufferView.target, color_buffer);
                        glBufferData(bufferView.target, bufferView.byteLength,
                            &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
                    }
                }
            }
        }
    }
}

// object rendering: 현재 scene은 삼각형 하나로 구성되어 있음.
void render_object()
{
    // 특정 쉐이더 프로그램 사용
    glUseProgram(program);

    // buffer는 필요없다. why? buffer에 대한 내용은 openGL의 buffer object로 보냈기 때문.
    const std::vector<tinygltf::Mesh>& meshes = model.meshes;
    const std::vector<tinygltf::Accessor>& accessors = model.accessors;
    const std::vector<tinygltf::BufferView>& bufferViews = model.bufferViews;
    
    for (size_t i = 0; i < meshes.size(); ++i)
    {
        // mesh 부터 접근. 
        const tinygltf::Mesh& mesh = meshes[i];

        if (mesh.name.compare("Triangle") == 0)
        {
            // primitive 접근.
            for (size_t j = 0; j < mesh.primitives.size(); ++j)
            {
                const tinygltf::Primitive& primitive = mesh.primitives[j];

                // count : glDrawArray를 사용할 때 필요한 정점의 개수를 받아오기 위함.
                int count = 0;

                // attribute 접근.
                for (std::map<std::string, int>::const_iterator it = primitive.attributes.cbegin();
                    it != primitive.attributes.cend();
                    ++it)
                {
                    const std::pair<std::string, int>& attrib = *it;

                    // accessor에 접근.
                    const int accessor_index = attrib.second;
                    const tinygltf::Accessor& accessor = accessors[accessor_index];

                    // accessor에 변수가 몇 개나 있는지.
                    count = accessor.count;

                    // bufferView에 접근.
                    const tinygltf::BufferView& bufferView = bufferViews[accessor.bufferView];

                    if (attrib.first.compare("POSITION") == 0)
                    {
                        glBindBuffer(bufferView.target, position_buffer);
                        glEnableVertexAttribArray(loc_a_position); 
                        // accessor.type : 몇 개씩 끊어 읽을 거냐. "VEC3" (3)
                        // accessor.componentType : float
                        // accessor.normalized : false
                        // BUFFER_OFFSET(accessor.byteOffset) : 0
                        glVertexAttribPointer(loc_a_position,
                            accessor.type, accessor.componentType,
                            accessor.normalized ? GL_TRUE : GL_FALSE, 0,
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

                // primitive.mode : 4
                // triangle인지 line인지 point인지에 대한 정보.

                // accessor.count : float변수를 3개씩 끊어 읽는 3차원 벡터가 몇개 들어있는지
                // count : 같은 primitive이기 때문에 같은 값을 가지고 있다. 아무거나 받아와서 됨. 
                glDrawArrays(primitive.mode, 0, count);

                // 정점 attribute 배열 비활성화
                glDisableVertexAttribArray(loc_a_position);
                glDisableVertexAttribArray(loc_a_color);
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
    window = glfwCreateWindow(500, 500, "Hello Triangle with glTF 2.0", NULL, NULL);
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

    load_model(model, "simple_triangle.gltf");

    // GPU의 VBO를 초기화하는 함수 호출
    init_buffer_objects();

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TODO: 물체(삼각형)를 렌더링하는 함수 호출
        render_object();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}