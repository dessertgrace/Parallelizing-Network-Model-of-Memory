/**
 * @file Props.cc
 *
 * Implements property file handling
 *
 * Author: Peter Helfer
 * Date: 2012-02-09
 */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <libgen.h>
#include <string>
#include <fmt/format.h>
#include "Trace.hh"
#include "Util.hh"
#include "Props.hh"

/**
 * Remove comment, if present, from a line of text. A comment consists
 * of an unquoted # character and all that follows it.
 * @param line The line of text.
 * @param fname File name, used for error messaging only.
 * @param lineNum Line number, used for error messaging only.
 */
static void stripComment(char *line, const char  *fname, uint lineNum)
{
    char quoteChar = 0;
    for (char *p = line; *p != 0; p++) {
        if (quoteChar == '\\') {
            quoteChar = 0;
        } else if (*p == quoteChar) {
            quoteChar = 0;
        } else if (*p == '\\' || *p == '"' || *p == '\'') {
            quoteChar = *p;
        } else if (quoteChar == 0 && *p == '#') {
            *p = 0;
            return;
        }
    }
    if (quoteChar != 0) {
        fmt::print(stderr, "{}: line {} - Warning: unclosed quote: {}\n",
                fname, lineNum, line);
    }
}
        
// Replace occurrences of prop names with their values in s
//
string Props::substProps(string s)
{
    for (auto &prop : props) {
        for (size_t pos = s.find(prop.name);
             pos < string::npos;
             pos = s.find(prop.name, pos + 1))
        {
            s.replace(pos, prop.name.length(), prop.value);
            prop.used = true;
        }
    }
    return s;
}

void Props::fatal(const string &file, uint lineNum, string errMsg, string exhibit)
{
    TRACE_FATAL("{} line {} - {}: '{}'", file, lineNum, errMsg, exhibit);
}

void Props::fatal(Props::Prop *p, string errMsg)
{
    fatal(p->propsFile, p->lineNum, errMsg, p->value);
}

/**
 * Constructor
 */
Props::Props(const char *fname)
{
    readProps(fname);
}

void Props::readProps(const char *fname)
{
    FILE *fp = fopen(fname, "r");
    if (fp == NULL) {
        perror(fname);
        TRACE_FATAL("Failed to open {}\n", fname);
    }

    this->fname = fname;
    if (topLevelFname.empty()) {
        topLevelFname = fname;
    }

    char line[1000];

    for (uint lineNum = 1; fgets(line, sizeof(line), fp) != NULL; lineNum++) {
        stripComment(line, fname, lineNum);
        if (Util::isBlank(line)) continue;

        char *colonPos = strchr(line, ':');
        if (colonPos == NULL) {
            TRACE_FATAL("{}: line {} - Bad property (no colon): {}\n",
                        fname, lineNum, line);
        }

        // A double colon suppresses the "Duplicate property" warning
        // when overwriting aproperty's value
        //
        bool doubleColon = (*(colonPos + 1) == ':');
        
        string name = Util::wstrip(string(line).substr(0, colonPos - line));
        string value = Util::wstrip(doubleColon ? colonPos + 2 : colonPos + 1);

        if (Util::strCiEq(name, "include")) {
            // Include directive
            //
            string path = value;
            if (path[0] != '/') {
                // Relative path. Prepend directory path of current file.
                //
                char fnameCopy[strlen(fname)+1];
                strcpy(fnameCopy, fname);
                path = string(dirname(fnameCopy)) + '/' + path;
            }
            readProps(path.c_str());
        } else {
            // Ordinary prop
            //
            if (value.size() == 0) {
                TRACE_FATAL("{}: line {} - Bad property (no value): {}\n",
                            fname, lineNum, line);
            }

            value = substProps(value);

            Prop *p = findProp(name, false);
            if (p == NULL) {
                uint n = props.size();
                props.resize(n + 1);
                p = &props[n];
                p->name = name;
            } else if (!p->immutable && !doubleColon) {
                TRACE_WARN("{}: line {} - Duplicate property: {}\n",
                            fname, lineNum, name);
            }
            
            if (!p->immutable) {
                p->value = value;
                p->propsFile = fname;
                p->lineNum = lineNum;
            }
        }
    }
}

Props::Prop *Props::findProp(const string &name, bool required)
{
    for (uint i = 0; i < props.size(); i++) {
        if (Util::strCiEq(props[i].name, name)) {
            props[i].used = true;
            return &props[i];
        }
    }
    if (required) {
        TRACE_FATAL("Property '{}' not found in {}\n", name, topLevelFname);
    } else {
        return NULL;
    }
}

string Props::getString(const string &name, const string &defaultVal)
{
    Prop *p = findProp(name, false);

    if (p != NULL) {
        return (p->value);
    } else {
        return defaultVal;
    }
}

string  Props::getString(const string &name)
{
    return findProp(name, true)->value;
}

int Props::propToInt(Props::Prop *p)
{
    string errMsg;
    int val = Util::strToInt(p->value, errMsg);
    if (!errMsg.empty()) {
        fatal(p, errMsg);
    }
    return val;
}

int Props::getInt(const string &name, int defaultVal)
{
    Prop *p = findProp(name, false);
    if (p != NULL) {
        return propToInt(p);
    } else {
        return defaultVal;
    }
}

int Props::getInt(const string &name)
{
    return propToInt(findProp(name, true));
}

uint Props::propToUint(Props::Prop *p)
{
    string errMsg;
    uint val = Util::strToUint(p->value, errMsg);
    if (!errMsg.empty()) {
        fatal(p, errMsg);
    }
    return val;
}

uint Props::getUint(const string &name, uint defaultVal)
{
    Prop *p = findProp(name, false);
    if (p != NULL) {
        return propToUint(p);
    } else {
        return defaultVal;
    }
}

uint Props::getUint(const string &name)
{
    return propToUint(findProp(name, true));
}

double Props::propToDouble(Props::Prop *p)
{
    string errMsg;
    double val = Util::strToDouble(p->value, errMsg);
    if (!errMsg.empty()) {
        fatal(p, errMsg);
    }
    return val;
}

double Props::getDouble(const string &name, double defaultVal)
{
    Prop *p = findProp(name, false);
    if (p != NULL) {
        return propToDouble(p);
    } else {
        return defaultVal;
    }
}

double Props::getDouble(const string &name)
{
    return propToDouble(findProp(name, true));
}

bool Props::propToBool(Props::Prop *p)
{
    string errMsg;
    bool val = Util::strToBool(p->value, errMsg);
    if (!errMsg.empty()) {
        fatal(p, errMsg);
    }
    return val;
}

bool Props::getBool(const string &name, bool defaultVal)
{
    Prop *p = findProp(name, false);
    if (p != NULL) {
        return propToBool(p);
    } else {
        return defaultVal;
    }
}

bool Props::getBool(const string &name)
{
    return propToBool(findProp(name, true));
}

/**
 * Remove initial "{" and final "}" from a vector of tokens (destructive)
 * @param tokens
 */
void Props::removeBraces(std::vector<string> &tokens, Props::Prop *p)
{
    if (tokens[0] == "{" && tokens[tokens.size() - 1] == "}") {
        tokens.pop_back();
        tokens.erase(tokens.begin());
    }
    for (uint i = 0; i < tokens.size(); i++) {
        if(Util::strEq(tokens[i], "{") || Util::strEq(tokens[i], "}")) {
            fatal(p, "Unbalanced or misplaced brace");
        }
    }
}

std::vector<string> Props::propToStringVector(Props::Prop *p)
{
    char copy[p->value.size() + 1];
    strcpy(copy, p->value.c_str());

    std::vector<string> vals;

    // List of values may be enclosed in { }.
    // This makes it possible to specify an empty list.

    const char *errMsg = NULL;
    std::vector<string> tokens = 
        Util::tokenize(copy, " \t", errMsg, "", "{}");
    if (errMsg != NULL) {
        fatal(p, errMsg);
    }

    removeBraces(tokens, p);
    for (uint i = 0; i < tokens.size(); i++) {
        vals.push_back(tokens[i]);
    }
    return vals;
}

std::vector<string> Props::getStringVector(const string &name, const std::vector<string> defaultVal)
{
    Prop *p = findProp(name, false);
    if (p != NULL) {
        return propToStringVector(p);
    } else {
        return defaultVal;
    }
}

std::vector<string> Props::getStringVector(const string &name)
{
    return propToStringVector(findProp(name, true));
}

std::vector<int> Props::propToIntVector(Props::Prop *p)
{
    char copy[p->value.size() + 1];
    strcpy(copy, p->value.c_str());
    Prop dummyProp = *p;

    std::vector<int> vals;

    // List of values may be enclosed in { }.
    // This makes it possible to specify an empty list.

    const char *errMsg = NULL;
    std::vector<string> tokens = 
        Util::tokenize(copy, " \t", errMsg, "", "{}");
    if (errMsg != NULL) {
        fatal(p, errMsg);
    }

    removeBraces(tokens, p);
    for (uint i = 0; i < tokens.size(); i++) {
        dummyProp.value = tokens[i];
        vals.push_back(propToInt(&dummyProp));
    }
    return vals;
}

std::vector<int> Props::getIntVector(const string &name, const std::vector<int> defaultVal)
{
    Prop *p = findProp(name, false);
    if (p != NULL) {
        return propToIntVector(p);
    } else {
        return defaultVal;
    }
}

std::vector<int> Props::getIntVector(const string &name)
{
    return propToIntVector(findProp(name, true));
}

std::vector<uint> Props::propToUintVector(Props::Prop *p)
{
    char copy[p->value.size() + 1];
    strcpy(copy, p->value.c_str());
    Prop dummyProp = *p;

    std::vector<uint> vals;

    // List of values may be enclosed in { } as separate
    // tokens. This makes it possible to specify an empty list.

    const char *errMsg = NULL;
    std::vector<string> tokens = Util::tokenize(copy, " \t", errMsg);
    if (errMsg != NULL) {
        fatal(p, errMsg);
    }

    removeBraces(tokens, p);
    for (uint i = 0; i < tokens.size(); i++) {
        dummyProp.value = tokens[i];
        vals.push_back(propToUint(&dummyProp));
    }
    return vals;
}

std::vector<uint> Props::getUintVector(const string &name, const std::vector<uint> defaultVal)
{
    Prop *p = findProp(name, false);
    if (p != NULL) {
        return propToUintVector(p);
    } else {
        return defaultVal;
    }
}

std::vector<uint> Props::getUintVector(const string &name)
{
    return propToUintVector(findProp(name, true));
}

std::vector<double> Props::propToDoubleVector(Props::Prop *p)
{
    char copy[p->value.size() + 1];
    strcpy(copy, p->value.c_str());
    Prop dummyProp = *p;

    std::vector<double> vals;

    // List of values may be enclosed in { } as separate
    // tokens. This makes it possible to specify an empty list.

    const char *errMsg = NULL;
    std::vector<string> tokens = Util::tokenize(copy, " \t", errMsg);
    if (errMsg != NULL) {
        fatal(p, errMsg);
    }

    removeBraces(tokens, p);
    for (uint i = 0; i < tokens.size(); i++) {
        dummyProp.value = tokens[i];
        vals.push_back(propToDouble(&dummyProp));
    }
    return vals;
}

std::vector<double> Props::getDoubleVector(const string &name, const std::vector<double> defaultVal)
{
    Prop *p = findProp(name, false);
    if (p != NULL) {
        return propToDoubleVector(p);
    } else {
        return defaultVal;
    }
}

std::vector<double> Props::getDoubleVector(const string &name)
{
    return propToDoubleVector(findProp(name, true));
}

std::vector<std::vector<double> > Props::propToDoubleMatrix(Props::Prop *p)
{
    char copy[p->value.size() + 1];
    strcpy(copy, p->value.c_str());
    Prop dummyProp = *p;

    // The whole matrix and the individual row vectors must be
    // delimited by braces, like so:
    // { { 1.0 2.0 3.0 } { 4.0 5.0 6.0 } }
    // Furthermore, the row vectors must be of equal size.

    const char *errMsg = NULL;
    std::vector<string> tokens = 
        Util::tokenize(copy, " \t", errMsg, "", "{}");
    if(errMsg != NULL) {
        fatal(p, errMsg);
    }

    // strip the initial and final braces
    //
    if(tokens[0] != "{" || tokens[tokens.size() - 1] != "}") {
        fatal(p, "Matrix not brace-enclosed.");
    }
    tokens.pop_back();
    tokens.erase(tokens.begin());

    // Parse the row vectors
    //
    std::vector<std::vector<double> > vals;
    std::vector<double> row;
    enum TokenType { LBRACE, ELEM };
    TokenType expect = LBRACE;

    for (uint i = 0; i < tokens.size(); i++) {
        switch (expect) {
            case LBRACE:
                if(tokens[i] != "{") {
                    fatal(p->propsFile, p->lineNum, 
                          "Expected '{', got", tokens[i]);
                }
                expect = ELEM;
                break;
            case ELEM:
                if (tokens[i] == "{") {
                    fatal(p, "Expected element, got '{'");
                } else if (tokens[i] == "}") {
                    vals.push_back(row);
                    row.clear();
                    expect = LBRACE;
                } else {
                    dummyProp.value = tokens[i];
                    row.push_back(propToDouble(&dummyProp));
                }
                break;
            default:
                TRACE_FATAL("logic error");
        }
    }
    if(expect != LBRACE) {
        fatal(p, "unbalanced braces");
    }

    for (uint i = 1; i < vals.size(); i++) {
        if (vals[i].size() != vals[0].size()) {
            fatal(p, "rows don't have equal size.");
        }
    }
    return vals;
}

std::vector<std::vector<double> > Props::getDoubleMatrix(const string &name, const std::vector<std::vector<double> > defaultVal)
{
    Prop *p = findProp(name, false);
    if (p != NULL) {
        return propToDoubleMatrix(p);
    } else {
        return defaultVal;
    }
}

std::vector<std::vector<double> > Props::getDoubleMatrix(const string &name)
{
    return propToDoubleMatrix(findProp(name, true));
}

/**
 * Associate a name with a string value.
 * @param name The name
 * @param value The value
 * @param immutable If true, then subsequent attempts to change the value
 *        will have no effect. This is useful for setting properties by a
 *        command line option and not have them be overwritten by
 *        subsequent property file reading.
 */
void Props::setString(const string &name, string value, bool immutable)
{
    Prop *p = findProp(name, false);
    if (p == NULL) {
        uint n = props.size();
        props.resize(n + 1);
        p = &props[n];
        p->name = name;
    }
    if (!p->immutable) {
        p->value = value;
        p->lineNum = -1;
        p->immutable = immutable;
    }
}

// TODO: change all the buf and sprintf stuff to fmt::format

void Props::setInt(const string &name, int value)
{
    char buf[64];
    sprintf(buf, "%d", value);
    setString(name, buf);
}

void Props::setUint(const string &name, uint value)
{
    char buf[64];
    sprintf(buf, "%d", value);
    setString(name, buf);
}

void Props::setDouble(const string &name, double value)
{
    char buf[64];
    sprintf(buf, "%f", value);
    setString(name, buf);
}

void Props::setBool(const string &name, bool value)
{
    char buf[64];
    sprintf(buf, "%s", value ? "true" : "false");
    setString(name, buf);
}

void Props::setStringVector(const string &name, std::vector<string> v)
{
    char buf[1024];
    char buf1[32];
    sprintf(buf, "{ ");
    for (uint i = 0; i < v.size(); i++) {
        sprintf(buf1, "%s ", v[i].c_str());
        strcat(buf, buf1);
    }
    strcat(buf, " }");
    setString(name, buf);
}
    
void Props::setIntVector(const string &name, std::vector<int> v)
{
    char buf[1024];
    char buf1[32];
    sprintf(buf, "{ ");
    for (uint i = 0; i < v.size(); i++) {
        sprintf(buf1, "%d ", v[i]);
        strcat(buf, buf1);
    }
    strcat(buf, " }");
    setString(name, buf);
}
    
void Props::setUintVector(const string &name, std::vector<uint> v)
{
    char buf[1024];
    char buf1[32];
    sprintf(buf, "{ ");
    for (uint i = 0; i < v.size(); i++) {
        sprintf(buf1, "%u ", v[i]);
        strcat(buf, buf1);
    }
    strcat(buf, " }");
    setString(name, buf);
}
    
void Props::setDoubleVector(const string &name, std::vector<double> v)
{
    char buf[1024];
    char buf1[32];
    sprintf(buf, "{ ");
    for (uint i = 0; i < v.size(); i++) {
        sprintf(buf1, "%f ", v[i]);
        strcat(buf, buf1);
    }
    strcat(buf, " }");
    setString(name, buf);
}
    
void Props::setDoubleMatrix(const string &name, std::vector<std::vector<double> > m)
{
    char buf[1024];
    char buf1[32];
    sprintf(buf, "{ ");
    for (uint i = 0; i < m.size(); i++) {
        strcat(buf, "{ ");
        for (uint j = 0; j < m[i].size(); j++) {
            sprintf(buf1, "%f ", m[i][j]);
            strcat(buf, buf1);
        }
        strcat(buf, "} ");
    }
    strcat(buf, "}");
    setString(name, buf);
}

void Props::reportUnused(bool isFatal)
{
    bool someUnused = false;
    for (uint i = 0; i < props.size(); i++) {
        if (!props[i].used) {
            TRACE_WARN("{}: line {} - Unused property: {}",
                       props[i].propsFile, props[i].lineNum, props[i].name);
            someUnused = true;
        }
    }
    ABORT_IF(someUnused && isFatal, "Unused properties", 0);
}

string Props::toString()
{
    string s;
    
    for (uint i = 0; i < props.size(); i++) {
        s.append(props[i].name);
        s.append(": ");
        s.append(props[i].value);
        s.append("\n");
    }
    return s;
}

