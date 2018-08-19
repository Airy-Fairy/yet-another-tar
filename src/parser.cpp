#include "parser.h"


Parser::Parser(int argc, char** argv)
{
    // If there's 2 arguments then it's help option
    if (argc == 2)
    {
        if (strcmp(argv[1], "-h") == 0)
            m_action = Action::Help;
        else
            m_action = Action::Error;
        return;
    }

    // Not help option and args < 3 -> error
    if (argc < 3)
    {
        m_action = Action::TooFewArgs;
        return;
    }

    // Parse 2nd argument
    if (strcmp(argv[1], "-a") == 0)
    {
        m_action = Action::Archive;
    }
    else if (strcmp(argv[1], "-x") == 0)
    {
        m_action = Action::Extract;
    }
    else if (strcmp(argv[1], "-l") == 0)
    {
        m_action = Action::List;
    }
    else if (strcmp(argv[1], "-i") == 0)
    {
        m_action = Action::Insert;
    }
    else
    {
        m_action = Action::Error;
        return;
    }

    switch (m_action)
    {
    case Action::Archive:
    case Action::Extract:
    case Action::Insert:
        m_inputPath = argv[2];
        if (argc == 5)
        {
            if (strcmp(argv[3], "-o"))
                m_action = Action::Error;
            else
                m_outputPath = argv[4];
        }
        else
        {
            m_outputPath = ".";
        }
        break;
    case Action::List:
        m_inputPath = argv[2];
        break;
    default:
        break;
    }
}


Parser::~Parser()
{
}

Parser::Action Parser::action() const
{
    return m_action;
}

std::string Parser::getInputPath() const
{
    return m_inputPath;
}

std::string Parser::getOutputPath() const
{
    return m_outputPath;
}
