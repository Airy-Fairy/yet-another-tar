#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <iostream>

class Archiver
{
public:
    Archiver();
    ~Archiver();

    int archive(const std::string& inputPath, const std::string& archivePath) const;
    int extract(const std::string& archivePath, const std::string& outputPath) const;
    int list(const std::string& archivePath) const;
};

#endif // ARCHIVER_H