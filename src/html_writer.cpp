#include <string>
#include <iostream>
#include <ostream>
#include <ctype.h>

#include "interfaces.h"
#include "html_writer.h"

namespace SDK_RTF2HTML {

    bool isASCII(unsigned int ch) {
        return ch < 0x80;
    };

    char HexToChar(int value)     {
        if (value < 10)  {
            return (char)('0' + value);
        }
        else {
            return (char)('A' + value - 10);
        }
    }

    std::string HexToString(int value) {
        using namespace std;

        string temp;

        //temp.clear();

        temp = HexToChar((value >> 4) & 0x0f);
        temp += HexToChar(value & 0x0f);

        return temp;
    }

    HtmlWriter::HtmlWriter(std::ostream& stream)
        : m_outputStream(stream)     {
        m_outputStream << "<HTML>\r\n";
        m_outputStream << "<BODY>\r\n";
        m_styleChanged = false;
        m_whiteSpaces = 0;
    }

    void HtmlWriter::writeTab()   {
        m_outputStream << "&emsp;&emsp;";
    }

    HtmlWriter::~HtmlWriter()   {
        clearStyles();
        m_outputStream << "</BODY>\r\n";
        m_outputStream << "</HTML>\r\n";
    }

    void HtmlWriter::writeChar(unsigned int character) {
        if (m_styleChanged)
        {
            //clearStyles();
            outputStyles();
            m_styleChanged = false;
        }

        if (isASCII(character))
        {
            char ASCIICharacter = (char)character;
            if (isspace(ASCIICharacter))
            {
                if (m_whiteSpaces)
                    m_outputStream << "&nbsp;";
                else
                    m_outputStream << ASCIICharacter;

                m_whiteSpaces++;
            }
            else
            {
                m_whiteSpaces = 0;
                switch (ASCIICharacter)
                {
                case '&':
                    m_outputStream << "&amp;";
                    break;

                case '<':
                    m_outputStream << "&lt;";
                    break;

                case '>':
                    m_outputStream << "&gt;";
                    break;

                default:
                    m_outputStream << ASCIICharacter;
                    break;
                }
            }
        }
        else
        {
            m_whiteSpaces = 0;
            m_outputStream << "&#" << (unsigned int)character << ";";
        }
    }

    void HtmlWriter::writeTable(TableType table)  {
        switch (table)
        {
        case TableStart:
            tableStopStyle();
            m_outputStream << "<table>";
            break;

        case TableRowStart:
            m_outputStream << "<tr>";
            break;

        case TableRowEnd:
            m_outputStream << "</tr>\r\n";
            break;

        case TableCellStart:
            m_outputStream << "<td>";
            tableRestartStyle();
            break;

        case TableCellEnd:
            tableStopStyle();
            m_outputStream << "</td>";
            break;

        case TableEnd:
            m_outputStream << "</table>\r\n";
            tableRestartStyle();
            break;
        }
    }

    void HtmlWriter::writeBreak(BreakType type)     {
        if (type == LineBreak)
        {
            m_outputStream << "\r\n<br>";
        }
    }

    void HtmlWriter::setStyle(const Style &newStyle)    {
        if (!m_styleChanged)
            m_oldStyle = m_style;
        m_styleChanged = true;
        m_style = newStyle;
    }

    void HtmlWriter::outputStyles(void)    {
        std::string old_link = m_oldStyle.getHyperlink();
        std::string link = m_style.getHyperlink();

        if (link.empty() && !old_link.empty()) {
            m_outputStream << "</a>";
        }

        if ((!m_style.getBold()) && m_oldStyle.getBold())
        {
            m_outputStream << "</b>";
        }

        if ((!m_style.getItalic()) && m_oldStyle.getItalic())
        {
            m_outputStream << "</i>";
        }

        if ((!m_style.getUnderline()) && m_oldStyle.getUnderline())
        {
            m_outputStream << "</u>";
        }

        if (m_oldStyle.getAlign() != m_style.getAlign() &&
            m_oldStyle.getAlign() != DefaultJustified &&
            m_oldStyle.getAlign() != LeftJustified)
        {
            m_outputStream << "</div>";
        }

        if (!m_oldStyle.getDefaultColour() &&
            (
                m_style.getDefaultColour() ||
                (m_oldStyle.getColourRed() != m_style.getColourRed()) ||
                (m_oldStyle.getColourGreen() != m_style.getColourGreen()) ||
                (m_oldStyle.getColourBlue() != m_style.getColourBlue())
                ))
        {
            m_outputStream << "</font>";
        }


        if (!m_style.getDefaultColour())
        {

            if (m_oldStyle.getDefaultColour())
            {
                m_outputStream << "<font color=";
                m_outputStream << HexToString(m_style.getColourRed());
                m_outputStream << HexToString(m_style.getColourGreen());
                m_outputStream << HexToString(m_style.getColourBlue());
                m_outputStream << ">";
            }
            else if ((m_oldStyle.getColourRed() != m_style.getColourRed()) ||
                (m_oldStyle.getColourGreen() != m_style.getColourGreen()) ||
                (m_oldStyle.getColourBlue() != m_style.getColourBlue()))
            {
                m_outputStream << "<font color=";
                m_outputStream << HexToString(m_style.getColourRed());
                m_outputStream << HexToString(m_style.getColourGreen());
                m_outputStream << HexToString(m_style.getColourBlue());
                m_outputStream << ">";
            }

        }

        if (!link.empty() && old_link.empty()) {
            m_outputStream << "<a href=\"" + link + "\">";
        }

        if ((!m_oldStyle.getBold()) && m_style.getBold())
        {
            m_outputStream << "<b>";
        }

        if ((!m_oldStyle.getItalic()) && m_style.getItalic())
        {
            m_outputStream << "<i>";
        }

        if ((!m_oldStyle.getUnderline()) && m_style.getUnderline())
        {
            m_outputStream << "<u>";
        }

        Align oldJustification = m_oldStyle.getAlign();
        Align newJustification = m_style.getAlign();

        if (oldJustification == DefaultJustified)
        {
            oldJustification = LeftJustified;
        }

        if (newJustification == DefaultJustified)
        {
            newJustification = LeftJustified;
        }

        if (oldJustification != newJustification)
        {
            if (m_style.getAlign() == DefaultJustified ||
                m_style.getAlign() == LeftJustified)
            {

            }
            else if (m_style.getAlign() == RightJustified)
                m_outputStream << "<div align=right>";
            else if (m_style.getAlign() == CentreJustified)
            {
                m_outputStream << "<div align=center>";
            }
            else if (m_style.getAlign() == Justified)
                m_outputStream << "<div align=justify>";
        }
        m_styleChanged = false;
    }

    void HtmlWriter::clearStyles(void)
    {
        m_oldStyle = m_style;
        m_style.setDefault();
        outputStyles();
    }

    void HtmlWriter::tableStopStyle(void)
    {
        Style tempStyle(m_style);
        m_oldStyle = m_style;
        m_style.setDefault();
        outputStyles();
        m_style = tempStyle;
    }

    void HtmlWriter::tableRestartStyle(void)
    {
        m_oldStyle.setDefault();
        outputStyles();
    }

} // namespace
