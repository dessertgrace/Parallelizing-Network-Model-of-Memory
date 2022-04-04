#ifndef NS_TRACT_HH
#define NS_TRACT_HH

#include <vector>
#include <string>
#include <functional>
using std::vector;
using std::string;

#include "NsLayer.hh"
#include "NsUnit.hh"
#include "NsConnection.hh"

class NsTract {
public:
    NsTract(const string &id,
            NsLayer *fromLayer, NsLayer *toLayer,
            const string &type);
    void depotentiateSome();
    void acquire(uint numStimCycles, const char *tag);
    void consolidate(uint numStimCycles);
    void calcRates();
    void stimulate(double learnRate, uint numStimCycles, const char *tag);
    void amparTrafficking();
    void maintain();
    void togglePsi(bool state);
    void reactivate();
    void calcDepotProb();
    uint getNumPotentiated() const;
    static void printNumPotentiatedHdr();
    void printNumPotentiated() const;
    void printState() const;

    string toStr(uint iLvl = 0, const string &iStr = "   ") const;

    string id;
    vector<NsConnection *> connections;
    string type;
    NsLayer *fromLayer;
    NsLayer *toLayer;

    double e3Level; // E3 enzyme level between 0.0 and 1.0
    double reactE3Level; // E3 level after reactivation

    // These are used to avoid useless calculations
    //
    double lastE3Level;
    uint   lastTimeStep;

    // Probability that any given potentiated connection in this tract will
    // depotentiate during a timeStep.
    //
    double depotProb;

    // Acquisition is simulated as a one-shot event, so not scaled to
    // timeStep.
    //
    double acqLearnRate;

    // These rates and probabilities are per timeStep. They are calculated
    // from the 01h rates, which are read from the property file
    //
    double consLearnRate;
    double psdDecayRate;
    double cpAmparRemovalRate;
    double ciAmparInsertionRate;
    double ciAmparRemovalRate;
    double baseDepotProb;
    double e3DepotProb;
    double e3DecayRate;
    double maxPotProb;

    // The 01h values for traffic rates and probabilities
    //
    double consLearnRate01h;
    double psdDecayRate01h;
    double cpAmparRemovalRate01h;
    double ciAmparInsertionRate01h;
    double ciAmparRemovalRate01h;
    double baseDepotProb01h;
    double maxE3DepotProb01h; // 01h prob of E3-induced depot when e3Level = 1.0
    double e3DepotProb01h;
    double e3DecayRate01h;
    double maxPotProb01h;
};

/**
 * Compare two tracts for equality (used by unordered_set<NsTract>)
 */
inline bool operator==(const NsTract &u1, const NsTract &u2)
{
    return (u1.id == u2.id);
}

#endif
