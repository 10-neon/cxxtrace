#pragma once
#include <string>

namespace neon {

class SourceLocation {
   public:
    constexpr SourceLocation() noexcept : filepath_(""), line_(0) {}
    constexpr SourceLocation(const SourceLocation& other) noexcept
        : filepath_(other.filepath_), line_(other.line_) {}
    constexpr SourceLocation(SourceLocation&& other) noexcept
        : filepath_(other.filepath_), line_(other.line_) {}
    SourceLocation& operator=(const SourceLocation& other) noexcept {
        filepath_ = other.filepath_;
        line_ = other.line_;
        return *this;
    }
    SourceLocation& operator=(SourceLocation&& other) noexcept {
        filepath_ = other.filepath_;
        line_ = other.line_;
        return *this;
    }

    static constexpr SourceLocation current(
        const char* filepath = __builtin_FILE(),
        const int line = __builtin_LINE()) noexcept {
        return SourceLocation(filepath, line);
    }

    constexpr const char* filepath() const noexcept { return filepath_; }

    const char* filename() const noexcept;

    constexpr int line() const noexcept { return line_; }
    std::string to_string() const;

   private:
    constexpr SourceLocation(const char* filepath, int line_num) noexcept
        : filepath_(filepath), line_(line_num) {}
    const char* filepath_;
    int line_;
};

using Location = SourceLocation;

}  // namespace neon
