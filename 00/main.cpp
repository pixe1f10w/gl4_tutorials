#define GLEW_NO_GLU
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <string>
#include <iostream>
#include <fstream>
#include <config.hpp>

const char* load_shader(const std::string& filepath) {
  const std::string& fullpath = std::string(PROJECT_ROOT) + std::string("/") + filepath;
  std::ifstream ifs(fullpath);
  std::string shader;
  shader.assign(std::istreambuf_iterator<char>(ifs),
                std::istreambuf_iterator<char>());
  std::cout << "loaded shader from '" << fullpath << "':\n'''\n" << shader << "'''\n";
  return shader.c_str();
}

int main () {
  // start GL context and O/S window using the GLFW helper library
  if (!glfwInit()) {
    std::cerr << "ERROR: could not start GLFW3\n";
    return 1;
  }

#ifdef __APPLE__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

  GLFWwindow* window = glfwCreateWindow(640, 480, "Hello Triangle", nullptr, nullptr);
  if (!window) {
    std::cerr << "ERROR: could not open window with GLFW3\n";
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent(window);

  // start GLEW extension handler
  glewExperimental = GL_TRUE;
  glewInit();

  // get version info
  const GLubyte* renderer = glGetString(GL_RENDERER);
  const GLubyte* version = glGetString(GL_VERSION);
  std::cout << "Renderer: " << renderer << std::endl;
  std::cout << "OpenGL version supported " <<  version << std::endl;

  // tell GL to only draw onto a pixel if the shape is closer to the viewer
  glEnable(GL_DEPTH_TEST); // enable depth-testing
  glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

  // geometry definition
  const GLfloat points1[] = {
    -.5f, .5f, .0f,
    .5f, .5f, .0f,
    .5f, -.5f, .0f
  };

  // vertex buffer object
  GLuint vbo1 = 0;
  glGenBuffers(1, &vbo1);
  glBindBuffer(GL_ARRAY_BUFFER, vbo1);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points1), points1, GL_STATIC_DRAW);

  // vertex array object
  GLuint vao1 = 0;
  glGenVertexArrays(1, &vao1);
  glBindVertexArray(vao1);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo1);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  const GLfloat points2[] = {
    .5f, -.5f, .0f,
    -.5f, -.5f, .0f,
    -.5f, .5f, .0f
  };

  // vertex buffer object
  GLuint vbo2 = 0;
  glGenBuffers(1, &vbo2);
  glBindBuffer(GL_ARRAY_BUFFER, vbo2);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points2), points2, GL_STATIC_DRAW);

  // vertex array object
  GLuint vao2 = 0;
  glGenVertexArrays(1, &vao2);
  glBindVertexArray(vao2);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo2);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  // shaders loading
  const char* vertex_shader1 = load_shader("00/shader1.vert");

  GLuint vs1 = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs1, 1, &vertex_shader1, nullptr);
  glCompileShader(vs1);

  const char* vertex_shader2 = load_shader("00/shader2.vert");

  GLuint vs2 = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs2, 1, &vertex_shader2, nullptr);
  glCompileShader(vs2);

  const char* fragment_shader1 = load_shader("00/shader1.frag");

  GLuint fs1 = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs1, 1, &fragment_shader1, nullptr);
  glCompileShader(fs1);

  const char* fragment_shader2 = load_shader("00/shader2.frag");

  GLuint fs2 = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs2, 1, &fragment_shader2, nullptr);
  glCompileShader(fs2);

  // shader program definition
  GLuint shader_program1 = glCreateProgram();
  glAttachShader(shader_program1, fs1);
  glAttachShader(shader_program1, vs1);
  glLinkProgram(shader_program1);

  GLuint shader_program2 = glCreateProgram();
  glAttachShader(shader_program2, fs2);
  glAttachShader(shader_program2, vs2);
  glLinkProgram(shader_program2);

  glClearColor(.6f, .6f, .8f, 1.0f);

  // draw loop
  while (!glfwWindowShouldClose(window)) {
    // wipe the drawing surface
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_program1);
    glBindVertexArray(vao1);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glUseProgram(shader_program2);
    glBindVertexArray(vao2);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  // close GL context and any other GLFW resources
  glfwTerminate();
  return 0;
}
