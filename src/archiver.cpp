#include "archiver.h"

#include <fstream>

Archiver::Archiver()
{
}

Archiver::~Archiver()
{
}

int Archiver::archive(const std::string & sInputPath, const std::string & sArchivePath) const
{
    auto inputPath = fs::path(sInputPath);
    auto archivePath = fs::path(sArchivePath);

    // Check path existence
    if (!fs::exists(inputPath))
        return 1;
    
    //if (!fs::exists(archivePath) && !fs::exists(archivePath.parent_path()))
    //    return 1;

    if (fs::is_directory(archivePath))
    {
        archivePath = archivePath / inputPath.filename();
    }

    if (archivePath.extension() != fs::path(".yat"))
    {
        archivePath += ".yat";
    }

    std::ofstream archiveStream(archivePath, std::ios_base::binary);

    //auto saveFileToArchive = [&archiveStream](fs::path p)
    //{
    //    std::ifstream inputStream(p, std::ios_base::binary);

    //    auto fileSize = fs::file_size(p);

    //    auto buffer = new char[fileSize];
    //    inputStream.read(buffer, fileSize);

    //    auto fileName = p.string();
    //    auto nameLen = static_cast<short>(fileName.size());
    //    nameLen = (nameLen << 1); // no flag

    //    archiveStream.write(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    //    archiveStream.write(fileName.c_str(), fileName.size());
    //    archiveStream.write(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
    //    archiveStream.write(buffer, fileSize);

    //    delete[] buffer;
    //    inputStream.close();
    //};

    // Recursive in depth traverse of the input directory
    if (fs::is_directory(inputPath))
    {
        for (auto& p : fs::recursive_directory_iterator(inputPath))
        {
            std::cout << p << std::endl;

            if (fs::is_directory(p))
            {
                //auto dirName = p.path().string();
                //auto pathLen = static_cast<short>(dirName.size());
                //pathLen = (pathLen << 1) | isDirFlag; // is directory flag

                //archiveStream.write(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));
                //archiveStream.write(dirName.c_str(), dirName.size());
                insertDir(archiveStream, p.path());
            }
            else
            {
                insertFile(archiveStream, p.path());
                //saveFileToArchive(p.path());
            }
        }
    }
    else
    {
        insertFile(archiveStream, inputPath);
        //saveFileToArchive(inputPath);
    }

    archiveStream.close();

    return 0;
}

int Archiver::extract(const std::string & sArchivePath, const std::string & sOutputPath) const
{
    auto archivePath = fs::path(sArchivePath);
    auto outputPath = fs::path(sOutputPath);

    // Check path existence
    if (!fs::exists(archivePath))
        return 1;

    if (!fs::exists(outputPath))
    {
        if (!fs::create_directory(outputPath))
            return 1;
    }

    //if (outputPath.parent_path() != "" && !fs::exists(outputPath.parent_path()))
    //    return 1;

    std::ifstream archiveStream(archivePath, std::ios_base::binary);

    while (archiveStream.peek() != EOF)
    {
        short nameLen;
        archiveStream.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));

        auto nameBuffer = new char[(nameLen >> 1) + 1];
        archiveStream.read(nameBuffer, nameLen >> 1);
        nameBuffer[nameLen >> 1] = '\0';

        if (nameLen & isDirFlag)
        {
            fs::create_directories(outputPath / fs::path(nameBuffer));
        }
        else
        {
            std::ofstream outputStream(outputPath / fs::path(nameBuffer), std::ios_base::binary);

            uintmax_t fileSize;
            archiveStream.read(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));

            if (fileSize > 0)
            {
                auto fileBuffer = new char[fileSize + 1];
                archiveStream.read(fileBuffer, fileSize);
                fileBuffer[fileSize] = '\0';

                outputStream.write(fileBuffer, fileSize);

                delete[] fileBuffer;
            }

            outputStream.close();
        }

        std::cout << outputPath / fs::path(nameBuffer) << std::endl;

        delete[] nameBuffer;
    }

    archiveStream.close();

    return 0;
}

int Archiver::list(const std::string & sArchivePath) const
{
    auto archivePath = fs::path(sArchivePath);

    // Check path existence
    if (!fs::exists(archivePath) || fs::is_directory(archivePath))
        return 1;

    std::ifstream archiveStream(archivePath, std::ios_base::binary);

    while (archiveStream.peek() != EOF)
    {
        short nameLen;
        archiveStream.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));

        auto nameBuffer = new char[(nameLen >> 1) + 1];
        archiveStream.read(nameBuffer, nameLen >> 1);
        nameBuffer[nameLen >> 1] = '\0';

        // If it's a file - skip its content
        if (!(nameLen & isDirFlag))
        {
            uintmax_t fileSize;
            archiveStream.read(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
            archiveStream.seekg(fileSize, archiveStream.cur);

            std::cout << fileSize << "\t";
        }
        else
        {
            std::cout << "<DIR>\t";
        }

        std::cout << nameBuffer << std::endl;

        delete[] nameBuffer;
    }

    archiveStream.close();

    return 0;
}

int Archiver::insert(const std::string & sInputPath, const std::string & sArchivePath) const
{


    return 0;
}

void Archiver::insertFile(std::ofstream & archiveStream, const fs::path & filePath) const
{
    std::ifstream inputStream(filePath, std::ios_base::binary);

    auto fileSize = fs::file_size(filePath);

    auto buffer = new char[fileSize];
    inputStream.read(buffer, fileSize);

    auto fileName = filePath.string();
    auto nameLen = static_cast<short>(fileName.size());
    nameLen = (nameLen << 1); // no flag

    archiveStream.write(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    archiveStream.write(fileName.c_str(), fileName.size());
    archiveStream.write(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
    archiveStream.write(buffer, fileSize);

    delete[] buffer;
    inputStream.close();
}

void Archiver::insertDir(std::ofstream & archiveStream, const fs::path & dirPath) const
{
    auto dirName = dirPath.string();
    auto pathLen = static_cast<short>(dirName.size());
    pathLen = (pathLen << 1) | isDirFlag; // is directory flag

    archiveStream.write(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));
    archiveStream.write(dirName.c_str(), dirName.size());
}
