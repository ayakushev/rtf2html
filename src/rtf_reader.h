#ifndef __RTF_READER_H__
#define __RTF_READER_H__

#include "utils.h"
#include "html_writer.h"
#include <functional>
#include <map>

namespace RTF2HTML
{
class IWriter;
class RtfStyle;

typedef std::function<void(IWriter & writer, RtfStyle & style, int value)> WriterFunctor;

class IReader {
public:
    virtual bool processData(void) = 0;
    virtual ~IReader() {
    };
};


class RtfReader : public IReader
{

private:
  long fileLength;
  bool isDestinationGroupIgnorable;
  std::istream* stream;
  IWriter* writer;
  std::map< std::string, WriterFunctor> m_commands;
	RtfStyleStack rtfStack;
	RtfStyle style;
  bool tableStarted, rowStarted, cellStarted;

  void readCommand(char inputCharacter);
  void handleCommand(std::string& inputString);
  bool readFirstSymbolSequence(std::string& symbolsSeq);
  void initCommands();

public:
  void flushTable(void);
  RtfReader(std::istream& newStream, IWriter& newWriter);
  ~RtfReader();
  virtual bool processData(void);
  virtual int getPercentComplete(void);

  void commandParagraphBreak(void);
  void commandLineBreak(void);
  void commandIgnoreDestinationKeyword(void);
  void commandSetDestinationGroupIgnorable(void);
  void commandColourTable(void);
  void commandCharacter(unsigned int character);
  void commandField(void);
  void commandFieldinst(void);
  void commandParagraphDefault(void);
  void commandTab(void);
  void commandUnicode(int value);

  void commandInTable(void);
  void commandEndCell(void);
  void commandEndRow(void);

  Style getStyle(void) const;
  void setStyle( const Style &style );

	RtfStyle getRtfStyle(void) const;
  void setRtfStyle( const RtfStyle &style );
};

} // namespace

#endif

