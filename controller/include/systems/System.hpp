#pragma once
#include "data/State.hpp"
#include <unordered_map>
namespace RaveBuddy
{
    class System
    {
    private:
        /* data */
    public:
        System() = default;
        ~System() = default;
        System(const System&) = delete;
        System& operator=(const System&) = delete;


        virtual bool requestUpdate(State &t_state) {return false;};
        virtual bool requestUpdateAll(std::unordered_map<uint8_t, State> &t_state){return false;};
        virtual bool responseUpdate(State &t_state) {return false;}

    };

    
} // namespace RaveBuddy
