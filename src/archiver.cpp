#include "archiver.h"

#include <fstream>
#include <unordered_set>


Archiver::Archiver()
{
}


Archiver::~Archiver()
{
}

/**
 * @brief      Archives files and directories from sInputPath
 *
 * @param[in]  sInputPath    The input path string
 * @param[in]  sArchivePath  The archive path string
 *
 * @return     0 if success and error code otherwise
 */
int Archiver::archive(const std::string & sInputPath, const std::string & sArchivePath)
{
    auto inputPath = fs::path(sInputPath);
    auto archivePath = fs::path(sArchivePath);
    
    if (sInputPath != ".") {
        if (inputPath.parent_path().string() != "")
            m_sRootPath = inputPath.parent_path().string() + "\\";
    }
    else {
        m_sRootPath = sInputPath;
    }

    // Check path existence
    if (!fs::exists(inputPath))
        return 1;

    if (fs::is_directory(archivePath)) {
        archivePath = archivePath / inputPath.filename();
    }

    if (archivePath.extension() != fs::path(".yat")) {
        archivePath += ".yat";
    }

    std::ofstream archiveStream(archivePath, std::ios_base::binary);

    if (fs::is_directory(archivePath)) {
        insertDir(archiveStream, inputPath);
    }

    // Recursive in depth traverse of the input directory
    if (fs::is_directory(inputPath))
    {
        for (auto& p : fs::recursive_directory_iterator(inputPath))
        {
            std::cout << p << std::endl;

            if (fs::is_directory(p))
            {
                insertDir(archiveStream, p.path());
            }
            else
            {
                insertFile(archiveStream, p.path());
            }
        }
    }
    else
    {
        insertFile(archiveStream, inputPath);
    }

    archiveStream.close();

    return 0;
}

/**
 * @brief      Extracts data from archive sArchivePath
 *
 * @param[in]  sArchivePath  The archive path string
 * @param[in]  sOutputPath   The output path string
 *
 * @return     0 if success and error code otherwise
 */
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

/**
 * @brief      { function_description }
 *
 * @param[in]  sArchivePath  The archive path string
 * @param      objList       The vector of objInfo structs, modified during execution
 *
 * @return     0 if success and error code otherwise
 */
int Archiver::list(const std::string & sArchivePath, std::vector<objInfo>& objList) const
{
    auto archivePath = fs::path(sArchivePath);

    // Check path existence
    if (!fs::exists(archivePath) || fs::is_directory(archivePath))
        return 1;

    std::ifstream archiveStream(archivePath, std::ios_base::binary);

    while (archiveStream.peek() != EOF)
    {
        objInfo object;
        object.size = -1;
        object.isDir = true;

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

            object.size = fileSize;
            object.isDir = false;
        }

        // Save object to the list
        object.name = nameBuffer;
        objList.push_back(object);

        delete[] nameBuffer;
    }

    archiveStream.close();

    return 0;
}

/**
 * @brief      Inserts new objects from sInputPath into archive sArchivePath
 *
 * @param[in]  sInputPath    The input path string
 * @param[in]  sArchivePath  The archive path string
 *
 * @return     0 if success and error code otherwise
 */
int Archiver::insert(const std::string & sInputPath, const std::string & sArchivePath)
{
    auto inputPath = fs::path(sInputPath);

    if (sInputPath != ".") {
        if (inputPath.parent_path().string() != "")
            m_sRootPath = inputPath.parent_path().string() + "\\";
    }
    else {
        m_sRootPath = sInputPath;
    }

    std::ifstream oldArchiveStream(sArchivePath, std::ios_base::binary);
    std::ofstream newArchiveStream(sArchivePath + ".tmp", std::ios_base::binary | std::ios_base::app);

    // Saves new object relative paths
    std::unordered_set<std::string> newObjects;

    // Recursive in depth traverse of the input directory
    if (fs::is_directory(inputPath))
    {
        for (auto& p : fs::recursive_directory_iterator(inputPath))
        {
            // objName <- relative path, i.e. without m_sRootPath
            auto sObjPath = p.path().string();
            auto objName = sObjPath.substr(m_sRootPath.length(), sObjPath.length() - m_sRootPath.length());

            if (fs::is_directory(p))
            {
                insertDir(newArchiveStream, p.path());
                newObjects.insert(objName);
            }
            else
            {
                insertFile(newArchiveStream, p.path());
                newObjects.insert(objName);
            }
        }
    }
    else
    {
        // fileName <- relative path, i.e. without m_sRootPath
        auto fileName = sInputPath.substr(m_sRootPath.length(), sInputPath.length() - m_sRootPath.length());

        insertFile(newArchiveStream, inputPath);
        newObjects.insert(fileName);
    }

    // Actual inserting
    while (oldArchiveStream.peek() != EOF)
    {
        short nameLen;
        oldArchiveStream.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));

        auto nameBuffer = new char[(nameLen >> 1) + 1];
        oldArchiveStream.read(nameBuffer, nameLen >> 1);
        nameBuffer[nameLen >> 1] = '\0';

        // If current object doesn't contain in new objects - write it
        if (newObjects.find(nameBuffer) == newObjects.end())
        {
            newArchiveStream.write(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            newArchiveStream.write(nameBuffer, nameLen >> 1);

            // If it's not directory
            if (!(nameLen & isDirFlag))
            {
                uintmax_t fileSize;
                oldArchiveStream.read(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
                newArchiveStream.write(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
                
                // If file is not empty - write it's content to the new archive
                if (fileSize > 0)
                {
                    auto fileBuffer = new char[fileSize + 1];
                    oldArchiveStream.read(fileBuffer, fileSize);
                    fileBuffer[fileSize] = '\0';

                    newArchiveStream.write(fileBuffer, fileSize);

                    delete[] fileBuffer;
                }
            }
        }
        else
        {
            // If it's a file - skip its content
            if (!(nameLen & isDirFlag))
            {
                uintmax_t fileSize;
                oldArchiveStream.read(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
                oldArchiveStream.seekg(fileSize, oldArchiveStream.cur);
            }
        }
    }

    oldArchiveStream.close();
    newArchiveStream.close();

    std::remove(sArchivePath.c_str());
    std::rename((sArchivePath + ".tmp").c_str(), sArchivePath.c_str());

    return 0;
}

/**
 * @brief      Inserts a file into archive
 *
 * @param      archiveStream  The archive stream
 * @param[in]  filePath       The file path
 */
void Archiver::insertFile(std::ofstream & archiveStream, const fs::path & filePath) const
{
    std::ifstream inputStream(filePath, std::ios_base::binary);

    auto fileSize = fs::file_size(filePath);

    auto buffer = new char[fileSize];
    inputStream.read(buffer, fileSize);

    // fileName <- relative path, i.e. without m_sRootPath
    auto sFilePath = filePath.string();
    auto fileName = sFilePath.substr(m_sRootPath.length(), sFilePath.length() - m_sRootPath.length());

    auto nameLen = static_cast<short>(fileName.size());
    nameLen = (nameLen << 1); // no flag

    archiveStream.write(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    archiveStream.write(fileName.c_str(), fileName.size());
    archiveStream.write(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
    archiveStream.write(buffer, fileSize);

    delete[] buffer;
    inputStream.close();
}

/**
 * @brief      Inserts a directory into archive
 *
 * @param      archiveStream  The archive stream
 * @param[in]  dirPath        The directory path
 */
void Archiver::insertDir(std::ofstream & archiveStream, const fs::path & dirPath) const
{
    // dirName <- relative path, i.e. without m_sRootPath
    auto sDirPath = dirPath.string();
    auto dirName = sDirPath.substr(m_sRootPath.length(), sDirPath.length() - m_sRootPath.length());

    auto pathLen = static_cast<short>(dirName.size());
    pathLen = (pathLen << 1) | isDirFlag; // is directory flag

    archiveStream.write(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));
    archiveStream.write(dirName.c_str(), dirName.size());
}
