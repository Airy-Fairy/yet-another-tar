#ifndef PARSER_H
#define PARSER_H

#include <iostream>

class Parser
{
public:
    enum class Action
    {
        Help,
        Archive,
        Extract,
        List,
        Insert,
        TooFewArgs,
        Error
    };

    Parser(int argc, char** argv);
    ~Parser();

    Action action() const;
    std::string getInputPath() const;
    std::string getOutputPath() const;

private:
    Action m_action;
    std::string m_inputPath;
    std::string m_outputPath;
};

#endif // PARSER_H