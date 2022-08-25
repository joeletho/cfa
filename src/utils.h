// src/utils.h

#pragma once

#include <fstream>
#include <string>

namespace utils
{
    bool valid_utf8 (unsigned char c);
}

namespace utils
{
    bool valid_utf8 (unsigned char c)
    {
      int digit = c - '0';

      return (digit >= 0 && digit < 127);
    }
}

namespace utils::file
{
    std::ifstream get_file (std::string &filename);
    void trim_filename (std::string &filename);
}


namespace utils::file
{
    void trim_filename (std::string &filename)
    {
      if (auto slash = filename.find_first_of ("/\\"); slash != std::string::npos)
        {
          size_t length = filename.size () - slash;
          filename = filename.substr (++slash, length);
        }
    }

    std::ifstream get_file (std::string &filename)
    {
      std::ifstream file;
      if (file.open (filename); !file)
        {
          // Check the previous directory (CMake works from `cmake-build-debug`)
          filename = "../" + filename;

          // Try again
          if (file.open (filename); !file)
            {
              printf ("File does not exist!\n");
            }
        }

      return file;
    }
}
