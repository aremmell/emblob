#ifndef _MKVEROBJ_HH_INCLUDED
#define _MKVEROBJ_HH_INCLUDED

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <functional>

namespace mkverobj
{   
    std::ofstream::pos_type write_file_contents(const std::string& fname,
        std::ios_base::openmode mode, const std::function<void(std::ostream&)>& cb);

    void delete_file_on_unclean_exit(const std::string& fname);
    
} // !namespace mkverobj

#endif // !_MKVEROBJ_HH_INCLUDED
