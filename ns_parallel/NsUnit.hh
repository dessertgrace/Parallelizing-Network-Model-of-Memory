#ifndef NS_UNIT_HH
#define NS_UNIT_HH

#include <stdlib.h>
#include <string>

using std::string;

class NsLayer;
class NsConnection;

class NsUnit {
public:
    NsUnit(const NsLayer *layer, uint index);
    bool activationFunction(double netInput);
    void computeNewActivation();
    void applyNewActivation();
    void setFrozen(bool state);
    void maintain();
    static void printStateHdr();
    void printState() const;
    string toStr(uint iLvl = 0, const string &iStr = "   ") const;

    const NsLayer *layer;
    const string id;
    double actFuncK;
    double actThreshold;
    bool isFrozen;
    bool isActive;
    bool newIsActive;
    double lastNetInput;
    vector<NsConnection *> inConnections;
};

#endif
