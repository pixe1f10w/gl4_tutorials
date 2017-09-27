#pragma once

#include <unordered_map>
#include <functional>
#include <string>
#include <fstream>
#include <map>

enum class gl_shader_kind {
    unknown = 0,
    fragment,
    vertex,
    // TODO: extend
};

GLenum shader_kind_to_GLenum(const gl_shader_kind kind) {
    static std::map<gl_shader_kind, GLenum> kinds = {
        {gl_shader_kind::fragment, GL_FRAGMENT_SHADER},
        {gl_shader_kind::vertex,   GL_VERTEX_SHADER},
        // TODO: extend
    };
    return kinds[kind];
}

const char* GL_type_to_string (GLenum type) {
    switch(type) {
        case GL_BOOL: return "bool";
        case GL_INT: return "int";
        case GL_FLOAT: return "float";
        case GL_FLOAT_VEC2: return "vec2";
        case GL_FLOAT_VEC3: return "vec3";
        case GL_FLOAT_VEC4: return "vec4";
        case GL_FLOAT_MAT2: return "mat2";
        case GL_FLOAT_MAT3: return "mat3";
        case GL_FLOAT_MAT4: return "mat4";
        case GL_SAMPLER_2D: return "sampler2d";
        case GL_SAMPLER_3D: return "sampler3d";
        case GL_SAMPLER_CUBE: return "sampleCube";
        case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow";
        default: return "other";
    }
}

class gl_shader {
    public:

    gl_shader(const GLenum type, const std::string& source):
        m_type(type), m_source(source) {}

    ~gl_shader() {
    }

    GLuint id() const {
        return m_id;
    }

    bool compile() {
        const char* src = m_source.c_str();
        m_id = glCreateShader(m_type);
        glShaderSource(m_id, 1, &src, nullptr);
        glCompileShader(m_id);

        int result = -1;
        glGetShaderiv(m_id, GL_COMPILE_STATUS, &result);

        return result == GL_TRUE;
    }

    void dump_info_log(tools::dumb_logger& logger) const {
        int max_length = 2048;
        int actual_length = 0;
        char log[2048];
        glGetShaderInfoLog(m_id, max_length, &actual_length, log);
        logger << "shader info log for GL index " << std::to_string(m_id) << ":\n" << log << "\n";
    }

    private:

    GLuint m_id;
    GLenum m_type;
    std::string m_source;
    };

class gl_shader_program {
    public:

    gl_shader_program():
        m_id(glCreateProgram()) {}

    void attach(const gl_shader& shader) {
        glAttachShader(m_id, shader.id());
    }

    friend void operator<<(gl_shader_program& program, gl_shader& shader) {
        program.attach(shader);
    }

    GLuint id() const {
        return m_id;
    }

    void use() const {
        glUseProgram(m_id);
    }

    bool link() {
        glLinkProgram(m_id);
        int result = -1;
        glGetProgramiv(m_id, GL_LINK_STATUS, &result);

        return result == GL_TRUE;
    }

    bool validate() const {
        glValidateProgram(m_id);
        int result = -1;
        glGetProgramiv(m_id, GL_VALIDATE_STATUS, &result);
        return result == GL_TRUE;
    }

    GLint get_uniform_location(const std::string& name) const {
        return glGetUniformLocation(m_id, name.c_str());
    }

    void bind_attribute_location(GLint id, const std::string& name) const {
        glBindAttribLocation(m_id, id, name.c_str());
    }

    void dump_info_log(tools::dumb_logger& logger) const {
        int max_length = 2048;
        int actual_length = 0;
        char log[2048];
        glGetProgramInfoLog(m_id, max_length, &actual_length, log);
        logger << "program info log for GL index " << std::to_string(m_id) << ":\n" << log << "\n";
    }

    void dump_details(tools::dumb_logger& logger) const {
        logger << "-----------------------------\ninformation for shader program " << std::to_string(m_id) << ":\n";
        int params = -1;
        glGetProgramiv(m_id, GL_LINK_STATUS, &params);
        logger << "GL_LINK_STATUS = " << std::to_string(params) << "\n";

        glGetProgramiv(m_id, GL_ATTACHED_SHADERS, &params);
        logger << "GL_ATTACHED_SHADERS = " << std::to_string(params) << "\n";

        glGetProgramiv(m_id, GL_ACTIVE_ATTRIBUTES, &params);
        logger << "GL_ACTIVE_ATTRIBUTES = " << std::to_string(params) << "\n";
        for (GLuint i = 0; i < (GLuint)params; i++) {
            char name[64];
            int max_length = 64;
            int actual_length = 0;
            int size = 0;
            GLenum type;
            glGetActiveAttrib(m_id, i, max_length, &actual_length, &size, &type, name);
            if (size > 1) {
                for (int j = 0; j < size; j++) {
                    char long_name[64];
                    sprintf(long_name, "%s[%i]", name, j);
                    int location = glGetAttribLocation(m_id, long_name);
                    logger << " " << std::to_string(i) << ") type: " << GL_type_to_string(type) << " name: " << long_name << " location: " << std::to_string(location) << "\n";
                }
            } else {
                  int location = glGetAttribLocation(m_id, name);
                  logger << " " << std::to_string(i) << ") type: " << GL_type_to_string(type) << " name: " << name << " location: " << std::to_string(location) << "\n";
            }
        }

        glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &params);
        logger << "GL_ACTIVE_UNIFORMS = " << std::to_string(params) << "\n";
        for (GLuint i = 0; i < (GLuint)params; i++) {
            char name[64];
            int max_length = 64;
            int actual_length = 0;
            int size = 0;
            GLenum type;
            glGetActiveUniform(m_id, i, max_length, &actual_length, &size, &type, name);
            if (size > 1) {
                for (int j = 0; j < size; j++) {
                    char long_name[64];
                    sprintf(long_name, "%s[%i]", name, j);
                    int location = glGetUniformLocation(m_id, long_name);
                    logger << " " << std::to_string(i) << ") type: " << GL_type_to_string(type) << " name: " << long_name << " location: " << std::to_string(location) << "\n";
                }
            } else {
                int location = glGetUniformLocation(m_id, name);
                logger << " " << std::to_string(i) << ") type: " << GL_type_to_string(type) << " name: " << name << " location: " << std::to_string(location) << "\n";
            }
        }
        logger << "-----------------------------\n";
    }

    private:

    GLuint m_id;
};

class gl_shader_loader {
    public:

    gl_shader_loader(tools::dumb_logger& logger, const std::string& root_path = ""):
        m_root_path(root_path),
        m_logger(logger) {}

    gl_shader& from_file(const std::string& filepath, const std::string& alias = "") {
        const std::string fullpath = m_root_path.empty() ? filepath : m_root_path + "/" + filepath;
        std::ifstream ifs(fullpath);
        std::string source;
        source.assign(std::istreambuf_iterator<char>(ifs),
                      std::istreambuf_iterator<char>());

        const std::string& key = alias.empty() ? source : alias;
        if (m_container.count(key)) {
            return m_container.at(key);
        }

        m_logger << "loading shader from '" << fullpath.c_str() << "'\n'''\n" << source << "\n'''\n";

        gl_shader_kind kind = deduce_shader_kind(fullpath);
        if (kind == gl_shader_kind::unknown) {
            throw std::runtime_error("unsupported shader filename");
        }

        gl_shader shader(shader_kind_to_GLenum(kind), source);
        if (shader.compile()) {
            m_logger << "shader " << std::to_string(shader.id()) << " compiled successfully\n";
            m_container.emplace(key, shader);
            return m_container.at(key);
        }

        shader.dump_info_log(m_logger);
        throw std::runtime_error{"unable to compile shader idx " + std::to_string(shader.id())};
    }

    gl_shader& operator()(const std::string& filepath) {
        return from_file(filepath);
    }

    private:

    gl_shader_kind deduce_shader_kind(const std::string& filepath) {
        static std::map<std::string, gl_shader_kind> kinds = {
            {"frag", gl_shader_kind::fragment},
            {"vert", gl_shader_kind::vertex},
            // TODO: extend
        };
        std::string::size_type dot_index = filepath.find_last_of("."); // TODO: better solution
        if (dot_index != std::string::npos) {
            const std::string& extension = filepath.substr(dot_index + 1);
            if (kinds.count(extension)) {
                return kinds.at(extension);
            }
        }
        return gl_shader_kind::unknown;
    }

    std::unordered_map<std::string, gl_shader> m_container;
    std::string m_root_path;
    tools::dumb_logger& m_logger;
};
