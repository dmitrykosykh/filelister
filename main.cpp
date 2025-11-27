#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;

struct DirectoryInfo
{
    size_t fileCount;
    size_t directoryCount;
    uintmax_t totalSize;
};

DirectoryInfo getDirectoryInfo(const std::string &directoryPath)
{
    DirectoryInfo info = {0, 0, 0};

    try
    {
        for (const auto &entry : fs::recursive_directory_iterator(directoryPath))
        {
            if (entry.is_regular_file())
            {
                info.fileCount++;
                try
                {
                    info.totalSize += entry.file_size();
                }
                catch (const fs::filesystem_error &)
                {
                    // Ignore files that can't be accessed
                }
            }
            else if (entry.is_directory())
            {
                info.directoryCount++;
            }
        }
    }
    catch (const fs::filesystem_error &ex)
    {
        std::cerr << "Directory access error: " << ex.what() << std::endl;
    }

    return info;
}

std::vector<std::string> getFilesWithRelativePath(const std::string &directoryPath)
{
    std::vector<std::string> files;
    fs::path basePath = fs::absolute(directoryPath);

    // Сначала получаем общую информацию о директории
    DirectoryInfo info = getDirectoryInfo(directoryPath);
    size_t totalFiles = info.fileCount;
    size_t processedFiles = 0;

    std::cout << "Total files to process: " << totalFiles << std::endl;

    try
    {
        for (const auto &entry : fs::recursive_directory_iterator(directoryPath))
        {
            if (entry.is_regular_file())
            {
                fs::path relativePath = fs::relative(entry.path(), basePath);
                files.push_back(relativePath.string());

                processedFiles++;

                // Выводим прогресс каждые 100 файлов или для последнего файла
                if (processedFiles % 100 == 0 || processedFiles == totalFiles)
                {
                    size_t remainingFiles = totalFiles - processedFiles;
                    std::cout << "Processed: " << processedFiles << " files, Remaining: "
                              << remainingFiles << " files" << std::endl;
                }
            }
        }

        // Финальное сообщение о завершении
        std::cout << "Processing completed! Total files processed: " << processedFiles << std::endl;
    }
    catch (const fs::filesystem_error &ex)
    {
        std::cerr << "Directory access error: " << ex.what() << std::endl;
    }

    return files;
}

std::vector<std::string> getFilesWithRelativePathShallow(const std::string &directoryPath)
{
    std::vector<std::string> files;
    fs::path basePath = fs::absolute(directoryPath);

    try
    {
        for (const auto &entry : fs::directory_iterator(directoryPath))
        {
            if (entry.is_regular_file())
            {
                fs::path relativePath = fs::relative(entry.path(), basePath);
                files.push_back(relativePath.string());
            }
        }
    }
    catch (const fs::filesystem_error &ex)
    {
        std::cerr << "Directory access error: " << ex.what() << std::endl;
    }

    return files;
}

int main()
{
    std::string directoryPath;

    std::cout << "Enter directory path: ";
    std::getline(std::cin, directoryPath);

    if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath))
    {
        std::cerr << "Error: path does not exist or is not a directory!" << std::endl;
        return 1;
    }

    // Показываем общую информацию о директории
    DirectoryInfo info = getDirectoryInfo(directoryPath);
    std::cout << "\n=== Directory Information ===" << std::endl;
    std::cout << "Total files: " << info.fileCount << std::endl;
    std::cout << "Total directories: " << info.directoryCount << std::endl;
    std::cout << "Total size: " << info.totalSize << " bytes" << std::endl;
    std::cout << "=============================" << std::endl;

    std::cout << "\nSelect search mode:" << std::endl;
    std::cout << "1 - Current directory only" << std::endl;
    std::cout << "2 - Recursive (including subdirectories)" << std::endl;
    std::cout << "Your choice: ";

    int choice;
    std::cin >> choice;

    std::vector<std::string> files;

    if (choice == 1)
    {
        files = getFilesWithRelativePathShallow(directoryPath);
        std::cout << "\nFiles in current directory:" << std::endl;
    }
    else if (choice == 2)
    {
        files = getFilesWithRelativePath(directoryPath);
        std::cout << "\nFiles recursively (including subdirectories):" << std::endl;
    }
    else
    {
        std::cerr << "Invalid choice!" << std::endl;
        return 1;
    }

    if (files.empty())
    {
        std::cout << "No files found in the specified directory." << std::endl;
    }
    else
    {
        std::cout << "Found files (" << files.size() << "):" << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        for (const auto &file : files)
        {
            std::cout << file << std::endl;
        }
    }

    return 0;
}