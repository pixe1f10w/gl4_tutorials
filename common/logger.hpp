#pragma once

#include <iostream>
#include <fstream>
#include <string>

namespace common {

class logger {
    public:

    enum class message_type {
        error,
        warning,
        notice
    };

    explicit logger(const char *build_version, const bool mirror = false, const char* filename = "default.log"):
        m_warnings_count(0), m_errors_count(0), m_mirrored(mirror) {
        m_file.open(filename);

        message(std::string("[INFO] starting build version '") + build_version + "'...\n");
    }

    ~logger() {
        message("[INFO] shutting down...\n");
        message(std::to_string(m_warnings_count) + " warnings\n");
        message(std::to_string(m_errors_count) + " errors\n");

        if (m_file.is_open()) {
            m_file.close();
        }
    }

    void toggle_mirroring() {
        m_mirrored = !m_mirrored;
    }

    friend logger& operator<<(logger& log, const message_type type) {
        switch (type) {
            case logger::message_type::error:
                log.message("[ERROR] ");
                ++log.m_errors_count;
                break;
            case logger::message_type::warning:
                log.message("[WARNING] ");
                ++log.m_warnings_count;
                break;
            default:
                log.message("[NOTICE] ");
                break;
        }
        return log;
    }

    friend logger& operator<<(logger& log, const char* text) {
        log.message(text);
        return log;
    }

    friend logger& operator<<(logger& log, const unsigned char* text) {
        log.message(reinterpret_cast<const char*>(text));
        return log;
    }

    friend logger& operator<<(logger& log, const std::string& text) {
        log.message(text.c_str());
        return log;
    }

    logger(const logger&) = delete;
    logger& operator=(const logger&) = delete;

    private:

    void message(const char* text) {
        if (m_mirrored) {
            std::cerr << text;
        }
        if (m_file.is_open()) {
            m_file << text;
        }
    }

    void message(const std::string& text) {
        message(text.c_str());
    }

    std::ofstream m_file;

    unsigned int  m_warnings_count;
    unsigned int  m_errors_count;
    bool          m_mirrored;
};

} // ns common
