#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <iostream>
#include <filesystem>

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

    Archiver();
    ~Archiver();

    int archive(const std::string& sInputPath, const std::string& sArchivePath);
    int extract(const std::string& sArchivePath, const std::string& sOutputPath) const;
    int list(const std::string& sArchivePath, std::vector<objInfo>& objList) const;
    int insert(const std::string& sInputPath, const std::string& sArchivePath);

private:
    void insertFile(std::ofstream& archiveStream, const fs::path& filePath) const;
    void insertDir(std::ofstream& archiveStream, const fs::path& dirPath) const;

    // TODO: objList sorting function: folder1, it's files, folder2, it's files, etc.

    std::string m_sRootPath{};
    static const int isDirFlag = 1;
};

#endif // ARCHIVER_H