#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <iostream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

class Archiver
{
public:
    enum class ErrorCode
    {
        Success,
        DoesNotExist,
        IsNotArchive,
        CreateDirErr,
        IFStreamErr,
        OFStreamErr,
        GetAttrsErr,
        SetAttrsErr
    };

#ifdef WIN32
    using attr_t = unsigned long;
#elif __linux__
    using attr_t = unsigned int;
#endif

    Archiver();
    ~Archiver();

    ErrorCode archive(const std::string& sInputPath, const std::string& sArchivePath);
    ErrorCode extract(const std::string& sArchivePath, const std::string& sOutputPath) const;
    ErrorCode list(const std::string& sArchivePath) const;
    ErrorCode insert(const std::string& sInputPath, const std::string& sArchivePath);

private:
    struct objInfo
    {
        std::string name;
        uintmax_t size;
        bool isDir;
    };

    ErrorCode insertFile(std::ofstream& archiveStream, const fs::path& filePath) const;
    ErrorCode insertDir(std::ofstream& archiveStream, const fs::path& dirPath) const;
    void getObjectAttributes(const char* path, attr_t* attributes) const;
    int setObjectAttributes(const char* path, const attr_t& attributes) const;
    bool isDir(const attr_t& attributes) const;
    void printObjList(std::vector<objInfo>& objList) const;

    std::string m_sRootPath{};
};

#endif // ARCHIVER_H