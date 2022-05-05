#pragma once
#include "Includes.hpp"

namespace rage {
    class scrThread;
    class tlsContext  {
    public:
        scrThread** getScriptThread() { return (scrThread**)((DWORD64)this + 0x828); }
        bool* isScriptThreadActive() { return (bool*)((DWORD64)this + 0x830); }
        static tlsContext* get() { return *(tlsContext**)(__readgsqword(0x58)); }
    };
}