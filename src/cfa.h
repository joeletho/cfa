// src/cfa.h

#pragma once

#include <cassert>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <random>

#include "utils.h"

//----------------------------------------------------//
//----------------------------------------------------//
//                                                    //
//                FORWARD DECLARATIONS                //
//                                                    //
//----------------------------------------------------//
//----------------------------------------------------//

namespace cfa
{
    const int MAX_STRING_LENGTH = 1024 * 4;

    enum class SortMethod {
        None = 0,
        Char_Ascending,
        Char_Descending,
        Value_Ascending,
        Value_Descending,
    };

    enum class ParseType {
        None = 1 << 0,
        Alpha = 1 << 2,
        Digit = 1 << 4,
        Symbol = 1 << 8,
        AlNum = Alpha | Digit,
        Ascii = AlNum | Symbol,
    };

    //--------------------------------------------
    //  [ SECTION TYPES ]
    //--------------------------------------------

    template<typename Num>
    struct CharMap;

    template<typename Num>
    struct CharVec;

    //----------------------------------------------------
    //  [ SECTION FUNCTIONS ]   Character Count
    //----------------------------------------------------

    template<typename Num>
    std::unique_ptr<CharMap<Num>> get_char_count_map (std::string &str, ParseType type);
    template<typename Num>
    std::unique_ptr<CharMap<Num>> get_char_count_map (std::ifstream &opened_file, ParseType type);
    template<typename Num>
    std::unique_ptr<CharVec<Num>> get_char_count_vec (std::string &str, ParseType type);
    template<typename Num>
    std::unique_ptr<CharVec<Num>> get_char_count_vec (std::ifstream &opened_file, ParseType type);

    //----------------------------------------------------
    //  [ SECTION FUNCTIONS ]   Character ranks
    //----------------------------------------------------

    std::unique_ptr<CharMap<float>> get_char_rank_map (std::string &str, ParseType type);
    std::unique_ptr<CharMap<float>> get_char_rank_map (std::ifstream &opened_file, ParseType type);
    std::unique_ptr<CharVec<float>> get_char_rank_vec (std::string &str, ParseType type);
    std::unique_ptr<CharVec<float>> get_char_rank_vec (std::ifstream &opened_file, ParseType type);

    //----------------------------------------------------
    //  [ SECTION FUNCTIONS ]   Display
    //----------------------------------------------------

    void header_prompt ();
    void file_count_program ();
    void file_rank_program ();
    void print_char_count (std::unique_ptr<CharVec<int>> &vec);
    void print_char_rank (std::unique_ptr<CharVec<float>> &vec);
}

//----------------------------------------------------
//  [ SECTION CFA TESTS ]
//----------------------------------------------------

namespace cfa::tests
{
    // Helpers
    std::string get_filename ();
    std::string generate_test_file ();
    SortMethod get_sort_method ();
    ParseType get_parse_selection ();
    int get_display_value ();

    // Tests
    void run_test_program ();
    void test_file_read ();
    void test_user_input ();
}

//----------------------------------------------------//
//----------------------------------------------------//
//                                                    //
//                    DEFINITIONS                     //
//                                                    //
//----------------------------------------------------//
//----------------------------------------------------//

namespace cfa
{
    //--------------------------------------------
    //  [ SECTION TYPES ]
    //--------------------------------------------:w

    template<typename Num>
    struct CharMap {
        std::unordered_map<char, Num> Data;

        CharMap<Num> () = default;

        explicit CharMap<Num> (std::unique_ptr<CharMap<Num>> map)
        {
          if (this != map)
            {
              if (!this)
                {
                  this = std::unique_ptr<CharMap<Num>> (new CharMap<Num>);
                }

              this->Data = map->Data;
            }
        }

        [[maybe_unused]] void increment (char c)
        {
          ++Data[c];
        }

        [[maybe_unused]] void decrement (char c)
        {
          --Data[c];
        }

        void increment_if (char c, ParseType type)
        {
          // Count number of times a particular char is read.
          if (utils::valid_utf8 (c))
            {
              switch (type)
                {
                  case ParseType::None:
                    break;
                  case ParseType::Alpha:
                    if (std::isalpha (c))
                      {
                        c = (char) std::toupper (c);
                        increment (c);
                      }
                  break;
                  case ParseType::Digit:
                    if (std::isdigit (c))
                      {
                        increment (c);
                      }
                  break;
                  case ParseType::Symbol:
                    if (!std::isalnum (c))
                      {
                        increment (c);
                      }
                  break;
                  case ParseType::AlNum:
                    if (std::isalnum (c))
                      {
                        if (std::isalpha (c))
                          {
                            c = (char) std::toupper (c);
                          }
                        increment (c);
                      }
                  break;
                  case ParseType::Ascii:
                    if (std::isalpha (c))
                      {
                        c = (char) std::toupper (c);
                      }
                  increment (c);
                  break;
                }
            }
        }

        std::unique_ptr<CharMap<Num>> ranks_to_map ()
        {
          std::unique_ptr<CharMap<Num>> tmp (new CharMap<Num> (*this));

          // We use _sum_ to normalize the results.
          [[maybe_unused]] float sum = 0;
          for ([[maybe_unused]] auto &[c, n]: tmp->Data)
            {
              sum += n;
            }

          for ([[maybe_unused]] auto &[c, n]: tmp->Data)
            {
              tmp->Data[c] = n / (float) sum;
            }

          return tmp;
        }

        std::unique_ptr<CharVec<Num>> ranks_to_vec ()
        {
          return ranks_to_map ()->copy_to_vec ();
        }

        std::unique_ptr<CharVec<Num>> copy_to_vec ()
        {
          std::unique_ptr<CharVec<Num>> vec (new CharVec<Num>);
          for ([[maybe_unused]] auto pair: Data)
            {
              vec->emplace_back (pair);
            }

          return vec;
        }
    };

    template<typename Num>
    struct CharVec {
        std::vector<std::pair<char, Num>> Data;

        void emplace_back (std::pair<const char, Num> &pair)
        {
          Data.emplace_back (pair);
        }

        void sort (SortMethod method)
        {
          for (;;)
            {
              switch (method)
                {
                  case SortMethod::None:
                    return;
                  case SortMethod::Char_Ascending:
                    _sort_char_ascending ();
                  return;
                  case SortMethod::Char_Descending:
                    _sort_char_descending ();
                  return;
                  case SortMethod::Value_Ascending:
                    _sort_value_ascending ();
                  return;
                  case SortMethod::Value_Descending:
                    _sort_value_descending ();
                  return;
                }
            }
        }

        void _sort_char_ascending ()
        {
          // Sort in ascending order
          std::sort (Data.begin (), Data.end (), [&]
              (const std::pair<char, Num> &a, const std::pair<char, Num> &b)
          {
              return a.first < b.first;
          });
        }

        void _sort_char_descending ()
        {
          // Sort in descending order
          std::sort (Data.begin (), Data.end (), [&]
              (const std::pair<char, Num> &a, const std::pair<char, Num> &b)
          {
              return a.first > b.first;
          });
        }

        void _sort_value_ascending ()
        {
          // Sort in ascending order
          std::sort (Data.begin (), Data.end (), [&]
              (const std::pair<char, Num> &a, const std::pair<char, Num> &b)
          {
              return a.second < b.second;
          });
        }

        void _sort_value_descending ()
        {
          // Sort in descending order
          std::sort (Data.begin (), Data.end (), [&]
              (const std::pair<char, Num> &a, const std::pair<char, Num> &b)
          {
              return a.second > b.second;
          });
        }
    };

    //----------------------------------------------------
    //  [ SECTION FUNCTIONS ]   Character Count
    //----------------------------------------------------

    template<typename Num>
    std::unique_ptr<CharMap<Num>> get_char_count_map (std::string &str, ParseType type)
    {
      std::unique_ptr<CharMap<Num>> char_map (new CharMap<Num>);

      for (char c: str)
        {
          char_map->increment_if (c, type);
        }
      return char_map;
    }

    template<typename Num>
    std::unique_ptr<CharMap<Num>> get_char_count_map (std::ifstream &opened_file, ParseType type)
    {
      assert(opened_file.is_open ());

      std::unique_ptr<CharMap<Num>> char_map (new CharMap<Num>);

      opened_file.seekg (0);
      while (!opened_file.eof ())
        {
          char c = (char) opened_file.get ();
          char_map->increment_if (c, type);
        }
      opened_file.close ();

      return char_map;
    }

    template<typename Num>
    std::unique_ptr<CharVec<Num>> get_char_count_vec (std::string &str, ParseType type)
    {
      return get_char_count_map<Num> (str, type)->copy_to_vec ();

    }
    template<typename Num>
    std::unique_ptr<CharVec<Num>> get_char_count_vec (std::ifstream &opened_file, ParseType type)
    {
      return get_char_count_map<Num> (opened_file, type)->copy_to_vec ();
    }

    //----------------------------------------------------
    //  [ SECTION FUNCTIONS ]   Character ranks
    //----------------------------------------------------

    std::unique_ptr<CharMap<float>> get_char_rank_map (std::string &str, ParseType type)
    {
      return get_char_count_map<float> (str, type)->ranks_to_map ();
    }

    std::unique_ptr<CharMap<float>> get_char_rank_map (std::ifstream &opened_file, ParseType type)
    {
      return get_char_count_map<float> (opened_file, type)->ranks_to_map ();
    }

    std::unique_ptr<CharVec<float>> get_char_rank_vec (std::string &str, ParseType type)
    {
      return get_char_count_map<float> (str, type)->ranks_to_vec ();
    }

    std::unique_ptr<CharVec<float>> get_char_rank_vec (std::ifstream &opened_file, ParseType type)
    {
      return get_char_count_map<float> (opened_file, type)->ranks_to_vec ();
    }

    //----------------------------------------------------
    //  [ SECTION FUNCTIONS ]   Display
    //----------------------------------------------------

    void header_prompt ()
    {
      printf ("\n"
              "Character Frequency Analyzer\n"
              "----------------------------\n");
    }

    void file_count_program ()
    {
      cfa::header_prompt ();
      for (;;)
        {
          std::string filename = tests::get_filename ();
          if (filename.size () == 1)
            {
              if ((char) toupper (filename.front ()) == 'Q')
                {
                  return;
                }
              else
                {
                  printf ("Invalid choice\n");
                  continue;
                }
            }

          if (auto file = utils::file::get_file (filename); file)
            {
              auto char_counts = cfa::get_char_count_vec<int> (file, cfa::ParseType::Alpha);
              char_counts->sort (SortMethod::Char_Ascending);
              cfa::print_char_count (char_counts);
            }
        }
    }

    void file_rank_program ()
    {
      cfa::header_prompt ();
      for (;;)
        {
          std::string filename = tests::get_filename ();
          if (filename.size () == 1)
            {
              if ((char) toupper (filename.front ()) == 'Q')
                {
                  return;
                }
              else
                {
                  printf ("Invalid choice\n");
                  continue;
                }
            }

          if (auto file = utils::file::get_file (filename); file)
            {
              auto char_ranks = cfa::get_char_rank_vec (file, cfa::ParseType::Alpha);
              char_ranks->sort (SortMethod::Char_Ascending);
              cfa::print_char_rank (char_ranks);
            }
        }
    }

    void print_char_count (std::unique_ptr<CharVec<int>> &vec)
    {
      printf ("\n"
              "------------------\n"
              "   Char   Count\n"
              "------------------\n");

      // Print the results
      for (auto &[c, n]: vec->Data)
        {
          printf ("    %c     %d\n", c, n);
        }
      printf ("\n");
    }

    void print_char_rank (std::unique_ptr<CharVec<float>> &vec)
    {
      printf ("\n"
              "---------------------\n"
              "   Char    Rank\n"
              "---------------------\n");

      // Print the results
      for (auto &[c, n]: vec->Data)
        {
          printf ("    %c     %.4f\n", c, n);
        }
      printf ("\n");
    }
}


//----------------------------------------------------
//  [ SECTION CFA TESTS ]
//----------------------------------------------------

namespace cfa::tests
{
    std::string get_filename ()
    {
      std::string filename;
      printf ("Enter filename (\"Q\" to quit): ");
      std::cin >> filename;
      std::cin.ignore ();

      return filename;
    }

    std::string generate_test_file ()
    {
      std::string input;
      for (;;)
        {
          printf ("\n"
                  "Create a test file (y/n)? ");
          std::cin >> input;
          std::cin.ignore ();

          if (input.size () > 1)
            {
              printf ("Invalid choice\n");
            }
          else
            {
              switch ((char) std::toupper (input.front ()))
                {
                  case 'N':
                    return "";
                  case 'Y':
                    {
                      std::string filename = "test.cfa";
                      if (std::ofstream file (filename, std::ios::binary | std::ios::out); file)
                        {
                          std::random_device dev;
                          std::mt19937 generator (dev ());
                          std::uniform_int_distribution<std::mt19937::result_type> range (32, 126);

                          for (size_t i = 0; i < 1000 * 1000; ++i)
                            {
                              // Generate random ascii from 32 - 126 ('SPACE' - '~')
                              unsigned char c = range (generator);
                              file << c;
                            }
                          file.close ();
                          printf ("Created '%s'\n", filename.c_str ());
                        }
                      return filename;
                    }
                  default:
                    printf ("Invalid choice\n");
                }
            }
        }
    }

    SortMethod get_sort_method ()
    {
      for (;;)
        {
          printf ("\n"
                  "Sort Results:\n"
                  "\t1. Char Ascending\n"
                  "\t2. Char Descending\n"
                  "\t3. Value Ascending\n"
                  "\t4. Value Descending\n"
                  "\t5. None\n"
                  "Enter Selection: ");
          int selection = std::cin.get () - '0';
          std::cin.ignore (MAX_STRING_LENGTH, '\n');

          switch (selection)
            {
              case 1:
                return SortMethod::Char_Ascending;
              case 2:
                return SortMethod::Char_Descending;
              case 3:
                return SortMethod::Value_Ascending;
              case 4:
                return SortMethod::Value_Descending;
              case 5:
                return SortMethod::None;
              default:
                printf ("Invalid selection\n");
            }
        }
    }

    ParseType get_parse_selection ()
    {
      for (;;)
        {
          printf ("\n"
                  "Select Parse Method:\n"
                  "\t1. Alpha\n"
                  "\t2. Numeric\n"
                  "\t3. Alpha-numeric\n"
                  "\t4. Symbol (non-alpha-numeric)\n"
                  "\t5. ASCII\n"
                  "Enter Selection: ");
          int selection = std::cin.get () - '0';
          std::cin.ignore (MAX_STRING_LENGTH, '\n');

          switch (selection)
            {
              case 1:
                return ParseType::Alpha;
              case 2:
                return ParseType::Digit;
              case 3:
                return ParseType::AlNum;
              case 4:
                return ParseType::Symbol;
              case 5:
                return ParseType::Ascii;
              default:
                printf ("Invalid selection\n");
            }
        }
    }

    int get_display_value ()
    {
      printf ("\nDisplay values as:\n"
              "\t1. Count\n"
              "\t2. Rank\n"
              "Enter Selection: ");

      int selection = std::cin.get () - '0';
      std::cin.ignore (cfa::MAX_STRING_LENGTH, '\n');
      return selection;
    }

    void run_test_program ()
    {
      header_prompt ();
      for (;;)
        {
          printf ("\nSelect test:\n"
                  "\t1. Read from file\n"
                  "\t2. Input text\n"
                  "\t3. Quit\n"
                  "Enter Selection: ");

          int selection = std::cin.get () - '0';
          std::cin.ignore (cfa::MAX_STRING_LENGTH, '\n');
          switch (selection)
            {
              case 1:
                test_file_read ();
              break;
              case 2:
                test_user_input ();
              break;
              case 3:
                return;
              default:
                printf ("Invalid selection\n");
              continue;
            }
          printf ("Press Enter to continue...");
          std::cin.get ();
          std::cin.clear ();
        }
    }

    void test_user_input ()
    {
      char input[MAX_STRING_LENGTH + 1];
      printf ("\n"
              "Enter text to analyze:\n");
      std::cin.getline (input, MAX_STRING_LENGTH, '\n');
      input[MAX_STRING_LENGTH] = '\0';
      std::string str (input);
      for (;;)
        {
          switch (get_display_value ())
            {
              case 1:
                {
                  ParseType parse = get_parse_selection ();
                  auto count_vec = get_char_count_vec<int> (str, parse);
                  count_vec->sort (get_sort_method ());
                  print_char_count (count_vec);
                }
              return;
              case 2:
                {
                  ParseType parse = get_parse_selection ();
                  auto rank_vec = get_char_rank_vec (str, parse);
                  rank_vec->sort (get_sort_method ());
                  print_char_rank (rank_vec);
                }
              return;
              default:
                printf ("Invalid selection\n");
              continue;
            }
        }
    }

    void test_file_read ()
    {
      std::string filename = generate_test_file ();
      for (;;)
        {
          if (filename.empty ())
            {
              filename = get_filename ();
            }
          if (filename.size () == 1 && (char) std::toupper (filename.front ()) == 'Q')
            {
              return;
            }

          if (std::ifstream file = utils::file::get_file (filename); file.is_open ())
            {
              utils::file::trim_filename (filename);
              printf ("Opened '%s'\n", filename.c_str ());

              for (;;)
                {
                  switch (get_display_value ())
                    {
                      case 1:
                        {
                          ParseType parse = get_parse_selection ();
                          auto count_vec = get_char_count_vec<int> (file, parse);
                          count_vec->sort (get_sort_method ());
                          print_char_count (count_vec);
                        }
                      return;
                      case 2:
                        {
                          ParseType parse = get_parse_selection ();
                          auto rank_vec = get_char_rank_vec (file, parse);
                          rank_vec->sort (get_sort_method ());
                          print_char_rank (rank_vec);
                        }
                      return;
                      default:
                        printf ("Invalid selection\n");
                      continue;
                    }
                }
            }
        }
    }
}