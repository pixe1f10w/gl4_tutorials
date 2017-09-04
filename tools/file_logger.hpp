#pragma once

#include <fstream>

namespace tools {

class file_logger {
    public:

    enum class record_type {
        error,
        warning,
        info
    };

    explicit file_logger(const char *build_version, const char* filename = "default.log"):
        m_warnings_count(0), m_errors_count(0) {
        m_file.open(filename);

        if (m_file.is_open()) {
            m_file << "build version " << build_version << std::endl;
        }
    }

    ~file_logger() {
        if (m_file.is_open()) {
            m_file << std::endl;

            m_file << m_warnings_count << " warnings" << std::endl;
            m_file << m_errors_count << " errors" << std::endl;

            m_file.close();
        }
    }

    friend file_logger& operator<<(file_logger& logger, const record_type type) {
        switch (type) {
            case file_logger::record_type::error:
                logger.m_file << "[ERROR] ";
                ++logger.m_errors_count;
                break;
            case file_logger::record_type::warning:
                logger.m_file << "[WARNING] ";
                ++logger.m_warnings_count;
                break;
            default:
                logger.m_file << "[INFO] ";
                break;
        }

        return logger;
    }

    friend file_logger& operator<<(file_logger& logger, const char* message) {
        logger.m_file << message << std::endl;
        return logger;
    }

    file_logger(const file_logger&) = delete;
    file_logger& operator=(const file_logger&) = delete;

    private:

    std::ofstream m_file;

    unsigned int  m_warnings_count;
    unsigned int  m_errors_count;
};

} // ns tools
