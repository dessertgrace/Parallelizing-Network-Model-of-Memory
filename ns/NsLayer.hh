#ifndef NS_LAYER_HH
#define NS_LAYER_HH

#include <vector>
#include <string>
#include <functional>

using std::vector;
using std::string;

#include "NsPattern.hh"

#include "NsUnit.hh"

class NsLayer {
public:
    NsLayer(const string &id, const string &type);
    void makePattern(const string &patId);
    void setPattern(const string &patId);
    void setPattern(const NsPattern &pat);
    void clearPatterns();
    const string &setRandomPattern();
    void clear();
    void randomize();
    void computeNewActivations();
    void applyNewActivations();
    void adjustInhibition();
    void setFrozen(bool state);
    void lesion();
    void maintain();
    uint getNumActive() const;
    static void printScoreHdr();
    uint getNumHits(const string &targetId) const;
    static void printNumActiveHdr();
    void printNumActive() const;
    void printState() const;
    void printGrid(const string &tag, const string &targetId) const;

    void saveInhibition() { savedInhibition = inhibition; }
    void restoreInhibition() { inhibition = savedInhibition; }

    string toStr(uint iLvl = 0, const string &iStr = "   ") const;
    const string id;
    const string type;
    const uint width;
    const uint height;
    const double k;
    const double minInhibition;
    const double maxInhibition;
    const double initInhibition;
    const double inhibIncr;
    double inhibition;
    double savedInhibition;
    bool isClamped;
    bool isFrozen;
    bool isLesioned;
    vector<NsUnit *> units;
    bool orthogonalPatterns;
    uint nextPatternUnit;
    unordered_map<string, NsPattern> definedPatterns;
    vector<string> definedPatternIds;
    bool printPatterns;
};

#endif
