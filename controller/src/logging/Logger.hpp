#pragma once
#include <string>
#include <vector>
namespace RaveBuddy
{
    class Logger
    {
    private:
        /* data */
    public:
        Logger(/* args */);

        void onMessage(std::string &t_msg);

        ~Logger();
    };
    

} // namespace RaveBuddy
