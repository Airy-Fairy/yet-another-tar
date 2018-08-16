#include "parser.h"


Parser::Parser(int argc, char** argv)
{
    if (argc == 2)
    {
        if (strcmp(argv[1], "-h") == 0)
            m_action = HELP;
        else
            m_action = ERROR;
        return;
    }

    if (argc < 3)
    {
        m_action = TOO_FEW_ARGS;
        return;
    }

    if (strcmp(argv[1], "-a") == 0)
    {
        m_action = ARCHIVE;
    }
    else if (strcmp(argv[1], "-x") == 0)
    {
        m_action = EXTRACT;
    }
    else if (strcmp(argv[1], "-l") == 0)
    {
        m_action = LIST;
    }

    switch (m_action)
    {
    case ARCHIVE:
    case EXTRACT:
        m_inputPath = argv[2];
        if (argc == 5)
        {
            if (strcmp(argv[3], "-o"))
                m_action = ERROR;
            else
                m_outputPath = argv[4];
        }
        break;
    case LIST:
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
