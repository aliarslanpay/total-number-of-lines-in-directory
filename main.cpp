#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <thread>
#include <mutex>


void numberOfLines(std::vector<std::string>& files, std::atomic<uint32_t>& lines, std::mutex& mtx)
{
    std::string file_name;
    std::ifstream ifs;

    while (!files.empty())
    {
        mtx.lock();
        file_name = files.back();
        files.pop_back();
        mtx.unlock();

        ifs.open(file_name);

        while (ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n'))
        {
            ++lines;
        }

        ifs.close();
    }
}

unsigned int line_counter(const std::string& path)
{
    unsigned int number_of_files{0};
    unsigned int number_of_threads = std::thread::hardware_concurrency();
    std::atomic<uint32_t> number_of_lines{0};
    std::vector<std::string> files{};
    std::vector<std::thread> threads{};
    std::mutex mtx{};

    for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(path))
    {
        if (entry.is_regular_file())
        {
            files.push_back(std::filesystem::absolute(entry.path()).string());
            ++number_of_files;
        }
    }

    number_of_threads = std::min(number_of_threads, number_of_files);

    threads.reserve(number_of_threads);
    for (int i{0}; i < number_of_threads; ++i)
    {
        threads.emplace_back(&numberOfLines, std::ref(files), std::ref(number_of_lines),
                                      std::ref(mtx));
    }

    for (std::thread& th : threads)
    {
        th.join();
    }

    return number_of_lines;
}

int main()
{
    std::string path;
    std::cout << "Please enter a directory: ";
    std::getline (std::cin, path);

    if (!std::filesystem::exists(path))
    {
        std::cout << "The directory does not exist!" << std::endl;
        return -1;
    }
    else if(std::filesystem::is_empty(path)){
        std::cout << "There is no file in the directory!" << std::endl;
        return -2;
    }

    std::cout << line_counter(path) << std::endl;

    return 0;
}