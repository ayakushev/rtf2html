#include <string>
#include <algorithm>
#include <vector>
#include <stack>
#include <iostream>
#include <stdlib.h>
#include <ctype.h>
#include <cctype>
#include <string.h>
#include "interfaces.h"
#include "rtf_reader.h"
#include "utils.h"

namespace SDK_RTF2HTML {

    unsigned hexToInt(char character)    {
        if ((character >= '0') && (character <= '9'))
            return character - '0';
        else if ((character >= 'a') && (character <= 'z'))
            return character - 'a' + 10;
        else if ((character >= 'A') && (character <= 'Z'))
            return character - 'A' + 10;
        else return 0;
    }


    RtfReader::RtfReader(std::istream& newStream, IWriter& newWriter) :
        style(), isDestinationGroupIgnorable(false)    {
        writer = &newWriter;
        stream = &newStream;
        tableStarted = false;
        initCommands();
    }

    RtfReader::~RtfReader()    {
    }

    void RtfReader::initCommands() {
        m_commands["fldrslt"] = [this](IWriter & writer, RtfStyle & style, int value) {}; 
        m_commands["fldinst"] = [this](IWriter & writer, RtfStyle & style, int value) { 
            this->commandFieldinst();
        };
        m_commands["par"] = [this](IWriter & writer, RtfStyle & style, int value) { 
            this->commandLineBreak();
        };
        m_commands["*"] = [this](IWriter & writer, RtfStyle & style, int value) { 
            this->commandSetDestinationGroupIgnorable();
        };
        m_commands["fonttbl"] = [this](IWriter & writer, RtfStyle & style, int value) { 
            this->commandIgnoreDestinationKeyword();
        };
        m_commands["pict"] = [this](IWriter & writer, RtfStyle & style, int value) { 
            this->commandIgnoreDestinationKeyword();
        };
        m_commands["stylesheet"] = [this](IWriter & writer, RtfStyle & style, int value) {
            this->commandIgnoreDestinationKeyword();
        };
        m_commands["info"] = [this](IWriter & writer, RtfStyle & style, int value) { 
            this->commandIgnoreDestinationKeyword();
        };
        m_commands["xe"] = [this](IWriter & writer, RtfStyle & style, int value) {
            this->commandIgnoreDestinationKeyword();
        };
        m_commands["header"] = [this](IWriter & writer, RtfStyle & style, int value) {
            this->commandIgnoreDestinationKeyword();
        };
        m_commands["footer"] = [this](IWriter & writer, RtfStyle & style, int value) {
            this->commandIgnoreDestinationKeyword();
        };
        m_commands["tc"] = [this](IWriter & writer, RtfStyle & style, int value) { 
            this->commandIgnoreDestinationKeyword();
        };
        m_commands["tcn"] = [this](IWriter & writer, RtfStyle & style, int value) {
            this->commandIgnoreDestinationKeyword();
        };
        m_commands["nonshppict"] = [this](IWriter & writer, RtfStyle & style, int value) { 
            this->commandIgnoreDestinationKeyword();
        };
        m_commands["colortbl"] = [this](IWriter & writer, RtfStyle & style, int value) { 
            this->commandColourTable();
        };
        m_commands["field"] = [this](IWriter & writer, RtfStyle & style, int value) {}; 
        m_commands["tab"] = [this](IWriter & writer, RtfStyle & style, int value) {
            this->commandTab();
        };
        m_commands["sbknone"] = [this](IWriter & writer, RtfStyle & style, int value) { 
            if (this->getRtfStyle().getSectionColumns())
                this->commandEndRow();
            else
                this->commandLineBreak();
        };
        m_commands["cols"] = [this](IWriter & writer, RtfStyle & style, int value) { 
            RtfStyle rtfStyle = this->getRtfStyle();
            rtfStyle.setSectionColumns(value);
            this->setRtfStyle(rtfStyle);
        };
        m_commands["sbkcol"] = [this](IWriter & writer, RtfStyle & style, int value) {
            this->commandEndCell();
        };
        m_commands["\\"] = [this](IWriter & writer, RtfStyle & style, int value) { 
            this->commandCharacter('\\');
        };
        m_commands["{"] = [this](IWriter & writer, RtfStyle & style, int value) {
            this->commandCharacter('{');
        };
        m_commands["}"] = [this](IWriter & writer, RtfStyle & style, int value) {
            this->commandCharacter('}');
        };
        m_commands[";"] = [this](IWriter & writer, RtfStyle & style, int value) { 
            this->commandCharacter(';');
        };
        m_commands["lquote"] = [this](IWriter & writer, RtfStyle & style, int value) { 
            this->commandCharacter((unsigned long)0x91);
        };
        m_commands["rquote"] = [this](IWriter & writer, RtfStyle & style, int value) { 
            this->commandCharacter((unsigned long)0x92);
        };
        m_commands["ldblquote"] = [this](IWriter & writer, RtfStyle & style, int value) { 
            this->commandCharacter((unsigned long)0x93);
        };
        m_commands["rdblquote"] = [this](IWriter & writer, RtfStyle & style, int value) {
            this->commandCharacter((unsigned long)0x94);
        };
        m_commands["bullet"] = [this](IWriter & writer, RtfStyle & style, int value) { 
            this->commandCharacter((unsigned long)0x95);
        };
        m_commands["endash"] = [this](IWriter & writer, RtfStyle & style, int value) { 
            this->commandCharacter((unsigned long)0x96);
        };
        m_commands["emdash"] = [this](IWriter & writer, RtfStyle & style, int value) { 
            this->commandCharacter((unsigned long)0x97);
        };
        m_commands["b"] = [this](IWriter & writer, RtfStyle & style, int value) {
            Style new_style = this->getStyle();
            if (value)
                new_style.setBold(true);
            else
                new_style.setBold(false);
            this->setStyle(new_style);
        };
        m_commands["i"] = [this](IWriter & writer, RtfStyle & style, int value) { 
            Style _style = this->getStyle();
            if (value)
                _style.setItalic(true);
            else
                _style.setItalic(false);
            this->setStyle(_style);
        };
        m_commands["u"] = [this](IWriter & writer, RtfStyle & style, int value) {
            this->commandUnicode(value);
        };
        m_commands["ul"] = [this](IWriter & writer, RtfStyle & _style, int value) { 
            Style style = this->getStyle();
            if (value)
                style.setUnderline(true);
            else
                style.setUnderline(false);
            this->setStyle(style);
        };
        m_commands["ulnone"] = [this](IWriter & writer, RtfStyle & _style, int value) { 
            Style style = this->getStyle();
            style.setUnderline(false);
            this->setStyle(style);
        };
        m_commands["plain"] = [this](IWriter & writer, RtfStyle & _style, int value) { 
            RtfStyle style = this->getRtfStyle();
            style.setPlain();
            this->setRtfStyle(style);
        };
        m_commands["ql"] = [this](IWriter & writer, RtfStyle & _style, int value) { 
            Style style = this->getStyle();
            if (value)
            {
                style.setAlign(LeftJustified);
            }
            else
            {
                style.setAlign(DefaultJustified);
            }
            this->setStyle(style);
        };
        m_commands["qr"] = [this](IWriter & writer, RtfStyle & _style, int value) {
            Style style = this->getStyle();
            if (value)
                style.setAlign(RightJustified);
            else
                style.setAlign(DefaultJustified);
            this->setStyle(style);
        };
        m_commands["qc"] = [this](IWriter & writer, RtfStyle & _style, int value) {
            Style style = this->getStyle();
            if (value)
                style.setAlign(CentreJustified);
            else
                style.setAlign(DefaultJustified);
            this->setStyle(style);
        };
        m_commands["intbl"] = [this](IWriter & writer, RtfStyle & style, int value) {
            RtfStyle rtfStyle = this->getRtfStyle();
            rtfStyle.setInTable(value);
            this->setRtfStyle(rtfStyle);
        };
        m_commands["row"] = [this](IWriter & writer, RtfStyle & style, int value) { 
            this->commandEndRow();
        };
        m_commands["cell"] = [this](IWriter & writer, RtfStyle & style, int value) {
            this->commandEndCell();
        };
        m_commands["pard"] = [this](IWriter & writer, RtfStyle & style, int value) {
            this->commandParagraphDefault();
        };
        m_commands["cf"] = [this](IWriter & writer, RtfStyle & _style, int value) { 
            RtfStyle style = this->getRtfStyle();
            style.setForegroundColour(value);
            this->setRtfStyle(style);
        };
    }

    bool RtfReader::processData(void)    {
        char character;
        stream->get(character);

        if (stream->good())
        {
            switch (character)
            {
            case '{':
                // push
                rtfStack.push(style);
                break;
            case '}':
                // pop
                if (rtfStack.size())
                {
                    style = rtfStack.top();
                    rtfStack.pop();
                    writer->setStyle(style.getStyle());
                }
                else
                {
                    // Mismatched closing brackets
                }
                break;

            case '\\':
                // command
                readCommand(character);
                break;
            case '\n':
            case '\r':
                break;
            default:
                // character
                flushTable();
                if (character >= 32)
                {
                    writer->writeChar(character);
                }
                break;
            }
            return true;
        }
        else  // End of file
            return false;
    }

    int RtfReader::getPercentComplete(void)    {
        return 0;
    }

    void RtfReader::readCommand(char inputCharacter)    {
        std::string inputString;
        const std::string escapedCharacters = "\\{}";
        char character;

        inputString.append(1, inputCharacter);

        // Command must be at least two characters long
        stream->get(character);
        //if \ is followed by space, ignore the symbol
        if (std::isspace(character)) {
            stream->putback(character);
            return;
        }
        if (stream->good())
        {
            inputString.append(1, character);

            // need to check for "\{ \} \\"


            if (escapedCharacters.find(character) != std::string::npos)
            {
                handleCommand(inputString);
                return;
            }
        }
        else
        {
            // Read failed
            return;
        }

        stream->get(character);
        while (stream->good())
        {
            if (inputString.compare("\\\'") == 0)
            {
                // Numeric representation of a character in hex
                inputString.append(1, character);
                stream->get(character);
                inputString.append(1, character);
                handleCommand(inputString);
                return;
            }
            else if (isdigit(character))
            {
                // this is the numeric parameter for the command
                // cannot be hex: decimal only
                while (isdigit(character) && stream->good())
                {
                    inputString.append(1, character);
                    stream->get(character);
                }

                if (!isspace(character))
                    stream->putback(character);

                handleCommand(inputString);
                return;
            }
            else
            {
                switch (character)
                {
                case '\\':
                {
                    //if \ is followed by space, ignore the symbol
                    char next_character = '\0';
                    stream->get(next_character);
                    stream->putback(next_character);
                    if (std::isspace(next_character))
                        return;

                    stream->putback(character);
                    handleCommand(inputString);
                    return;
                }
                case '{':
                case '}':
                case ';':
                {
                    stream->putback(character);
                    handleCommand(inputString);
                    return;
                }
                case ' ':
                case '\r':
                case '\n':
                {
                    handleCommand(inputString);
                    return;
                }
                default:
                    inputString.append(1, character);
                    break;
                }
            }
            stream->get(character);
        }
    }

    void RtfReader::handleCommand(std::string& inputString)    {
        using namespace std;

        if (inputString[1] == '\'')
        {
            unsigned int lsd = hexToInt(inputString[3]);
            unsigned int msd = hexToInt(inputString[2]);
            unsigned long value = (msd << 4) | lsd;

            commandCharacter(value);

            return;
        }

        string::size_type digitIndex = inputString.find_first_of("-0123456789", 0);
        string commandString;

        if (digitIndex > 0)
        {
            commandString = inputString.substr(1, digitIndex - 1);
        }
        else
        {
            commandString = inputString.substr(1);
        }

        int value;
        if (digitIndex >= inputString.length())
        {
            value = 1;
        }
        else
        {
            string valueString(inputString, digitIndex);
            value = atoi(valueString.c_str());
        }

        std::map< std::string, WriterFunctor>::iterator found = m_commands.find(commandString);
        if (found != m_commands.end())
        {
            isDestinationGroupIgnorable = false;
            found->second(*writer, this->style, value);
        }
        else if (isDestinationGroupIgnorable) {
            //Ignore current and nested groups
            commandIgnoreDestinationKeyword();
            isDestinationGroupIgnorable = false;
        }
        else
        {
            ; // log[LOG_INFO] << DEBUG_ID << "Command " << commandString << " not found\n";
        }

    }

    bool RtfReader::readFirstSymbolSequence(std::string & symbolsSeq)    {
        char character = '\0';
        symbolsSeq = "";

        do {
            stream->get(character);
        } while (std::isspace(character));

        do {
            symbolsSeq.append(1, character);
            stream->get(character);
        } while (!std::isspace(character)
            && character != '\\' && character != '}');

        stream->putback(character);
        return true;
    }

    void RtfReader::commandIgnoreDestinationKeyword(void)    {
        int level;
        char character;

        for (level = 1; level != 0 && stream->good();)
        {
            stream->get(character);

            if (character == '{')
            {
                level++;
            }
            else if (character == '}')
            {
                level--;
            }
        }
        stream->putback(character);
    }

    void RtfReader::commandSetDestinationGroupIgnorable(void)    {
        isDestinationGroupIgnorable = true;
    }

    void RtfReader::commandParagraphBreak(void)    {
        writer->writeBreak(ParagraphBreak);
    }

    void RtfReader::commandLineBreak(void)    {
        writer->writeBreak(LineBreak);
    }

    void RtfReader::commandCharacter(unsigned int character) {
        writer->writeChar(character);
    }

    void RtfReader::commandField(void)    {
        //<field>	'{' \field <fieldmod> ? <fieldinst> <fieldrslt> '}'
        //<fieldinst>	'{\*' \fldinst <fieldtype><para>+ \fldalt? <datafield>? <formfield>? '}'
    }

    void RtfReader::commandFieldinst(void)    {
        //<fieldinst>	'{\*' \fldinst <fieldtype><para>+\fldalt ? <datafield> ? <formfield> ? '}'
        char character = '\0';
        std::string fieldtype = "";
        int count = 0;
        do {
            stream->get(character);
            if (character == '{')
                ++count;
        } while (!isalpha(character));

        stream->putback(character);

        //read fieldtype
        readFirstSymbolSequence(fieldtype);

        //read parameter
        std::string parameter = "";
        readFirstSymbolSequence(parameter);

        do {
            stream->get(character);
        } while (character != '\\' && character != '}');

        stream->putback(character);

        //in case filed type is HYPERLINK
        if (fieldtype.compare("HYPERLINK") == 0) {
            //we want a style to be aplied to the next group
            //this specific case is encounted only when the command fldinst is processed 
            rtfStack.top().setHyperlink(parameter);

            style.setHyperlink(parameter);
            for (int i = 0; i < count; ++i) {
                rtfStack.push(style);
            }
        }
    }

    void RtfReader::commandParagraphDefault(void)    {
        style.setInTable(false);
        Style standardStyle = style.getStyle();
        standardStyle.setAlign(DefaultJustified);
        style.setStyle(standardStyle);
        writer->setStyle(standardStyle);
    }

    void RtfReader::commandTab(void)    {
        writer->writeTab();
    }

    void RtfReader::commandUnicode(int value)    {
        //(Word specification 2009)
        //This keyword is followed immediately by equivalent character(s) in ANSI representation.
        char character = '\0';

        //look for ascii representation
        do {
            stream->get(character);
        } while (character != '\'');

        stream->get(character); //hex value
        unsigned int lsd = hexToInt(character);
        stream->get(character); //hex value

        unsigned int msd = hexToInt(character);
        unsigned long ascii_value = (msd << 4) | lsd;

        if (value < 0)
            value += 65536;
        if (value >= 65536) {
            writer->writeChar(ascii_value);
            return;
        }
        writer->writeChar((unsigned int)value);
        //ignore ascii, because unicode is supported
    }

    void RtfReader::commandInTable(void)    {
        // Note: RTF does not allow nested tables
        style.setInTable(true);
    }

    void RtfReader::flushTable(void)    {
        if (style.getInTable() || style.getSectionColumns())
        {
            if (!tableStarted)
            {
                // Beginning of table
                writer->writeTable(TableStart);
                tableStarted = true;

                writer->writeTable(TableRowStart);
                rowStarted = true;

                writer->writeTable(TableCellStart);
                cellStarted = true;
            }

            if (!rowStarted)
            {
                writer->writeTable(TableRowStart);
                rowStarted = true;
            }

            if (!cellStarted)
            {
                writer->writeTable(TableCellStart);
                cellStarted = true;
            }
        }
        else if (tableStarted)
        {
            writer->writeTable(TableEnd);
            tableStarted = false;
        }
    }

    void RtfReader::commandEndCell(void)    {
        if (cellStarted)
            writer->writeTable(TableCellEnd);

        cellStarted = false;
    }

    void RtfReader::commandEndRow(void)    {
        commandEndCell();

        if (rowStarted)
            writer->writeTable(TableRowEnd);

        rowStarted = false;
    }

    Style RtfReader::getStyle(void) const    {
        return style.getStyle();
    }

    void RtfReader::setStyle(const Style &value)    {
        style.setStyle(value);
        writer->setStyle(value);
    }

    void RtfReader::commandColourTable(void)    {
        int brackets = 1;
        char character;
        std::string inputString;

        stream->get(character);

        while (brackets && stream->good())
        {
            if (inputString.length() > 0)
            {
                switch (character)
                {
                case '\\':
                case '{':
                case '}':
                case ';':
                {
                    stream->putback(character);
                    // handle the colour commands
                    std::string::size_type digitIndex = inputString.find_first_of("-0123456789", 0);
                    std::string commandString(inputString, 1, digitIndex - 1);


                    int value;

                    if (digitIndex >= inputString.length())
                        value = 1;
                    else
                    {
                        std::string valueString(inputString, digitIndex);
                        value = atoi(valueString.c_str());
                    }

                    if (commandString == "red")
                    {
                        style.setRed(value);
                    }
                    else if (commandString == "green")
                    {
                        style.setGreen(value);
                    }
                    else if (commandString == "blue")
                    {
                        style.setBlue(value);
                    }

                    inputString.assign("");
                }
                break;

                default:
                    inputString.append(1, (char)character);
                    break;
                }
            }
            else
            {
                switch (character)
                {
                case '{':
                    brackets++;
                    break;

                case '}':
                    brackets--;
                    break;

                case ';':
                    style.colourTerminate();
                    break;

                case '\\':
                    inputString.append(1, (char)character);
                    break;
                }
            }
            if (brackets)
                stream->get(character);
        }
    }

    RtfStyle RtfReader::getRtfStyle(void) const     {
        return style;
    }

    void RtfReader::setRtfStyle(const RtfStyle &value)    {
        style = value;
        writer->setStyle(value.getStyle());
    }

} // namespace 
