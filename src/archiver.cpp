#include "archiver.h"

#include <fstream>
#include <unordered_set>
#include <algorithm>

#ifdef WIN32
#include <Windows.h>
#elif __linux__
#include <sys/stat.h>
#endif


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
Archiver::ErrorCode Archiver::archive(const std::string & sInputPath, const std::string & sArchivePath)
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
    if (!fs::exists(inputPath)) {
        return ErrorCode::DoesNotExist;
    }

    if (!fs::exists(archivePath) && !fs::exists(archivePath.parent_path())) {
        return ErrorCode::DoesNotExist;
    }

    if (fs::is_directory(archivePath)) {
        archivePath = archivePath / inputPath.filename();
    }

    if (archivePath.extension() != fs::path(".yat")) {
        archivePath += ".yat";
    }

    std::ofstream archiveStream(archivePath, std::ios_base::binary);

    // Check stream
    if (!archiveStream.good()) {
        return ErrorCode::OFStreamErr;
    }

    std::cout << inputPath << std::endl;

    // Recursive in depth traverse of the input directory
    if (fs::is_directory(inputPath))
    {
        insertDir(archiveStream, inputPath);

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

    return ErrorCode::Success;
}

/**
 * @brief      Extracts data from archive sArchivePath
 *
 * @param[in]  sArchivePath  The archive path string
 * @param[in]  sOutputPath   The output path string
 *
 * @return     0 if success and error code otherwise
 */
Archiver::ErrorCode Archiver::extract(const std::string & sArchivePath, const std::string & sOutputPath) const
{
    auto archivePath = fs::path(sArchivePath);
    auto outputPath = fs::path(sOutputPath);

    // Check path existence
    if (!fs::exists(archivePath)) {
        return ErrorCode::DoesNotExist;
    }

    if (!fs::exists(outputPath))
    {
        if (!fs::create_directory(outputPath)) {
            return ErrorCode::CreateDirErr;
        }
    }

    std::ifstream archiveStream(archivePath, std::ios_base::binary);

    // Check stream
    if (!archiveStream.good()) {
        return ErrorCode::IFStreamErr;
    }

    while (archiveStream.peek() != EOF)
    {
        short nameLen;
        archiveStream.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));

        auto nameBuffer = new char[nameLen + 1];
        archiveStream.read(nameBuffer, nameLen);
        nameBuffer[nameLen] = '\0';

        attr_t attributes;
        archiveStream.read(reinterpret_cast<char*>(&attributes), sizeof(attributes));

        auto objPath = outputPath / fs::path(nameBuffer);

        // If it's a directory - create directories by objPath
        // else - read further for file content
        if (isDir(attributes))
        {
            fs::create_directories(objPath);
        }
        else
        {
            std::ofstream outputStream(objPath, std::ios_base::binary);

            uintmax_t fileSize;
            archiveStream.read(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));

            if (fileSize > 0)
            {
                auto fileBuffer = new char[fileSize];
                archiveStream.read(fileBuffer, fileSize);

                outputStream.write(fileBuffer, fileSize);

                delete[] fileBuffer;
            }

            outputStream.close();
        }

        // Restore attributes
        setObjectAttributes(objPath.string().c_str(), attributes);

        std::cout << objPath << std::endl;

        delete[] nameBuffer;
    }

    archiveStream.close();

    return ErrorCode::Success;
}

/**
 * @brief      { function_description }
 *
 * @param[in]  sArchivePath  The archive path string
 * @param      objList       The vector of objInfo structs, modified during execution
 *
 * @return     0 if success and error code otherwise
 */
Archiver::ErrorCode Archiver::list(const std::string & sArchivePath) const
{
    auto archivePath = fs::path(sArchivePath);

    // Check path existence
    if (!fs::exists(archivePath)) {
        return ErrorCode::DoesNotExist;
    }

    if (fs::is_directory(archivePath) || archivePath.extension() != fs::path(".yat")) {
        return ErrorCode::IsNotArchive;
    }

    std::ifstream archiveStream(archivePath, std::ios_base::binary);

    // Check stream
    if (!archiveStream.good()) {
        return ErrorCode::IFStreamErr;
    }

    std::vector<objInfo> objList;

    while (archiveStream.peek() != EOF)
    {
        objInfo object;
        object.size = -1;
        object.isDir = true;

        short nameLen;
        archiveStream.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));

        auto nameBuffer = new char[nameLen + 1];
        archiveStream.read(nameBuffer, nameLen);
        nameBuffer[nameLen] = '\0';

        attr_t attributes;
        archiveStream.read(reinterpret_cast<char*>(&attributes), sizeof(attributes));

        // If it's a file - skip its content
        if (!isDir(attributes))
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

    printObjList(objList);

    return ErrorCode::Success;
}

/**
 * @brief      Inserts new objects from sInputPath into archive sArchivePath
 *
 * @param[in]  sInputPath    The input path string
 * @param[in]  sArchivePath  The archive path string
 *
 * @return     0 if success and error code otherwise
 */
Archiver::ErrorCode Archiver::insert(const std::string & sInputPath, const std::string & sArchivePath)
{
    auto inputPath = fs::path(sInputPath);

    // Check path existence
    if (!fs::exists(inputPath)) {
        return ErrorCode::DoesNotExist;
    }

    if (!fs::exists(fs::path(sArchivePath))) {
        return ErrorCode::DoesNotExist;
    }
    
    if (sInputPath != ".") {
        if (inputPath.parent_path().string() != "")
            m_sRootPath = inputPath.parent_path().string() + "\\";
    }
    else {
        m_sRootPath = sInputPath;
    }

    std::ifstream oldArchiveStream(sArchivePath, std::ios_base::binary);
    std::ofstream newArchiveStream(sArchivePath + ".tmp", std::ios_base::binary | std::ios_base::app);

    // Check streams
    if (!oldArchiveStream.good()) {
        return ErrorCode::IFStreamErr;
    }

    if (!newArchiveStream.good()) {
        return ErrorCode::OFStreamErr;
    }

    // Saves new object relative paths
    std::unordered_set<std::string> newObjects;

    std::cout << inputPath << std::endl;

    // Recursive in depth traverse of the input directory
    if (fs::is_directory(inputPath))
    {
        insertDir(newArchiveStream, inputPath);

        for (auto& p : fs::recursive_directory_iterator(inputPath))
        {
            // objName <- relative path, i.e. without m_sRootPath
            auto sObjPath = p.path().string();
            auto objName = sObjPath.substr(m_sRootPath.length(), sObjPath.length() - m_sRootPath.length());

            std::cout << sObjPath << std::endl;

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

        auto nameBuffer = new char[nameLen + 1];
        oldArchiveStream.read(nameBuffer, nameLen);
        nameBuffer[nameLen] = '\0';

        attr_t attributes;
        oldArchiveStream.read(reinterpret_cast<char*>(&attributes), sizeof(attributes));

        // If current object doesn't contain in new objects - write it
        if (newObjects.find(nameBuffer) == newObjects.end())
        {
            newArchiveStream.write(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
            newArchiveStream.write(nameBuffer, nameLen);
            newArchiveStream.write(reinterpret_cast<char*>(&attributes), sizeof(attributes));

            // If it's not directory
            if (!isDir(attributes))
            {
                uintmax_t fileSize;
                oldArchiveStream.read(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
                newArchiveStream.write(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
                
                // If file is not empty - write it's content to the new archive
                if (fileSize > 0)
                {
                    auto fileBuffer = new char[fileSize];
                    oldArchiveStream.read(fileBuffer, fileSize);

                    newArchiveStream.write(fileBuffer, fileSize);

                    delete[] fileBuffer;
                }
            }
        }
        else
        {
            // If it's a file - skip its content
            if (!isDir(attributes))
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

    return ErrorCode::Success;
}

/**
 * @brief      Inserts a file into archive
 *
 * @param      archiveStream  The archive stream
 * @param[in]  filePath       The file path
 */
Archiver::ErrorCode Archiver::insertFile(std::ofstream & archiveStream, const fs::path & filePath) const
{
    std::ifstream inputStream(filePath, std::ios_base::binary);

    // Check stream
    if (!inputStream.good()) {
        return ErrorCode::IFStreamErr;
    }

    auto fileSize = fs::file_size(filePath);

    auto buffer = new char[fileSize];
    inputStream.read(buffer, fileSize);

    // fileName <- relative path, i.e. without m_sRootPath
    auto sFilePath = filePath.string();
    auto fileName = sFilePath.substr(m_sRootPath.length(), sFilePath.length() - m_sRootPath.length());

    auto nameLen = static_cast<short>(fileName.size());

    attr_t attributes = 0;
    getObjectAttributes(sFilePath.c_str(), &attributes);
    if (attributes == 0) {
        return ErrorCode::GetAttrsErr;
    }

    /*
    * The order of writing a file information is following:
    *   1. File name length
    *   2. File name
    *   3. File attributes
    *   4. File size
    *   5. File content
    */
    archiveStream.write(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    archiveStream.write(fileName.c_str(), fileName.size());
    archiveStream.write(reinterpret_cast<char*>(&attributes), sizeof(attributes));
    archiveStream.write(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
    archiveStream.write(buffer, fileSize);

    delete[] buffer;
    inputStream.close();

    return ErrorCode::Success;
}

/**
 * @brief      Inserts a directory into archive
 *
 * @param      archiveStream  The archive stream
 * @param[in]  dirPath        The directory path
 */
Archiver::ErrorCode Archiver::insertDir(std::ofstream & archiveStream, const fs::path & dirPath) const
{
    // dirName <- relative path, i.e. without m_sRootPath
    auto sDirPath = dirPath.string();
    auto dirName = sDirPath.substr(m_sRootPath.length(), sDirPath.length() - m_sRootPath.length());

    auto pathLen = static_cast<short>(dirName.size());

    attr_t attributes = 0;
    getObjectAttributes(sDirPath.c_str(), &attributes);
    if (attributes == 0) {
        return ErrorCode::GetAttrsErr;
    }

    /*
    * The order of writing a directory information is following:
    *   1. Directory name length
    *   2. Directory name
    *   3. Directory attributes
    */
    archiveStream.write(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));
    archiveStream.write(dirName.c_str(), dirName.size());
    archiveStream.write(reinterpret_cast<char*>(&attributes), sizeof(attributes));

    return ErrorCode::Success;
}

/**
 * @brief      Gets the object attributes.
 *
 * @param[in]  path  The path
 *
 * @return     The object attributes.
 */
void Archiver::getObjectAttributes(const char * path, attr_t* attributes) const
{
#ifdef WIN32
    auto ret = GetFileAttributes(path);
    if (ret != INVALID_FILE_ATTRIBUTES) {
        *attributes = GetFileAttributes(path);
    }
#elif __linux__
    struct stat fileStatus;
    auto ret = stat(path, &fileStatus);
    if (ret >= 0) {
        *attributes = fileStatus.st_mode;
    }
#endif
}

/**
 * @brief      Sets the object attributes.
 *
 * @param[in]  path        The path
 * @param[in]  attributes  The attributes
 *
 * @return     Returns the return value of corresponding attributes setting function
 */
int Archiver::setObjectAttributes(const char* path, const attr_t & attributes) const
{
#ifdef WIN32
    return SetFileAttributes(path, attributes);
#elif __linux__
    return chmod(path, attributes);
#endif
}

/**
 * @brief      Determines if an object defined by attributes is a directory.
 *
 * @param[in]  attributes  The attributes
 *
 * @return     True if dir, False otherwise.
 */
bool Archiver::isDir(const attr_t& attributes) const
{
    #ifdef _WIN32
        return (attributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
    #elif __linux__
        return static_cast<bool>(S_ISDIR(attributes));
    #endif
}

/**
 * @brief      Prints list of filesystem objects
 *
 * @param      objList  The object list
 */
void Archiver::printObjList(std::vector<objInfo>& objList) const
{
    std::sort(objList.begin(), objList.end(),
        [](const objInfo& a, const objInfo& b) {
        return a.name < b.name;
    });

    for (auto obj : objList)
    {
        if (obj.isDir)
            std::cout << "<DIR>\t" << obj.name << std::endl;
        else
            std::cout << obj.size << "\t" << obj.name << std::endl;
    }
}
