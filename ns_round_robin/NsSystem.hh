#ifndef NS_SYSTEM_HH
#define NS_SYSTEM_HH

#include <unordered_map>
using std::unordered_map;

#include "fmt/format.h"
#include "Props.hh"
#include "Util.hh"

#include "NsLayer.hh"
#include "NsTract.hh"
#include "NsPattern.hh"


static const string hpcLayerId = "HPC";
static const string accLayerId = "ACC";
static const string sc0LayerId = "SC0";
static const string sc1LayerId = "SC1";

static const string hpcTractTypeId = "hpc";
static const string ncTractTypeId  = "nc";

static const string hpcLayerTypeId = "hpc";
static const string ncLayerTypeId  = "nc";

/**
 * Output trace message (without locator) if traceLevel is
 * INFO or lower
 */
template <typename... Args>
void infoTrace(const char *fmt, Args ... args)
{
    if (TRACE_INFO_IS_ON) {
        fmt::vprint(stdout, fmt, fmt::make_format_args(args...));
    }
}

/**
 * Output trace message (without locator) if traceLevel is
 * INFO1 or lower
 */
template <typename... Args>
void info1Trace(const char *fmt, Args ... args)
{
    if (TRACE_INFO1_IS_ON) {
        fmt::vprint(stdout, fmt, fmt::make_format_args(args...));
    }
}

/**
 * Output trace message (without locator) if traceLevel is
 * DEBUG or lower
 */
template <typename... Args>
void debugTrace(const char *fmt, Args ... args)
{
    if (TRACE_DEBUG_IS_ON) {
        fmt::vprint(stdout, fmt, fmt::make_format_args(args...));
    }
}


class NsSystem {
public:
    NsSystem() {}
    NsSystem(Props &props);

    void addLayer(const string &id, const string &type);
    void addTract(const string &fromLayerId, const string &toLayerId,
                  const string &type);
    /**
     * Add bidirectional tracts between two layers
     */
    void addBiTract(const string &layer1Id, const string &layer2Id,
                    const string &type);

   /**
    * Calculates rates for current timeStep value for all tracts
    */
    void calcRates();

    void acquire(uint numTrainCycles, const char *tag);
    void consolidate();
    void maintain();
    void clear();
    void setFrozen(const string &layerId, bool state);
    void lesion(const string &layerId);
    void settle();
    void runBackgroundProcesses();
    void retrieve(const string &cueLayerId, const string &patternId,
                  const string &tag);
    void train();
    void reactivate();
    void test(const string &cueLayerId, const string &patternId,
              const string &condition);
    void togglePsi(string layerId, bool state);
    static void printStateHdrs();
    void printState() const;
    void printGrids(const string &tag, const string &targetId = "") const;

    NsLayer *getLayer(const string &id) { return layers.at(id); }
    NsTract *getTract(const string &id) { return tracts.at(id); }

    void printSize();
    string toStr(uint iLvl = 0, const string &iStr = "   ") const;

    unordered_map<string, NsLayer *> layers;
    unordered_map<string, NsTract *> tracts;

    uint trainNumStimCycles;
    uint consNumStimCycles;
    uint reactNumStimCycles;
    uint numSettleCycles;
};

#endif
