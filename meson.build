project('opengl tutorials', 'cpp', default_options: ['cpp_std=c++14'])

# use vcs_tag instead?
version = run_command('git', ['log', '-1', '--format=%h']).stdout().strip()

conf_data = configuration_data()
conf_data.set_quoted('PROJECT_VERSION', version)
conf_data.set_quoted('PROJECT_ROOT', meson.source_root())

configure_file(output: 'config.hpp',
               configuration: conf_data)

project_directory = include_directories('.')

if (build_machine.system() == 'windows')
  opengldep = dependency('opengl32')
else
  opengldep = dependency('GL')
endif

glfwdep = dependency('glfw3')
glewdep = dependency('glew')

project_dependencies = [opengldep, glfwdep, glewdep]

subdir('03')
subdir('04')
subdir('tests')
