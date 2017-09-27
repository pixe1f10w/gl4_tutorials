#pragma once

#include <iostream>
#include <fstream>
#include <string>

namespace tools {

class dumb_logger {
    public:

    enum class message_type {
        error,
        warning,
        notice
    };

    explicit dumb_logger(const char *build_version, const bool mirror = false, const char* filename = "default.log"):
        m_warnings_count(0), m_errors_count(0), m_mirrored(mirror) {
        m_file.open(filename);

        message(std::string("[INFO] starting build version '") + build_version + "'...\n");
    }

    ~dumb_logger() {
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

    friend dumb_logger& operator<<(dumb_logger& logger, const message_type type) {
        switch (type) {
            case dumb_logger::message_type::error:
                logger.message("[ERROR] ");
                ++logger.m_errors_count;
                break;
            case dumb_logger::message_type::warning:
                logger.message("[WARNING] ");
                ++logger.m_warnings_count;
                break;
            default:
                logger.message("[NOTICE] ");
                break;
        }
        return logger;
    }

    friend dumb_logger& operator<<(dumb_logger& logger, const char* text) {
        logger.message(text);
        return logger;
    }

    friend dumb_logger& operator<<(dumb_logger& logger, const unsigned char* text) {
        logger.message(reinterpret_cast<const char*>(text));
        return logger;
    }

    friend dumb_logger& operator<<(dumb_logger& logger, const std::string& text) {
        logger.message(text.c_str());
        return logger;
    }

    dumb_logger(const dumb_logger&) = delete;
    dumb_logger& operator=(const dumb_logger&) = delete;

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

} // ns tools
