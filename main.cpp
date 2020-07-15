
#include <sstream>

#include "rtf_reader.h"
#include "html_writer.h"

using namespace RTF2HTML;

int main(int argc, char **argv){
    if (argc != 3)
        return 0;
    std::string sdata;
    {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            fseek(f, 0, SEEK_END);
            int lSize = ftell(f);
            fseek(f, 0, SEEK_SET);
            sdata.resize(lSize);
            fread((void*)sdata.data(), 1, lSize, f);
            fclose(f);
        }
    }
    std::istringstream in(sdata);
    std::ostringstream out;


    HtmlWriter htmlWriter(out);
    IWriter* writer = &htmlWriter;
    RtfReader *rtfReader = new RtfReader(in, *writer);
    while (rtfReader->processData()) {    }

    std::string html = out.str();
    FILE *f = fopen(argv[2], "w");
    if (f) {
        fwrite(html.data(), 1, html.size(), f);
        fclose(f);
    }

	return 0;
}

