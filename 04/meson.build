project('tutorial04', 'cpp', default_options: ['cpp_std=c++14'])

if (build_machine.system() == 'windows')
  opengldep = dependency('opengl32')
else
  opengldep = dependency('GL')
endif

glfwdep = dependency('glfw3')
glewdep = dependency('glew')

executable('tutorial04', 'main.cpp', dependencies: [opengldep, glfwdep, glewdep])
