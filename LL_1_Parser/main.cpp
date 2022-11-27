#include "parser.h"

using namespace std;

const int NOFILE = -1;
const int WRFORMT = -2;
const int OK = 0;

void help() {
    fprintf(stderr,
        "usage: parser  [--help]\n"
        "               [--parse <inputfilename> <outputfilename>]\n");
}

void wrparanum() {
    cerr << "parser: wrong para num" << endl;
    help();
    exit(-1);
}

void wrpara() {
    cerr << "parser: wrong para" << endl;
    help();
    exit(-1);
}

int main(int argc,char* argv[])
{
    parser P;
    if (argc < 2) {
        wrparanum();
    }
    if (!strcmp(argv[1], "--help")) {
        help();
    }
    else if (!strcmp(argv[1], "--parse")) {
        if (argc != 4) {
            wrparanum();
        }
        else {
            switch (P.read_from_file(argv[2])) {
            case NOFILE:
                cerr << "read_from_file: No such inputfile" << endl;
                exit(-1);
            case WRFORMT:
                cerr << "read_from_file : Wrong inputfile format" << endl;
                exit(-1);
            case OK:
            default:
                break;
            }
            P.initTermchar();
            P.initParsingTable();
            switch (P.write_to_file(argv[3]))
            {
            case NOFILE:
                cerr << "write_to_file: Can not open outputfile" << endl;
                exit(-1);
            case OK:
            default:
                break;
            }
        }
    }
    else {
        wrpara();
    }


    
    return 0;
}
