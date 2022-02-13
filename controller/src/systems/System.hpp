#pragma once
#include "data/State.hpp"
namespace RaveBuddy
{
    class System
    {
    private:
        /* data */
    public:
        virtual bool requestUpdate(State &t_state) = 0;
        virtual bool responseUpdate(State &t_state)= 0;

    };

    
} // namespace RaveBuddy
