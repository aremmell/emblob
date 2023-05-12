#ifndef _LINKERS_HH_INCLUDED
#define _LINKERS_HH_INCLUDED

#include <list>
#include <string>

namespace mkverobj
{
    /** contains a list of strings that are passed to a 
     * linker, separated by whitespace. */
    typedef std::list<std::string> linker_cmdline_args;

    class Linker
    {
        public:
            Linker(const std::string& name) : _name(name) {
            }

            virtual ~Linker() {}

            /** @brief Gives a detailed description of the linker.
             * 
             * @return std::string
             */
            virtual std::string get_description() const = 0;

            // TODO: figure out all the ways in which linkers
            // can differ, how to identify them, and then trick them.

        protected:
            linker_cmdline_args _args = {};
            std::string _name;
    };
}; // !namespace mkverobj

#endif // !_LINKERS_HH_INCLUDED
