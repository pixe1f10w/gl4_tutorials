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

    bool link() {
        glLinkProgram(m_id);
        int result = -1;
        glGetProgramiv(m_id, GL_LINK_STATUS, &result);

        return result == GL_TRUE;
    }

    GLuint id() const {
        return m_id;
    }

    bool validate() const {
        glValidateProgram(m_id);
        int result = -1;
        glGetProgramiv(m_id, GL_VALIDATE_STATUS, &result);
        return result == GL_TRUE;
    }

    private:

    GLuint m_id;
};

class gl_shader_manager {
    public:

    gl_shader_manager() {}

    gl_shader& load_from_file(const std::string& filepath) {
        std::ifstream ifs(filepath);
        std::string source;
        source.assign(std::istreambuf_iterator<char>(ifs),
                      std::istreambuf_iterator<char>());
        std::size_t hash = std::hash<std::string>{}(source);
        if (m_container.count(hash)) {
            return m_container.at(hash);
        }
        //g_log << "loaded_shader from '" << filepath.c_str() << "'\n'''\n" << shader.c_str() << "\n'''\n";
        gl_shader_kind kind = deduce_shader_kind(filepath);
        if (kind == gl_shader_kind::unknown) {
            throw std::runtime_error("unsupported shader filename");
        }
        gl_shader shader(shader_kind_to_GLenum(kind), source);
        if (shader.compile()) {
            m_container.emplace(hash, shader);
            return m_container.at(hash);
        }
        throw std::runtime_error{"unable to compile shader idx " + std::to_string(shader.id())};
    }

    private:

    gl_shader_kind deduce_shader_kind(const std::string& filepath) {
        static std::map<std::string, gl_shader_kind> kinds = {
            {"frag", gl_shader_kind::fragment},
            {"vert", gl_shader_kind::vertex},
            // TODO: extend
        };
        std::string::size_type dot_index = filepath.find_last_of(".");
        if (dot_index != std::string::npos) {
            const std::string& extension = filepath.substr(dot_index + 1);
            if (kinds.count(extension)) {
                return kinds.at(extension);
            }
        }
        return gl_shader_kind::unknown;
    }

    std::unordered_map<std::size_t, gl_shader> m_container;
};
