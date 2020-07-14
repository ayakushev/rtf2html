#ifndef __RTFSTYLE_H__
#define __RTFSTYLE_H__

#include <stack>
#include <vector>
#include "interfaces.h"

namespace SDK_RTF2HTML {


    class Colour {
    public:
        Colour() {
            SetDefault();
        }

        void SetDefault() {
            redDefault = true;
            greenDefault = true;
            blueDefault = true;
        }

        void SetRed(int value) {
            redDefault = false;
            red = value;
        }

        void SetGreen(int value) {
            greenDefault = false;
            green = value;
        }

        void SetBlue(int value) {
            blueDefault = false;
            blue = value;
        }

        bool IsDefault() const {
            return redDefault || greenDefault || blueDefault;
        }

        int GetRed() const {
            return red;
        }

        int GetGreen() const {
            return green;
        }

        int GetBlue() const {
            return blue;
        }

        bool operator==(const Colour &rhs) const {
            return blue == rhs.blue && green == rhs.green && red == rhs.red;
        }

    private:
        bool redDefault, greenDefault, blueDefault;
        int red, green, blue;

    };

    enum  Align {
        DefaultJustified,
        LeftJustified,
        CentreJustified,
        RightJustified,
        Justified
    };


    class Style {

        Align m_align;
        bool m_bold;
        bool m_italic;
        bool m_underline;
        std::string m_hyperlink;
        bool m_defaultColour;
        Colour m_colour;

    public:
        Style() {
            setDefault();
        }


        Style(const Style &rhs) {
            m_align = rhs.m_align;
            m_bold = rhs.m_bold;
            m_italic = rhs.m_italic;
            m_underline = rhs.m_underline;
            m_hyperlink = rhs.m_hyperlink;

            m_colour = rhs.m_colour;
        }
        ~Style() {}

        Style& operator=(const Style& rhs) {
            if (this == &rhs)
                return *this;
            m_align = rhs.m_align;
            m_bold = rhs.m_bold;
            m_italic = rhs.m_italic;
            m_underline = rhs.m_underline;
            m_hyperlink = rhs.m_hyperlink;

            m_colour = rhs.m_colour;

            return *this;
        }


        bool operator==(const Style& rhs) const {
            return m_align == rhs.m_align && m_bold == rhs.m_bold &&
                m_italic == rhs.m_italic && m_underline == rhs.m_underline &&
                m_defaultColour == rhs.m_defaultColour &&
                (m_defaultColour || m_colour == rhs.m_colour) && !m_hyperlink.compare(rhs.m_hyperlink);
        }

        bool operator!=(const Style& rhs) const {
            return !(*this == rhs);
        }

        void setDefault(void) {
            m_hyperlink.clear();
            m_bold = false;
            m_italic = false;
            m_underline = false;
            m_align = DefaultJustified;
            m_colour.SetDefault();
        }

        void setHyperlink(const std::string & link) {
            m_hyperlink = link;
        }

        void setBold(bool value) {
            m_bold = value;
        }

        void setItalic(bool value) {
            m_italic = value;
        }

        void setUnderline(bool value) {
            m_underline = value;
        }

        void setAlign(Align value) {
            m_align = value;
        }

        void setColour(void) {
            m_colour.SetDefault();
        }

        void setColour(int redValue, int greenValue, int blueValue) {
            m_colour.SetRed(redValue);
            m_colour.SetGreen(greenValue);
            m_colour.SetBlue(blueValue);
        }

        std::string getHyperlink(void) const {
            return m_hyperlink;
        }

        bool getBold(void) const {
            return m_bold;
        }

        bool getItalic(void) const {
            return m_italic;
        }

        bool getUnderline(void) const {
            return m_underline;
        }

        Align getAlign(void) const {
            return m_align;
        }

        bool getDefaultColour(void) const {
            return m_colour.IsDefault();
        }

        int getColourRed(void) const {
            return m_colour.GetRed();
        }

        int getColourBlue(void) const {
            return m_colour.GetBlue();
        }

        int getColourGreen(void) const {
            return m_colour.GetGreen();
        }

    };

    enum FileFormat
    {
        FORMAT_UNKNOWN,
        FORMAT_RTF,
        FORMAT_HTML,
        FORMAT_TEXT
    };


    typedef enum {
        LineBreak,
        ParagraphBreak,
        PageBreak
    } BreakType;

    typedef enum {
        TableStart,
        TableCellStart,
        TableCellEnd,
        TableRowStart,
        TableRowEnd,
        TableEnd
    } TableType;


    class RtfStyle    {
        bool m_inTable;
        std::vector<Colour> m_colour;
        Colour m_currentColour;
        Style m_style;
        int m_sectionColumns;

    public:

        RtfStyle()         {
            setPlain();
            setSectionPlain();
        }

        virtual ~RtfStyle()        {

        }

        void setPlain(void)        {
            m_style.setBold(false);
            m_style.setItalic(false);
            m_style.setUnderline(false);
            m_style.setColour();
            m_inTable = false;
        }

        void setInTable(bool insideTable)        {
            m_inTable = insideTable;
        }

        bool getInTable()     {
            return m_inTable;
        }

        void setSectionPlain(void)        {
            m_sectionColumns = 0;
        }

        void setSectionColumns(int columns)        {
            m_sectionColumns = columns;
        }

        int getSectionColumns(void)        {
            return m_sectionColumns;
        }

        std::string getHyperlink()        {
            return m_style.getHyperlink();
        }

        void setRed(int value)        {
            m_currentColour.SetRed(value);
        }

        void setGreen(int value)        {
            m_currentColour.SetGreen(value);
        }

        void setBlue(int value)        {
            m_currentColour.SetBlue(value);
        }

        void setHyperlink(const std::string & link)        {
            m_style.setHyperlink(link);
        }

        void colourTerminate(void)        {
            m_colour.push_back(m_currentColour);
        }

        void setForegroundColour(int index)        {
            // Colour index is based on 1

            if (index < 0)
            {
                // integer index can't be compared to unsigned size if negative
                //    log[LOG_WARNING] << DEBUG_ID << "Index cannot be negative\n";
            }
            else if (unsigned(index) >= m_colour.size()) // cast to unsigned
            {
                //		log[LOG_WARNING] << DEBUG_ID << index << "is beyond bounds (" << 
                //      colour.size() << "\n";
            }
            else
            {
                if (m_colour[index].IsDefault())
                {
                    m_style.setColour();
                }
                else
                {
                    m_style.setColour(m_colour[index].GetRed(), m_colour[index].GetGreen(),
                        m_colour[index].GetBlue());
                }
            }
        }

        Style getStyle() const {
            return m_style;
        }
        void setStyle(const Style &newStyle) {
            m_style = newStyle;
        }
    };

    typedef std::stack<RtfStyle> RtfStyleStack;

} // namespace 

#endif
