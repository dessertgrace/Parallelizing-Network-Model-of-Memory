#ifndef NS_CONNECTION_HH
#define NS_CONNECTION_HH

#include <math.h>
#include "NsUnit.hh"
#include "NsGlobals.hh"

class NsTract;

class NsConnection {
private:
    bool forceStaticInit;
public:
    NsConnection(const NsTract *tract, const NsUnit *from, NsUnit *to);
    void stimulate(double learnRate, uint numStimCycles, const char *tag);
    void amparTrafficking(double cpAmparRemovalRate,
                          double ciAmparInsertionRate,
                          double ciAmparRemovalRate);

    static void printStateHdr();
    void depotentiate(const char *tag);
    void togglePsi(bool state) { psiIsOn = state; }
    void reactivate();
    double getStrength() const;
    void printState() const;
    string toStr(uint iLvl = 0, const string &iStr = "   ") const;
    bool isHebbian() const { return fromUnit->isActive && toUnit->isActive; }

    bool isPotentiated;
    const NsUnit *fromUnit;
    NsUnit *toUnit;

private:
    void potentiate(const char *tag);
    bool initializeStatics();

    void setNumCiAmpars(double n)
    {
        TRACE_DEBUG("simTime: {} {}.numCiAmpars {:5.2f} --> {:5.2f}\n",
                    simTime, id, numCiAmpars, n);
        ABORT_IF(n < minNumCiAmpars || isnan(n), "Oops");
        numCiAmpars = n;
    }
    
    void setNumCpAmpars(double n)
    {
        TRACE_DEBUG("simTime: {} {}.numCpAmpars {:5.2f} --> {:5.2f}\n",
                    simTime, id, numCpAmpars, n);
        ABORT_IF(n < minNumCpAmpars || isnan(n), "Oops");
        numCpAmpars = n;
    }
    
    void learn(double learnRate, uint numStimCycles, const char *tag);

    const  NsTract *tract;
    const  string  id;
    double         psdSize;
    double         numCiAmpars;
    double         numCpAmpars;
    bool           psiIsOn;
    bool           staticsInitialized;

    static double  minPsdSize;
    static double  maxPsdSize;
    static double  minNumCiAmpars;
    static double  minNumCpAmpars;
    static double  potProbK;    // K value for potentiation probability as
                                // function of numTrainCycles
    static double  potProbHalf; // numTrainCycles value at which probability
                                // of potentiation is 0.5

};

#endif
