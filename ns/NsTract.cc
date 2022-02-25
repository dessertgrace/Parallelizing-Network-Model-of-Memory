#include <limits.h>
#include <float.h>

#include "NsSystem.hh"
#include "NsTract.hh"


#define CHECK_RANGE(val, min, max) \
    ABORT_UNLESS(Util::isInRange(val, min, max), \
                 "bad value for '{}': {}", #val, val)


NsTract::NsTract(const string &id,
                 NsLayer *fromLayer,
                 NsLayer *toLayer,
                 const string &type)
    : id(id), type(type), fromLayer(fromLayer), toLayer(toLayer),
      e3Level(0), lastE3Level(DBL_MAX), lastTimeStep(UINT_MAX)
{
    acqLearnRate            = props.getDouble(type + '.' + "acqLearnRate");
    reactE3Level            = props.getDouble(type + '.' + "reactE3Level");
    consLearnRate01h        = props.getDouble(type + '.' + "consLearnRate01h");
    psdDecayRate01h         = props.getDouble(type + '.' + "psdDecayRate01h");
    cpAmparRemovalRate01h   = props.getDouble(type + '.' + "cpAmparRemovalRate01h");
    ciAmparInsertionRate01h = props.getDouble(type + '.' + "ciAmparInsertionRate01h");
    ciAmparRemovalRate01h   = props.getDouble(type + '.' + "ciAmparRemovalRate01h");
    baseDepotProb01h        = props.getDouble(type + '.' + "baseDepotProb01h");
    maxE3DepotProb01h       = props.getDouble(type + '.' + "maxE3DepotProb01h");
    e3DecayRate01h          = props.getDouble(type + '.' + "e3DecayRate01h");
    maxPotProb01h           = props.getDouble(type + '.' + "maxPotProb01h");
    
    
    // Sanity check
    //
    CHECK_RANGE(acqLearnRate,            0.0, 1.0);
    CHECK_RANGE(reactE3Level,            0.0, 1.0);
    CHECK_RANGE(consLearnRate01h,        0.0, 1.0);
    CHECK_RANGE(psdDecayRate01h,         0.0, 1.0);
    CHECK_RANGE(cpAmparRemovalRate01h,   0.0, 1.0);
    CHECK_RANGE(ciAmparRemovalRate01h,   0.0, 1.0);
    CHECK_RANGE(baseDepotProb01h,        0.0, 1.0);
    CHECK_RANGE(e3DecayRate01h,          0.0, 1.0);
    CHECK_RANGE(maxE3DepotProb01h,       0.0, 1.0);
    CHECK_RANGE(maxPotProb01h,           0.0, 1.0);

    for (auto fu : fromLayer->units) {
        for (auto tu : toLayer->units) {
            if (fu != tu) {
                connections.push_back(new NsConnection(this, fu, tu));
            }
        }
    }
}

/**
 * Given an exponential decay rate for some interval A, calculate the
 * equivalent rate for some other interval B.
 *
 * Exponential decay is: x(t+a) = (1 - rateA) * x(t)
 *
 * In "increasing exponential decay" the distance to some asymptote S decays
 * exponentially with rate (1 - rateA), so
 * 
 * A - x(t+a) = (1 - rateA) * (S - x(t))
 *
 * For both cases, rateB = 1 - (1 - rateA^(B/A))
 * 
 * "Increasing exponential decay" is also known as "exponential decay
 * (increasing form)" or "exponential decay (rising form)".
 */
static double calcExpDecayRate(double rateA, double intervalA, double intervalB)
{
    return 1.0 - pow(1.0 - rateA, intervalB/intervalA);
}

/**
 * Given a probability of an event happening during a some interval A,
 * calculate the equivalent probability for some other interval B.
 *
 * P(n) = 1 - (1 - P(A))^(B/A) 
 *
 * Note: this exactly the same as calcExpDecayRate, which is not
 *       surprising, since a constant probability of decay at the particle
 *       level translates to a constant *rate* of decay at the population
 *       level. 
 */
static double calcProb(double probA, double intervalA, double intervalB)
{
    return 1.0 - pow(1.0 - probA, intervalB/intervalA);
}

/**
 * Given a constant rate for some interval A, calculate the equivalent rate
 * for some other interval B.
 */
static double calcConstantRate(double rateA, double intervalA, double intervalB)
{
    return intervalB / intervalA * rateA;
}

/**
 * Calculates rates for the current timeStep value
 */
void NsTract::calcRates()
{
    consLearnRate        = calcExpDecayRate(consLearnRate01h, 1.0, timeStep);
    psdDecayRate         = calcExpDecayRate(psdDecayRate01h, 1.0, timeStep);
    cpAmparRemovalRate   = calcExpDecayRate(cpAmparRemovalRate01h, 1.0,
                                            timeStep);
    ciAmparInsertionRate = calcConstantRate(ciAmparInsertionRate01h, 1.0,
                                          timeStep);
    ciAmparRemovalRate   = calcExpDecayRate(ciAmparRemovalRate01h, 1.0,
                                            timeStep);
    baseDepotProb        = calcProb(baseDepotProb01h, 1.0, timeStep);
    e3DecayRate          = calcExpDecayRate(e3DecayRate01h, 1.0, timeStep);
    maxPotProb           = calcProb(maxPotProb01h, 1.0, timeStep);
    calcDepotProb();
}

/*
 * Calculate the total probability of depotentiation as the combination of
 * two independent probabilities: the constitutive depotentiation
 * (baseDepotProb) and depotentition due to the E3 enzyme (e3DepotProb)
 *
 * This function is called whenever e3Level or timeStep changes.
 */
inline void NsTract::calcDepotProb()
{
    // Don't waste time if nothing changed
    //
    if (e3Level != lastE3Level || timeStep != lastTimeStep) {
        double e3DepotProb01h = maxE3DepotProb01h * e3Level;
        e3DepotProb = calcProb(e3DepotProb01h, 1.0, timeStep);
        depotProb = baseDepotProb + e3DepotProb - baseDepotProb * e3DepotProb;

        ABORT_IF(depotProb > 1.0, "impossible");

        lastE3Level = e3Level;
        lastTimeStep = timeStep;
    }
}

void NsTract::stimulate(double learnRate, uint numStimCycles,
                        const char *tag)
{
    for (auto c : connections) {
        c->stimulate(learnRate, numStimCycles, tag);
    }
}

void NsTract::acquire(uint numStimCycles, const char *tag)
{
    stimulate(acqLearnRate, numStimCycles, tag);
}

void NsTract::consolidate(uint numStimCycles)
{
    stimulate(consLearnRate, numStimCycles, "cons");
}

void NsTract::amparTrafficking()
{
    for (auto c : connections) {
        c->amparTrafficking(cpAmparRemovalRate,
                            ciAmparInsertionRate,
                            ciAmparRemovalRate);
    }
}

/**
 * Randomly depotentiate some connections
 *
 */
void NsTract::depotentiateSome()
{
    for (auto c: connections) {
        if (c->isPotentiated && (Util::randDouble(0.0, 1.0) < depotProb)) {
            c->depotentiate("random");
        }
    }
}

/**
 * Run maintenance processes
 */
void NsTract::maintain()
{
    depotentiateSome();
    amparTrafficking();
    e3Level -= e3DecayRate * e3Level;
    
    // Recalculate depotentiation probability after updating E3 level
    //
    calcDepotProb();

    debugTrace("time: {} tract: {}  e3Level: {}  depotProb: {}\n",
               simTime, id, e3Level, depotProb);
}

/**
 * Toggle PSI on or off on all of the tract's connections
 * @param state State
 */
void NsTract::togglePsi(bool state)
{
    for (auto c: connections) {
        c->togglePsi(state);
    }
}

/**
 * Set E3 level and invoke reactivation processing in all connection that
 * are in the Hebbian condition, i.e. from-unit and to-unit are both active
 */
void NsTract::reactivate()
{
    // - Activate E3 enzyme. (E3 increases probability of depotentiation)
    //   TODO: should this be restricted to connections originating from or
    //   terminating on the units selected in makePattern below? i.e. units
    //   activated by reactivation.
    //
    e3Level = reactE3Level;
    calcDepotProb();

    for (auto c: connections) {
        if (c->fromUnit->isActive && c->toUnit->isActive) {
            c->reactivate();
        }
    }
}


/**
 * Count number of potentiated connections in the tract
 * @return The count
 */
uint NsTract::getNumPotentiated() const
{
    uint ret = 0;
    for (auto c: connections) {
        if (c->isPotentiated) ret++;
    }
    return ret;
}

/**
 * Print header line for the numPotentiate printouts
 */
void NsTract::printNumPotentiatedHdr()
{
    infoTrace("time tract id numPotentiated\n");
}

/**
 * Print number of potentiated connections in the tract
 */
void NsTract::printNumPotentiated() const
{
    infoTrace("{} tract {} {}\n", simTime, id, getNumPotentiated());
}

/**
 * Print the state of all of the tract's connections
 */
void NsTract::printState() const
{
    printNumPotentiated();
    for (auto c: connections) {
        c->printState();
    }
}

/**
 * Generate a string representation of the tract and all its connections
 */
string NsTract::toStr(uint iLvl, const string &iStr) const
{
    string ret = fmt::format("{}NsTract[{}]: ",
                             Util::repeatStr(iStr, iLvl), id);
    ret += fmt::format("\n{}acqLearnRate={}",
                       Util::repeatStr(iStr, iLvl + 1), acqLearnRate);
    ret += fmt::format("\n{}consLearnRate={}",
                       Util::repeatStr(iStr, iLvl + 1), consLearnRate);

    for (auto c: connections) {
        ret += "\n" + c->toStr(iLvl + 1, iStr);
    }
    return ret;
}
