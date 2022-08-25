//  main.cpp

#include "src/cfa.h"

int main (int argc, char **argv)
{
  if (argc > 1)
    {
      for (int i = 1; i < argc; ++i)
        {
          std::string comm_arg (argv[i]);

          if (comm_arg.front () == '-')
            {
              comm_arg = comm_arg.substr (1, comm_arg.size ());
              if (comm_arg == "count")
                {
                  cfa::file_count_program ();
                }
              else if (comm_arg == "rank")
                {
                  cfa::file_rank_program ();
                }
              else if (comm_arg == "test")
                {
                  cfa::tests::run_test_program ();
                }
              else
                {
                  printf ("Could not start program: `%s` not a recognized argument.\n", comm_arg.c_str ());
                }
            }
        }
    }
  else
    {
      // Default
      cfa::file_count_program ();
    }
}