#include <iostream>

#include "parser.h"
#include "archiver.h"

using std::cout;
using std::endl;

int handleError(Archiver::ErrorCode errorCode);

int main(int argc, char** argv)
{
    Parser parser(argc, argv);

    auto action = parser.action();

    if (action == Parser::Action::Help)
    {
        cout << "YAT - Yet Another TAR\n"
            << "Available options:\n"
            << "  -a\tcollect input data and put it into yat archive\n"
            << "  -x\textract data from yat archive into output path\n"
            << "  -l\tlist yat archive data\n"
            << "  -i\tinsert data into existing yat archive\n"
            << "  -o\toutput path or yat archive path for inserting (-i)\n"
            << "  -h\tdisplay this help message" << endl;
    }
    else if (action == Parser::Action::Archive)
    {
        cout << "Archiving data..." << endl;
        auto inputPath = parser.getInputPath();
        auto archivePath = parser.getOutputPath();

        Archiver archiver;
        auto ret = archiver.archive(inputPath, archivePath);
        return handleError(ret);
    }
    else if (action == Parser::Action::Extract)
    {
        cout << "Extracting data..." << endl;
        auto archivePath = parser.getInputPath();
        auto outputPath = parser.getOutputPath();

        Archiver archiver;
        auto ret = archiver.extract(archivePath, outputPath);
        return handleError(ret);
    }
    else if (action == Parser::Action::List)
    {
        auto archivePath = parser.getInputPath();

        Archiver archiver;
        auto ret = archiver.list(archivePath);
        return handleError(ret);
    }
    else if (action == Parser::Action::Insert)
    {
        cout << "Inserting data..." << endl;
        auto inputPath = parser.getInputPath();
        auto archivePath = parser.getOutputPath();

        Archiver archiver;
        auto ret = archiver.insert(inputPath, archivePath);
        return handleError(ret);
    }
    else if (action == Parser::Action::TooFewArgs)
    {
        cout << "Error: Too few arguments!" << endl
            << "Usage: yat -axli (input_path | archive) [-o (output_path | archive)]" << endl
            << "To get help use: yat -h" << endl;
    }
    else if (action == Parser::Action::Error)
    {
        cout << "Parser error occured!" << endl << "Please, refer to help: yat -h" << endl;
    }

    return 0;
}

int handleError(Archiver::ErrorCode errorCode)
{
    switch (errorCode)
    {
    case Archiver::ErrorCode::DoesNotExist:
        cout << "File or directory doesn't exist!";
        break;
    case Archiver::ErrorCode::IsNotArchive:
        cout << "This is not a YAT archive!";
        break;
    case Archiver::ErrorCode::CreateDirErr:
        cout << "Couldn't create a directory!";
        break;
    case Archiver::ErrorCode::IFStreamErr:
        cout << "Input file stream error!";
        break;
    case Archiver::ErrorCode::OFStreamErr:
        cout << "Output file stream error!";
        break;
    case Archiver::ErrorCode::GetAttrsErr:
        cout << "An error occured while retrieving the attributes!";
        break;
    case Archiver::ErrorCode::SetAttrsErr:
        cout << "An error occured while setting the attributes!";
        break;
    default:
        break;
    }

    cout << endl;

    return errorCode == Archiver::ErrorCode::Success;
}
