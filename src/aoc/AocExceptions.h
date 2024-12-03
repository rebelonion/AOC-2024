#pragma once

#include <stdexcept>
#include <string>


namespace aoc::exceptions {
    class AocException : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class FileOpenError final : public AocException {
    public:
        explicit FileOpenError(const std::string &filename)
            : AocException("Failed to open file: " + filename) {
        }
    };

    class DataFormatError final : public AocException {
    public:
        explicit DataFormatError(const std::string &message)
            : AocException("Data format error: " + message) {
        }
    };

    class InputParseError final : public AocException {
    public:
        explicit InputParseError(const std::string &message)
            : AocException("Input parsing error: " + message) {
        }
    };

    class AlgorithmError final : public AocException {
    public:
        explicit AlgorithmError(const std::string &message)
            : AocException("Algorithm error: " + message) {
        }
    };
} // namespace aoc::exceptions
