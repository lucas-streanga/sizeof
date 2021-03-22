#include<filesystem>
#include<iostream>
#include<iomanip>
#include<string.h>

#define KILO 1024
#define MEGA 1048576
#define GIGA 1073741824

#if DEBUG == 1
#define LOG(x) std::clog << x << '\n'
#else
#define LOG(x)
#endif

#define MAX_RETRY 20

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

namespace fs = std::filesystem;

unsigned int g_tries;

byte_size_and_num_files find_recursive(const fs::path& path)
{
    g_tries++;
    if(g_tries > MAX_RETRY)
    {
      printf("Max retries (%u) reached at directory %s\n", MAX_RETRY, path.c_str());
      byte_size_and_num_files n;
      return n;
    }
    byte_size_and_num_files bsnf;
    fs::path pa;
    fs::path cur_directory;
    try
    {
      for(const auto& p: fs::recursive_directory_iterator(path, fs::directory_options::skip_permission_denied))
      {
          pa = p.path();

          try
          {
            if(fs::is_directory(p))
            {
              cur_directory = pa;
              continue;
            }
            if (fs::exists(p) && !fs::is_directory(p))
            {
              LOG(bsnf.files + 1 << ": " << pa);
              try
              {
                //dev/core will mess up file sized!
                if(fs::is_regular_file(fs::status(p)) && pa != "/dev/core" && pa != "/proc/kcore")
                {
                  bsnf.size += fs::file_size(p);
                  LOG(bsnf.size);
                }
                else
                  LOG("SKIPPED: size is not determinable: " << pa << "\n");
              }
              catch(fs::filesystem_error& e)
              {
                LOG("SKIPPED: size is not determinable: " << pa << "\n");
              }
              catch(std::bad_alloc&)
              {
                std::cout << "Allocation error. " << "\n";
                byte_size_and_num_files err;
                return err;
              }
              bsnf.files++;
            }
        }
        catch(fs::filesystem_error& e)
        {
            LOG("SKIPPED: size is not determinable: " << pa << "\n");
        }
      }
    }
    catch(fs::filesystem_error& e)
    {
      std::cout << "Unable to access file or path " << pa <<": " << e.what() << "\n";
      byte_size_and_num_files temp = find_recursive(cur_directory);
      bsnf.size += temp.size;
      bsnf.files += temp.files;
    }
    catch(std::bad_alloc&)
    {
      std::cout << "Allocation error. " << "\n";
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
  g_tries = 0;
  byte_size_and_num_files bsnf;

  fs::path p(argv[1]);

  if(fs::is_directory(p))
    bsnf = find_recursive(p);
  else
  {
    try
    {
      bsnf.size += fs::file_size(p);
    }
    catch(fs::filesystem_error& e)
    {
      LOG("SKIPPED: size is not determinable: " << argv[1] << "\n");
    }
  }

  if(bsnf.size == 0)
  {
    std::cout << "No size found!\n";
    return 0;
  }

  std::cout << std::setprecision(2) << std::fixed;

  if(bsnf.size > GIGA)
    std::cout << argv[1] << " is "  << ((double) bsnf.size) / ((double) GIGA) << " gigabytes";
  else if(bsnf.size > MEGA)
    std::cout << argv[1] << " is "  << ((double) bsnf.size) / ((double) MEGA) << " megabytes";
  else if(bsnf.size > KILO)
    std::cout << argv[1] << " is "  << ((double) bsnf.size) / ((double) KILO) << " kilobytes";
  else
    std::cout << argv[1] << " is "  << bsnf.size << " bytes";

  if(bsnf.files == 0)
    std::cout << ".\n";
  else
    std::cout << " with " << bsnf.files << " files.\n";

  return 0;
}
