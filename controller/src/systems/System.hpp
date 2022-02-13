#pragma once
#include "data/State.hpp"
namespace RaveBuddy
{
    class System
    {
    private:
        /* data */
    public:
        virtual bool requestUpdate(State &t_state);
        virtual bool responseUpdate(State &t_state);

    };

    
} // namespace RaveBuddy
