//---------------------------------------------------------------------------
#ifndef InterfaceH
#define InterfaceH

#include "utils.h"
#include <string>

namespace SDK_RTF2HTML {

class IWriter {
public:
  virtual void writeTab() = 0;
  virtual void writeChar(unsigned int character) = 0;
  virtual void writeBreak(BreakType type) = 0;
  virtual void writeTable(TableType table) = 0;
  virtual void setStyle( const Style &newStyle ) = 0;
  virtual ~IWriter() { };
};


class IReader {
public:
    virtual bool processData(void) = 0;
    virtual ~IReader() {
    };
};

} // namespace

//---------------------------------------------------------------------------
#endif
