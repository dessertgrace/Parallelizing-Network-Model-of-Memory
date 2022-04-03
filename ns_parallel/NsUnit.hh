#ifndef NS_UNIT_HH
#define NS_UNIT_HH

#include <stdlib.h>
#include <string>

using std::string;

class NsLayer;
class NsConnection;

class NsUnit {
public:
    NsUnit(const NsLayer *layer, uint index, uint gid);
    uint8_t activationFunction(double netInput);
    void computeNewActivation();
    void applyNewActivation();
    void setFrozen(bool state);
    void maintain();
    static void printStateHdr();
    void printState() const;
    string toStr(uint iLvl = 0, const string &iStr = "   ") const;

    const NsLayer *layer;
    const string id;
    const uint gid;
    double actFuncK;
    double actThreshold;
    bool isFrozen;
    uint8_t *isActive;
    uint8_t newIsActive;
    double lastNetInput;
    vector<NsConnection *> inConnections;
};

#endif
