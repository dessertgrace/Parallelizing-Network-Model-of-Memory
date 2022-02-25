/**
 * @file Props.hh
 *
 * Property file handling
 *
 * Author: Peter Helfer
 * Date: 2012-02-09
 */

#ifndef PROPS_HH
#define PROPS_HH

#include <string>
#include <vector>
using std::string;

class Props {
public:
    /**
     * Default constructor
     */
    Props() {}

    /**
     * Constructors
     */
    Props(const char *fname);
    Props(const string fname) : Props(fname.c_str()) {};

    /**
     * Read props from file
     * @param fname File name
     */
    void readProps(const char *fname);

    /**
     * Getters with default values
     */
    string  getString(const string &name, const string &defaultVal);
    int     getInt(const string &name, int defaultVal);
    uint    getUint(const string &name, uint defaultVal);
    double  getDouble(const string &name, double defaultVal);
    bool    getBool(const string &name, bool defaultVal);
    std::vector<string> getStringVector(const string &name, const std::vector<string> defaultVal);
    std::vector<int> getIntVector(const string &name, const std::vector<int> defaultVal);
    std::vector<uint> getUintVector(const string &name, const std::vector<uint> defaultVal);
    std::vector<double> getDoubleVector(const string &name, const std::vector<double> defaultVal);
    std::vector<std::vector<double> > getDoubleMatrix(const string &name, const std::vector<std::vector<double> > defaultVal);
    
    /**
     * Getters without defaults (abort if property not found)
     */
    string  getString(const string &name);
    int     getInt(const string &name);
    uint    getUint(const string &name);
    double  getDouble(const string &name);
    bool    getBool(const string &name);
    std::vector<string> getStringVector(const string &name);
    std::vector<int> getIntVector(const string &name);
    std::vector<uint> getUintVector(const string &name);
    std::vector<double> getDoubleVector(const string &name);
    std::vector<std::vector<double> > getDoubleMatrix(const string &name);

    /*
     * Setters - these will overwrite if prop exists, otherwise add
     */
    void setString(const string &name, string value, bool immutable = false);
    void setInt(const string &name, int value);
    void setUint(const string &name, uint value);
    void setDouble(const string &name, double value);
    void setBool(const string &name, bool value);
    void setStringVector(const string &name, std::vector<string> v);
    void setIntVector(const string &name, std::vector<int> v);
    void setUintVector(const string &name, std::vector<uint> v);
    void setDoubleVector(const string &name, std::vector<double> v);
    void setDoubleMatrix(const string &name, std::vector<std::vector<double> > m);

    /**
     * Return the prop array as a string of "name: value"
     * entries, separated by newlines.
     */
    string toString();

    /**
     * Print warning if any prop is unused.
     */
    void reportUnused(bool fatal);
    
private:
    string fname;
    string topLevelFname;
    
    struct Prop {
        string name;
        string value;
        string propsFile;
        int lineNum;
        bool immutable;
        bool used;

        Prop()
            : immutable(false), used(false)
        {}
    };
    
    std::vector<Prop> props;

    Prop *findProp(const string &name, bool required);
    int propToInt(Props::Prop *prop);
    uint propToUint(Props::Prop *prop);
    double propToDouble(Props::Prop *prop);
    bool propToBool(Props::Prop *prop);

    std::vector<string> propToStringVector(Props::Prop *prop);
    std::vector<int> propToIntVector(Props::Prop *prop);
    std::vector<uint> propToUintVector(Props::Prop *prop);
    std::vector<double> propToDoubleVector(Props::Prop *prop);
    std::vector<std::vector<double> > propToDoubleMatrix(Props::Prop *prop);

    string substProps(string s);
    static void removeBraces(std::vector<string> &tokens, Props::Prop *p);
    static void fatal(const string &file, uint lineNum, string errMsg, string exhibit);
    static void fatal(Props::Prop *p, string errMsg);
};


#endif // PROPS_HH
