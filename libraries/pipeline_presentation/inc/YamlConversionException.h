#pragma once

#include <stdexcept>

class YamlConversionException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};
