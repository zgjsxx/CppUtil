#ifndef SRC_COMMON_INCLUDE_STATUS_H_
#define SRC_COMMON_INCLUDE_STATUS_H_

#include <string>

namespace common{
class Status{
public:
    Status() = default;
    Status(int code, const std::string& msg): code_(code), msg_(msg){}
public:
    [[nodiscard]] inline bool ok() const{
        return code_ == 0;
    }
    [[nodiscard]] inline int code() const{
        return code_;
    }
    [[nodiscard]] inline std::string  message() const{
        return msg_;
    }
private:
    int code_{0};
    std::string msg_;
};

}
#endif