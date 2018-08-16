#include <iostream>

#include "parser.h"
#include "archiver.h"

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
    Parser parser(argc, argv);

    auto action = parser.action();

    if (action == Parser::Action::HELP)
    {
        cout << "Help message is here" << endl;
    }
    else if (action == Parser::Action::ARCHIVE)
    {
        auto inputPath = parser.getInputPath();
        auto outputPath = parser.getOutputPath();
        Archiver archiver;
        archiver.archive(inputPath, outputPath);
    }
    else if (action == Parser::Action::EXTRACT)
    {
        auto inputPath = parser.getInputPath();
        auto outputPath = parser.getOutputPath();
        Archiver archiver;
        archiver.extract(inputPath, outputPath);
    }
    else if (action == Parser::Action::LIST)
    {
        auto inputPath = parser.getInputPath();
        Archiver archiver;
        archiver.list(inputPath);
    }
    else if (action == Parser::Action::TOO_FEW_ARGS)
    {
        cout << "Error: Too few arguments!" << endl
            << "Usage: yat -axl input_path [-o output_path]" << endl
            << "To get help use: yat -h" << endl;
    }
    else if (action == Parser::Action::ERROR)
    {
        cout << "Error: unknown!" << endl << "Please, refer to help: yat -h";
    }

    system("pause");

    return 0;
}