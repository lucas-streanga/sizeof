#include<stdint.h>
#include<filesystem>
#include<iostream>
#include<iomanip>

#define KILO 1024
#define MEGA 1048576
#define GIGA 1073741824

#define TERM_COLOR_LOG "\033[35m"
#define TERM_COLOR_RESET "\033[0m"

#if DEBUG == 1
#define LOG(x) std::clog << TERM_COLOR_LOG << x << TERM_COLOR_RESET << '\n'
#else
#define LOG(x)
#endif


struct byte_size_and_num_files
{
  uintmax_t size;
  uintmax_t files;

  byte_size_and_num_files()
  {
    size = 0;
    files = 0;
  }
};

byte_size_and_num_files find_recursive(const std::filesystem::path& path)
{
    byte_size_and_num_files bsnf;
    std::filesystem::path pa;
    try
    {
      for(const auto& p: std::filesystem::recursive_directory_iterator(path, std::filesystem::directory_options::skip_permission_denied))
      {
          pa = p.path();
          if (std::filesystem::exists(p) && !std::filesystem::is_directory(p))
          {
            LOG(bsnf.files + 1 << ": " << pa);
            try
            {
              if(std::filesystem::is_regular_file(pa))
              {
                bsnf.size += std::filesystem::file_size(p);
              }
              else
                std::cout << "SKIPPED: size is not determinable: " << pa << "\n";
            }
            catch(std::filesystem::filesystem_error& e)
            {
              std::cout << "SKIPPED: size is not determinable: " << pa << "\n";
            }
            catch(std::bad_alloc&)
            {
              std::cout << "Allocation error. Exiting..." << "\n";
              byte_size_and_num_files err;
              return err;
            }
            bsnf.files++;
          }
        }
    }
    catch(std::filesystem::filesystem_error& e)
    {
      std::cout << "Unable to access file or path " << pa <<": " << e.what() << "\n";
    }
    catch(std::bad_alloc&)
    {
      std::cout << "Allocation error. Exiting..." << "\n";
      byte_size_and_num_files err;
      return err;
    }
    return bsnf;
}

int main(int argc, char ** argv)
{
  if(argc < 2)
  {
    printf("No path or file specified...\n");
    return 0;
  }
  byte_size_and_num_files bsnf;

  std::filesystem::path p(argv[1]);

  if(std::filesystem::is_directory(p))
    bsnf = find_recursive(p);
  else
  {
    try
    {
      bsnf.size += std::filesystem::file_size(p);
    }
    catch(std::filesystem::filesystem_error& e)
    {
      std::cout << "SKIPPED: size is not determinable: " << argv[1] << "\n";
    }
  }

  if(bsnf.size == 0)
    return 0;

  std::cout << std::setprecision(2) << std::fixed;

  if(bsnf.size > GIGA)
    std::cout << argv[1] << " is "  << (double) (bsnf.size / GIGA) << " gigabytes";
  else if(bsnf.size > MEGA)
    std::cout << argv[1] << " is "  << (double) (bsnf.size / MEGA) << " megabytes";
  else if(bsnf.size > KILO)
    std::cout << argv[1] << " is "  << (double) (bsnf.size / KILO) << " kilobytes";
  else
    std::cout << argv[1] << " is "  << bsnf.size << " bytes";

  if(bsnf.files == 0)
    std::cout << ".\n";
  else
    std::cout << " with " << bsnf.files << " files.\n";

  return 0;
}
