#define GLEW_NO_GLU
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include "../tools/dumb_logger.hpp"

tools::dumb_logger g_log("unknown", true);
int g_gl_width = 640;
int g_gl_height = 480;
bool g_gl_fullscreen = false;

struct color {
  GLfloat r = .0f;
  GLfloat g = .0f;
  GLfloat b = .0f;
  GLfloat a = 1.0f;
} g_color;

// GLFW callbacks
void glfw_error_callback(int error, const char* message) {
  g_log << tools::dumb_logger::message_type::error << message;
}

void glfw_window_size_callback(GLFWwindow* window, int width, int height) {
  g_gl_width = width;
  g_gl_height = height;
}

void update_fps_counter(GLFWwindow* window) {
  static const double threshold = .5f;
  static double previous_seconds = glfwGetTime();
  static int frame_count;
  double current_seconds = glfwGetTime();
  double elapsed_seconds = current_seconds - previous_seconds;
  if (elapsed_seconds > threshold) {
    previous_seconds = current_seconds;
    double fps = (double)frame_count / elapsed_seconds;
    char tmp[128];
    sprintf(tmp, "fps: %.2f", fps);
    glfwSetWindowTitle(window, tmp);
    frame_count = 0;
  }
  frame_count++;
}

void update_color(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    g_color.r = 1.0f;
  } else {
    g_color.r = .0f;
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    g_color.g = 1.0f;
  } else {
    g_color.g = .0f;
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    g_color.b = 1.0f;
  } else {
    g_color.b = .0f;
  }
}

void log_gl_parameters() {
  std::map<GLuint, const char*> params = {
    {GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS"},
    {GL_MAX_CUBE_MAP_TEXTURE_SIZE,        "GL_MAX_CUBE_MAP_TEXTURE_SIZE"},
    {GL_MAX_DRAW_BUFFERS,                 "GL_MAX_DRAW_BUFFERS"},
    {GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,  "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS"},
    {GL_MAX_TEXTURE_IMAGE_UNITS,          "GL_MAX_TEXTURE_IMAGE_UNITS"},
    {GL_MAX_TEXTURE_SIZE,                 "GL_MAX_TEXTURE_SIZE"},
    {GL_MAX_VARYING_FLOATS,               "GL_MAX_VARYING_FLOATS"},
    {GL_MAX_VERTEX_ATTRIBS,               "GL_MAX_VERTEX_ATTRIBS"},
    {GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,   "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS"},
    {GL_MAX_VERTEX_UNIFORM_COMPONENTS,    "GL_MAX_VERTEX_UNIFORM_COMPONENTS"},
    {GL_MAX_VIEWPORT_DIMS,                "GL_MAX_VIEWPORT_DIMS"},
    {GL_STEREO,                           "GL_STEREO"}
  };

  // get version info
  const GLubyte* renderer = glGetString(GL_RENDERER);
  const GLubyte* version = glGetString(GL_VERSION);
  g_log << "Renderer: " << renderer << "\n";
  g_log << "OpenGL version supported " << version << "\n";

  g_log << "----------------------------------------\n";
  g_log << "GL context parameters:\n";
  for (const auto pair : params) {
    if (pair.first == GL_MAX_VIEWPORT_DIMS) {
      int v[2] = {0, 0};
      glGetIntegerv(pair.first, v);
      g_log << pair.second << " == " << std::to_string(v[0]).c_str() << ", " << std::to_string(v[1]).c_str() << "\n";
      continue;
    }
    if (pair.first == GL_STEREO) {
      unsigned char v = 0;
      glGetBooleanv(pair.first, &v);
      g_log << pair.second << " == " << std::to_string(v).c_str() << "\n";
      continue;
    }
    int v = 0;
    glGetIntegerv(pair.first, &v);
    g_log << pair.second << " == " << std::to_string(v).c_str() << "\n";
  }
  g_log << "----------------------------------------\n";
}

const char* load_shader(const std::string& filepath) {
  std::ifstream ifs(filepath);
  std::string shader;
  shader.assign(std::istreambuf_iterator<char>(ifs),
                std::istreambuf_iterator<char>());
  g_log << "loaded_shader from '" << filepath.c_str() << "'\n'''\n" << shader.c_str() << "\n'''\n";
  return shader.c_str();
}

int main (int argc, char* argv[]) {
  if (argc == 2) {
    std::string param1(argv[1]);
    g_gl_fullscreen = (param1 == "--fullscreen" || param1 == "-f");
  }

  g_log << "Starting GLFW: " << glfwGetVersionString() << "\n";

  // start GL context and O/S window using the GLFW helper library
  if (!glfwInit()) {
    g_log << tools::dumb_logger::message_type::error << "ERROR: could not start GLFW3\n";
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_SAMPLES, 4);
  GLFWwindow* window;

  if (g_gl_fullscreen) {
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* video_mode = glfwGetVideoMode(monitor);
    g_gl_width = video_mode->width;
    g_gl_height = video_mode->height;
    window = glfwCreateWindow(g_gl_width, g_gl_height, "Extended GL init", monitor, nullptr);
  } else {
    window = glfwCreateWindow(g_gl_width, g_gl_height, "Extended GL init", nullptr, nullptr);
  }

  if (!window) {
    g_log << tools::dumb_logger::message_type::error << "ERROR: could not open window with GLFW3\n";
    glfwTerminate();
    return 1;
  }

  glfwSetErrorCallback(glfw_error_callback);
  glfwSetWindowSizeCallback(window, glfw_window_size_callback);

  glfwMakeContextCurrent(window);

  // start GLEW extension handler
  glewExperimental = GL_TRUE;
  glewInit();

  log_gl_parameters();

  // tell GL to only draw onto a pixel if the shape is closer to the viewer
  glEnable(GL_DEPTH_TEST); // enable depth-testing
  glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

  // geometry definition
  const GLfloat points[] = {
    .0f, .5f, .0f,
    .5f, -.5f, .0f,
    -.5f, -.5f, .0f
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
  const char* vertex_shader = load_shader("../shader.vert");

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs, 1, &vertex_shader, nullptr);
  glCompileShader(vs);

  const char* fragment_shader = load_shader("../shader.frag");

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs, 1, &fragment_shader, nullptr);
  glCompileShader(fs);

  // shader program definition
  GLuint shader_program = glCreateProgram();
  glAttachShader(shader_program, fs);
  glAttachShader(shader_program, vs);
  glLinkProgram(shader_program);

  GLint color_location = glGetUniformLocation(shader_program, "input_color");

  glClearColor(.6f, .6f, .8f, 1.0f);

  // draw loop
  while (!glfwWindowShouldClose(window)) {
    update_fps_counter(window);
    update_color(window);

    // wipe the drawing surface
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, g_gl_width, g_gl_height);

    glUseProgram(shader_program);
    glUniform4f(color_location, g_color.r, g_color.g, g_color.b, g_color.a);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwPollEvents();
    glfwSwapBuffers(window);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, true);
    }
  }

  // close GL context and any other GLFW resources
  glfwTerminate();
  return 0;
}

