#define GLEW_NO_GLU
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <string>
#include <iostream>
#include <fstream>

const char* load_shader(const std::string& filepath) {
  std::ifstream ifs(filepath);
  std::string shader;
  shader.assign(std::istreambuf_iterator<char>(ifs),
                std::istreambuf_iterator<char>());
  std::cout << "loaded shader from '" << filepath << "':\n'''\n" << shader << "'''\n";
  return shader.c_str();
}

int main () {
  // start GL context and O/S window using the GLFW helper library
  if (!glfwInit()) {
    std::cerr << "ERROR: could not start GLFW3\n";
    return 1;
  }

  GLFWwindow* window = glfwCreateWindow (640, 480, "Hello Triangle", nullptr, nullptr);
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
  const GLfloat points[] = {
    /*
    .0f, .5f, .0f,
    .5f, -.5f, .0f,
    -.5f, -.5f, .0f
    */
    -.5f, .5f, .0f,
    .5f, .5f, .0f,
    .5f, -.5f, .0f,

    .5f, -.5f, .0f,
    -.5f, -.5f, .0f,
    -.5f, .5f, .0f
  };

  // vertex buffer object
  GLuint vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

  // vertex array object
  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  // shaders loading
  const char* vertex_shader = load_shader("../vertex.shader");

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vertex_shader, nullptr);
  glCompileShader(vs);

  const char* fragment_shader = load_shader("../fragment.shader");

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fragment_shader, nullptr);
  glCompileShader(fs);

  // shader program definition
  GLuint shader_program = glCreateProgram();
  glAttachShader(shader_program, fs);
  glAttachShader(shader_program, vs);
  glLinkProgram(shader_program);

  glClearColor(.6f, .6f, .8f, 1.0f);

  // draw loop
  while (!glfwWindowShouldClose(window)) {
    // wipe the drawing surface
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_program);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  // close GL context and any other GLFW resources
  glfwTerminate();
  return 0;
}
