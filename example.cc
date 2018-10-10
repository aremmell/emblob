#include <iostream>
#include <cstdlib>
#include "version.h"

int main (int argc, char** argv)
{
    auto res = get_version_resource();
    std::cout << "major: " << res->major << ", minor: " << res->minor << ", build: " << res->build
        << ", notes: '" << res->notes << "'" << std::endl;
    return EXIT_SUCCESS;
}
