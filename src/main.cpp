#include <iostream>

#include "parser.h"
#include "archiver.h"

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
    Parser parser(argc, argv);

    auto action = parser.action();

    // TODO: handle errors

    if (action == Parser::Action::Help)
    {
        // TODO
        cout << "Help message is here" << endl;
    }
    else if (action == Parser::Action::Archive)
    {
        cout << "Archiving data..." << endl;
        auto inputPath = parser.getInputPath();
        auto archivePath = parser.getOutputPath();
        Archiver archiver;
        archiver.archive(inputPath, archivePath);
    }
    else if (action == Parser::Action::Extract)
    {
        cout << "Extracting data..." << endl;
        auto archivePath = parser.getInputPath();
        auto outputPath = parser.getOutputPath();
        Archiver archiver;
        archiver.extract(archivePath, outputPath);
    }
    else if (action == Parser::Action::List)
    {
        auto archivePath = parser.getInputPath();
        Archiver archiver;
        std::vector<Archiver::objInfo> objList;
        archiver.list(archivePath, objList);
        
        for (auto obj : objList)
        {
            if (obj.isDir)
                cout << "<DIR>\t" << obj.name << endl;
            else
                cout << obj.size << "\t" << obj.name << endl;
        }
    }
    else if (action == Parser::Action::Insert)
    {
        cout << "Inserting data..." << endl;
        auto inputPath = parser.getInputPath();
        auto archivePath = parser.getOutputPath();
        Archiver archiver;
        archiver.insert(inputPath, archivePath);
    }
    else if (action == Parser::Action::TooFewArgs)
    {
        cout << "Error: Too few arguments!" << endl
            << "Usage: yat -axli (input_path | archive) [-o (output_path | archive)]" << endl
            << "To get help use: yat -h" << endl;
    }
    else if (action == Parser::Action::Error)
    {
        cout << "Error: unknown!" << endl << "Please, refer to help: yat -h";
    }

    return 0;
}