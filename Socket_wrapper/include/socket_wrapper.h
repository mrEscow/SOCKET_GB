#pragma once
#pragma comment (lib,"Ws2_32.lib")
#pragma comment (lib,"MSVCRTD.lib")
#include <memory>
#include <string>


namespace socket_wrapper
{

    class SocketWrapperImpl;

    class SocketWrapper
    {
    public:
        SocketWrapper();
        ~SocketWrapper();

    public:
        bool initialized() const;
        int get_last_error_code() const;
        std::string get_last_error_string() const;

    private:
        std::unique_ptr<SocketWrapperImpl> impl_;
    };

}