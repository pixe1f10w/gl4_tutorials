#define GLEW_NO_GLU
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <shared/logger.hpp>
#include <shared/shader_manager.hpp>
#include <config.hpp>

common::logger g_log(PROJECT_VERSION, true);
int g_gl_width = 640;
int g_gl_height = 480;
bool g_gl_fullscreen = false;

// GLFW callbacks
void glfw_error_callback(int error, const char* message) {
  g_log << common::logger::message_type::error << message;
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

int main (int argc, char* argv[]) {
  if (argc == 2) {
    std::string param1(argv[1]);
    g_gl_fullscreen = (param1 == "--fullscreen" || param1 == "-f");
  }

  g_log << "Starting GLFW: " << glfwGetVersionString() << "\n";

  // start GL context and O/S window using the GLFW helper library
  if (!glfwInit()) {
    g_log << common::logger::message_type::error << "ERROR: could not start GLFW3\n";
    return 1;
  }

#ifdef __APPLE__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

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
    g_log << common::logger::message_type::error << "could not open window with GLFW3\n";
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

  const GLfloat colors[] = {
      1.0f, .0f, .0f,
      .0f, 1.0f, .0f,
      .0f, .0f, 1.0f
  };

  // vertex buffer object
  GLuint points_vbo = 0;
  glGenBuffers(1, &points_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

  GLuint colors_vbo = 0;
  glGenBuffers(1, &colors_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

  // vertex array object
  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  // shaders loading
  gl_shader_loader shader_loader(g_log, PROJECT_ROOT);
  gl_shader_program shader_program;
  try {
      shader_program << shader_loader("02/shader.vert");
      shader_program << shader_loader("02/shader.frag");
  } catch (std::runtime_error& e) {
      g_log << common::logger::message_type::error << e.what() << "\n";
  }

  glBindAttribLocation(shader_program.id(), 0, "vertex_position");
  glBindAttribLocation(shader_program.id(), 1, "vertex_color");

  if (!shader_program.link()) {
    g_log << common::logger::message_type::error << "could not link shader program GL index " << std::to_string(shader_program.id()) << "\n";
    shader_program.dump_info_log(g_log);
  } else {
    shader_program.dump_details(g_log);
    bool is_valid = shader_program.validate();
    g_log << "program " << std::to_string(shader_program.id()) << " GL_VALIDATE_STATUS = " << std::to_string(is_valid) << "\n";
    if (!is_valid) {
        shader_program.dump_info_log(g_log);
    }
  }

  glClearColor(.6f, .6f, .8f, 1.0f);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CW);

  // draw loop
  while (!glfwWindowShouldClose(window)) {
    update_fps_counter(window);

    // wipe the drawing surface
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, g_gl_width, g_gl_height);

    shader_program.use();
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

