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
        auto outputPath = parser.getOutputPath();
        Archiver archiver;
        archiver.archive(inputPath, outputPath);
    }
    else if (action == Parser::Action::Extract)
    {
        cout << "Extracting data..." << endl;
        auto inputPath = parser.getInputPath();
        auto outputPath = parser.getOutputPath();
        Archiver archiver;
        archiver.extract(inputPath, outputPath);
    }
    else if (action == Parser::Action::List)
    {
        auto inputPath = parser.getInputPath();
        Archiver archiver;
        archiver.list(inputPath);
    }
    else if (action == Parser::Action::Insert)
    {
        // TODO
    }
    else if (action == Parser::Action::TooFewArgs)
    {
        cout << "Error: Too few arguments!" << endl
            << "Usage: yat -axli (input_path | archive) [-o (output_path | input_path)]" << endl
            << "To get help use: yat -h" << endl;
    }
    else if (action == Parser::Action::Error)
    {
        cout << "Error: unknown!" << endl << "Please, refer to help: yat -h";
    }

    return 0;
}