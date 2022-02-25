/**
 * @file columns.cc
 *
 * Copy selected columns from input to output
 *
 * Author: Peter Helfer
 * Date: 2016-12-10
 */

#include <unistd.h>
#include <vector>
#include <string>
using std::string;
#include <fmt/format.h>
#include "Util.hh"

// A few abbreviations

const int NONE = Util::OPTARG_NONE;
const int INT  = Util::OPTARG_INT;
const int UINT = Util::OPTARG_UINT;
const int DBLE = Util::OPTARG_DBLE;
const int STR  = Util::OPTARG_STR;

bool   help            = false;

static const char *fname    = NULL;  // default is stdin
static const char *sepChars = " \t"; // input file separator chars
static const char *osep = "\t";      // output separator
static bool tabular = false;

/**
 * Print error message and exit
 * @param file File name
 * @param line Line number
 * @param format fmt::print format string
 * @param args Additional arguments to fmt::print
 */
template <typename... T>
void fail(string file, uint line, const char *format, const T & ... args)
{
    fmt::print(stderr, "File {}, line {}: ", file, line);
    fmt::print(stderr, format, args...);
    fmt::print(stderr, "\n");
    exit(1);
}

int main(int argc, char *argv[])
{
    char *pname = argv[0];

    // Process command line args:
    // 1. parse the options
    //
    std::vector<Util::ParseOptSpec> optSpecs = {
      { "file", STR,  &fname,    "file_name", "default: stdin" },
      { "sep",  STR,  &sepChars, "input_separator_chars", "default: \" \\t\""},
      { "osep", STR,  &osep,     "output_separator_string", "default: \"\\t\"" },
      { "t",    NONE, &tabular,  "", "tabular" },
      { "help", NONE, &help,     "", "" }};

    if (parseOpts(argc, argv, optSpecs) != 0 ||
        optind == argc ||
        help) 
    {
        std::vector<string> nonFlags = {
            "column_name [column_name ...] (or number)"
        };
        Util::usageExit(
            parseOptsUsage(
                pname, optSpecs, true,
                nonFlags).c_str(), NULL);
    }

    // 2. Remaining cmd line args are column specifications
    //
    std::vector<string> colSpecs;

    while (optind < argc) {
        colSpecs.push_back(argv[optind++]);
    }

    // Open the input file
    //
    FILE *fp;

    if (fname == NULL) {
        fp = stdin;
        fname = "<stdin>"; // for diagnostics only
    } else {
        fp = fopen(fname, "r");
        if (fp == NULL) {
            perror(fname);
            exit(errno);
        }
    } 
    
    // Parse the header line
    //
    const uint LINELEN = 2048;
    char line[LINELEN];
    uint lineNum = 1;
    if (fgets(line, LINELEN, fp) == NULL) {
        fmt::print(stderr, "{}: failed to read header line\n", fname);
        exit(errno);
    }
    Util::chop(line);
    string errMsg;
    std::vector<string> headers = Util::tokenize(line, sepChars, errMsg);
    if (!errMsg.empty()) {
        fail(fname, lineNum, "{}", errMsg);
    }

    // Determine which columns to copy
    //
    std::vector<uint> columnNumbers;
    for (auto colSpec : colSpecs) {
        // Try to convert colSpec to an int
        //
        char *p = NULL;
        uint colNum = strtol(colSpec.c_str(), &p, 10);

        if (*p == '\0') {
            // If colSpec is an int, take it as column number
            //
            if (colNum > 0 || colNum <= headers.size()) {
                columnNumbers.push_back(colNum - 1); // zero-based
            } else {
                fmt::print(stderr, "invalid column number: {}\n", colNum);
                exit(1);
            }
        } else {
            // Otherwise colSpec should be a column header
            uint i = 0;
            for (; i < headers.size(); i++) {
                if (Util::strCiEq(colSpec, headers[i])) {
                    columnNumbers.push_back(i);
                    break;
                }
            }
            if (i == headers.size()) {
                fail(fname, lineNum, "{}: column not found", colSpec);
            }
        }
    }

    // If tabular output was requested, then pipe through column(3)
    //
    FILE *outFile = stdout;
    if (tabular) {
        outFile = popen("column -t", "w");
        ABORT_IF(outFile == NULL, "Ouch!");
    }

    // Copy the selected columns of the header line
    //
    for (uint i = 0; i < columnNumbers.size(); i++) {
        fmt::print(outFile, "{}", headers[columnNumbers[i]]);
        if (i < columnNumbers.size() - 1) {
            fmt::print(outFile, "{}", osep);
        }
    }
    fmt::print(outFile, "\n");
                   
    // Read the rest of the file and copy the
    // selected columns in the specified order
    //
    while (fgets(line, LINELEN, fp) != NULL) {
        Util::chop(line);
        lineNum++;
        std::vector<string> tokens = Util::tokenize(line, sepChars, errMsg);
        if (!errMsg.empty()) {
            fail(fname, lineNum, "{}", errMsg);
        }
        if (tokens.size() != headers.size()) {
            fail(fname, lineNum, "Expected {} columns, found {}",
                 headers.size(), tokens.size());
        }
        for (uint i = 0; i < columnNumbers.size(); i++) {
            fmt::print(outFile, "{}", tokens[columnNumbers[i]]);
            if (i < columnNumbers.size() - 1) {
                fmt::print(outFile, "{}", osep);
            }
        }
        fmt::print(outFile, "\n");
    }
}
