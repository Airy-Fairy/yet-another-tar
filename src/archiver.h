#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <iostream>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

class Archiver
{
public:
    // MAYBE TEMP
    struct objInfo
    {
        std::string name;
        uintmax_t size;
        bool isDir;
    };

#ifdef WIN32
    using attr_t = unsigned long;
#elif __linux__
    using attr_t = unsigned int;
#endif

    Archiver();
    ~Archiver();

    int archive(const std::string& sInputPath, const std::string& sArchivePath);
    int extract(const std::string& sArchivePath, const std::string& sOutputPath) const;
    int list(const std::string& sArchivePath, std::vector<objInfo>& objList) const;
    int insert(const std::string& sInputPath, const std::string& sArchivePath);

private:
    void insertFile(std::ofstream& archiveStream, const fs::path& filePath) const;
    void insertDir(std::ofstream& archiveStream, const fs::path& dirPath) const;
    int getObjectAttributes(const char* path, attr_t* attributes) const;
    int setObjectAttributes(const char* path, const attr_t& attributes) const;
    bool isDir(const attr_t& attributes) const;

    // TODO: objList sorting function: folder1, it's files, folder2, it's files, etc.

    std::string m_sRootPath{};
    // static const int isDirFlag = 1; // TODO: Get rid of this crutch
};

#endif // ARCHIVER_H