#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <iostream>
#include <filesystem>

namespace fs = std::experimental::filesystem;

class Archiver
{

public:

    Archiver();
    ~Archiver();

    int archive(const std::string& sInputPath, const std::string& sArchivePath) const;
    int extract(const std::string& sArchivePath, const std::string& sOutputPath) const;
    int list(const std::string& sArchivePath) const;
    int insert(const std::string& sInputPath, const std::string& sArchivePath) const;

private:
    void insertFile(std::ofstream& archiveStream, const fs::path& filePath) const;
    void insertDir(std::ofstream& archiveStream, const fs::path& dirPath) const;

    static const int isDirFlag = 1;
};

#endif // ARCHIVER_H