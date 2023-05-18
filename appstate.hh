#ifndef _MKVEROBJ_APPSTATE_HH_INCLUDED
#define _MKVEROBJ_APPSTATE_HH_INCLUDED

namespace mkverobj
{
    class app_state
    {
    public:
        bool created_bin_file = false;
        bool created_asm_file = false;
        bool created_obj_file = false;
    };    
} // !namespace mkverobj

#endif // !_MKVEROBJ_APPSTATE_HH_INCLUDED
