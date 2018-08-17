#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <iostream>

class Archiver
{

public:

    Archiver();
    ~Archiver();

    int archive(const std::string& sInputPath, const std::string& sArchivePath) const;
    int extract(const std::string& sArchivePath, const std::string& sOutputPath) const;
    int list(const std::string& sArchivePath) const;

private:
    static const int isDirFlag = 1;
};

#endif // ARCHIVER_H