#include <limits.h>

#include "MathUtil.hh"
#include "NsSystem.hh"
#include "NsConnection.hh"

/*
 * Constructor
 */
NsConnection::NsConnection(const NsTract *tract,
                           const NsUnit *fromUnit,
                           NsUnit *toUnit)
    : forceStaticInit(initializeStatics()),
      isPotentiated(false),
      fromUnit(fromUnit),
      toUnit(toUnit),
      tract(tract),
      id(fmt::format("{}-{}", fromUnit->id, toUnit->id)),
      psdSize(minPsdSize),
      numCiAmpars(minNumCiAmpars),
      numCpAmpars(minNumCpAmpars),
      psiIsOn(false)
{
    toUnit->inConnections.push_back(this);
}

/*
 * Static member variables
 */
double NsConnection::minPsdSize;
double NsConnection::maxPsdSize;
double NsConnection::minNumCiAmpars;
double NsConnection::minNumCpAmpars;
double NsConnection::potProbK;
double NsConnection::potProbHalf;

/**
 * Initialize static member variables This needs to happen before regular
 * member variables are initialized in the constructor, but can't be done by
 * static member initializers because it must happen after props have been
 * initialized, which happens in main().
 * 
 * Hence the 'forceStaticInit' member variable that forces this function to
 * be called before any other member variables are initialized.
 */
bool NsConnection::initializeStatics()
{
    static bool staticsInitialized = false;
    if (!staticsInitialized) {
        minPsdSize = props.getDouble("minPsdSize");
        maxPsdSize = props.getDouble("maxPsdSize");
        minNumCiAmpars = props.getDouble("minNumCiAmpars");
        minNumCpAmpars = props.getDouble("minNumCpAmpars");
        potProbK = props.getDouble("potProbK");
        potProbHalf = props.getDouble("potProbHalf");
        staticsInitialized = true;
    }
    return true;
}

/**
 * Set 'isPotentiated' = true. This will cause CI-AMPARs to move into slots
 * as they are vacated by decaying CP-AMPARs
 */
void NsConnection::potentiate(const char *tag)
{
    isPotentiated = true;

    infoTrace("{:.1f} potentiating {} ({}) [{}]\n",
              (double) simTime / 24.,
              id, tag, toUnit->lastNetInput);
}

/**
 * Turn off isPotentiated flag; this will cause CI-AMPARs
 * to start being removed;
 */
void NsConnection::depotentiate(const char *tag)
{
    isPotentiated = false;
    setNumCiAmpars(minNumCiAmpars);

    infoTrace("{:.1f} depotentiating {} ({}) [{}]\n",
              (double) simTime / 24.,
              id, tag, getStrength());
}

/**
 * Decay num CP-AMPARs no matter what. If the connection is potentiated AND
 * Hebbian, drive in CI-AMPARs, otherwise decay them towards their min and
 * shrink PSD.
 */
void NsConnection::amparTrafficking(double cpAmparRemovalRate, 
                                    double ciAmparInsertionRate,
                                    double ciAmparRemovalRate)
{
    setNumCpAmpars(numCpAmpars -
                   cpAmparRemovalRate * (numCpAmpars - minNumCpAmpars));

    if (isPotentiated && !psiIsOn) {
        if (fromUnit->isActive && toUnit->isActive) {
            double delta = Util::min(ciAmparInsertionRate,
                                     psdSize - (numCpAmpars + numCiAmpars));
            setNumCiAmpars(numCiAmpars + delta);
        }
    } else {
        // Constitutive CI-AMPAR removal
        //
        setNumCiAmpars(numCiAmpars -
                       ciAmparRemovalRate * (numCiAmpars - minNumCiAmpars));
    }

    // PSD size decays toward the greater of the number of inserted
    // AMPARs and minPsdSize
    //
    double asymptote =
        Util::max(numCpAmpars + numCiAmpars, minPsdSize);
    psdSize -= tract->psdDecayRate * (psdSize - asymptote);
}

/**
 * Remove all CI-AMPARs and replace them by CP-AMPARs
 */
void NsConnection::reactivate()
{
    // Rapid removal of CI-AMPARs
    //
    setNumCiAmpars(minNumCiAmpars);
 
    // Rapid replacement by CP-AMPARS
    //
    setNumCpAmpars(psdSize - numCiAmpars);
}

/**
 * 
 */
void NsConnection::stimulate(double learnRate, uint numStimCycles,
                             const char *tag)
{
    if (learnRate > 0) {
        learn(learnRate, numStimCycles, tag);
    }
}

/**
 * If the connection is in the Hebbian condition, grow the PSD in
 * accordance with the number of learning cycles specified and fill
 * vacant slots with CP-AMPARs. Then, with  a probability depending on
 * the number of learning cycles, potentiate the synapse.
 */
void NsConnection::learn(double learnRate, uint numStimCycles, const char *tag)
{
    if (fromUnit->isActive && toUnit->isActive) {
        for (uint i = 0; i < numStimCycles; i++) {
            psdSize += learnRate * (maxPsdSize - psdSize);
        }

        setNumCpAmpars(psdSize - numCiAmpars);
        
        if (!isPotentiated && !psiIsOn) {
            // Probability of potentiation is an asigmoid function of
            // stimulation level, reflecting a fuzzy threshold level e.g. of
            // accumulated kinase in a series of spikes.
            //
            // The level of stimulation is just numStimCycles
            //
            double probOfPotentiation =
                MathUtil::asigmoid(numStimCycles, potProbK, potProbHalf) *
                tract->maxPotProb;
            if (Util::randDouble(0.0, 1.0) < probOfPotentiation) {
                potentiate(tag);
            }
        }
    }
}

/*
 * Calculate strength as the number of inserted AMPARs divided by
 * maxPsdSize, the maximum number of AMPARs that can be inserted.
 * Thus, strength is a number in the range 0.0 to 1.0
 */
double  NsConnection::getStrength() const
{
    return (numCiAmpars + numCpAmpars) / 100 /*maxPsdSize*/;
}

void NsConnection::printStateHdr()
{
    infoTrace("time conn ID PSD-SIZE CI-AMPARS CP-AMPARS Potentiated Hebbian\n");
}

void NsConnection::printState() const
{
    infoTrace("{} conn {} {:.1f} {} {} {} {}\n",
               simTime / 24., id, 
              psdSize, numCiAmpars, numCpAmpars, isPotentiated, isHebbian());
}

string NsConnection::toStr(uint iLvl, const string &iStr) const
{
    return fmt::format("{}{} psd={} ci={} cp={}",
                       Util::repeatStr(iStr, iLvl),
                       id,
                       psdSize, numCiAmpars, numCpAmpars);
}
