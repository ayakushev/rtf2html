#ifndef __HTML_WRITER_H__
#define __HTML_WRITER_H__

#include "utils.h"

namespace RTF2HTML {

class IWriter {
public:
    virtual void writeTab() = 0;
    virtual void writeChar(unsigned int character) = 0;
    virtual void writeBreak(BreakType type) = 0;
    virtual void writeTable(TableType table) = 0;
    virtual void setStyle(const Style &newStyle) = 0;

    virtual ~IWriter() { };
};

class HtmlWriter : public IWriter {
private:
	void outputStyles( void );
	void clearStyles( void );
	void tableStopStyle( void );
	void tableRestartStyle( void );

public:
    HtmlWriter(std::ostream& stream);
    virtual void writeTab();
    virtual void writeChar(unsigned int character);
    virtual void writeTable(TableType table);
    virtual ~HtmlWriter();
    virtual void writeBreak(BreakType type);
    virtual void setStyle(const Style &newStyle);

protected:
    std::ostream&   m_outputStream;
    Style           m_style;
    Style           m_oldStyle;
    bool            m_styleChanged;
    int             m_whiteSpaces;
};

} // namespace 

#endif

