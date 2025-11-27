#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>

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

bool writeFilesToOutput(const std::vector<std::string> &files, const std::string &outputFile)
{
    std::ofstream outFile(outputFile);
    if (!outFile.is_open())
    {
        std::cerr << "Error: Could not open output file: " << outputFile << std::endl;
        return false;
    }

    for (const auto &file : files)
    {
        outFile << file << std::endl;
    }

    outFile.close();
    return true;
}

int main(int argc, char *argv[])
{
    // Проверяем аргументы командной строки
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <directory_path> <output_file> [mode]" << std::endl;
        std::cerr << "Modes:" << std::endl;
        std::cerr << "  1 - Current directory only" << std::endl;
        std::cerr << "  2 - Recursive (including subdirectories) - DEFAULT" << std::endl;
        return 1;
    }

    std::string directoryPath = argv[1];
    std::string outputFile = argv[2];
    int choice = 2; // По умолчанию рекурсивный поиск

    // Если указан третий аргумент - используем его как выбор режима
    if (argc >= 4)
    {
        choice = std::stoi(argv[3]);
    }

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

    std::vector<std::string> files;

    if (choice == 1)
    {
        std::cout << "\nSearching in current directory only..." << std::endl;
        files = getFilesWithRelativePathShallow(directoryPath);
    }
    else if (choice == 2)
    {
        std::cout << "\nSearching recursively (including subdirectories)..." << std::endl;
        files = getFilesWithRelativePath(directoryPath);
    }
    else
    {
        std::cerr << "Invalid choice! Using default recursive mode." << std::endl;
        files = getFilesWithRelativePath(directoryPath);
    }

    // Записываем результаты в файл
    if (writeFilesToOutput(files, outputFile))
    {
        std::cout << "\nResults written to: " << outputFile << std::endl;
        std::cout << "Total files found: " << files.size() << std::endl;
    }
    else
    {
        std::cerr << "Failed to write results to file!" << std::endl;
        return 1;
    }

    return 0;
}