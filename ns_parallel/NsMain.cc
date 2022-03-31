#include <vector>
using std::vector;

#include <getopt.h>

#include "Util.hh"
#include "Sched.hh"

#include "NsSystem.hh"
#include "NsTract.hh"
#include "NsLayer.hh"

static NsSystem *nsSystem;

static uint stopTime;     // hours
static uint numBackgroundPatterns;

/**
 * Print the system size (number of units and connections) and the system
 * state if the debug tags "psize" and "psys" are set, respectively.
 */
static void printSystem()
{
    if (TTRACE_DEBUG_IS_ON("psize")) {
        nsSystem->printSize();
    }
    TTRACE_DEBUG("psys",
                 "\n------- System ------\n{}\n--------------------\n",
                 nsSystem->toStr(0, "   "));
}

/**
 * Build the system
 */
static void buildSystem()
{
    // Create the layers (units)
    nsSystem->addLayer(hpcLayerId, hpcLayerTypeId);
    nsSystem->addLayer(accLayerId, ncLayerTypeId);
    nsSystem->addLayer(sc0LayerId, ncLayerTypeId);
    nsSystem->addLayer(sc1LayerId, ncLayerTypeId);

    // Create the tracts (connections)
    nsSystem->addBiTract(hpcLayerId, accLayerId, hpcTractTypeId);
    nsSystem->addBiTract(hpcLayerId, sc0LayerId, hpcTractTypeId);
    nsSystem->addBiTract(hpcLayerId, sc1LayerId, hpcTractTypeId);
    nsSystem->addBiTract(accLayerId, sc0LayerId, ncTractTypeId);
    nsSystem->addBiTract(accLayerId, sc1LayerId, ncTractTypeId);
}

/**
 * Convert time in days::hours format to hours
 */
static uint dhToH(string dh)
{
    string errMsg;
    uint days = 0;
    uint hours = 0;
    vector<string> tokens = Util::tokenize(dh, ":", errMsg);
    if(tokens.size() == 1) {
        days = Util::strToUint(tokens[0], errMsg);
    } else if (tokens.size() == 2) {
        days = Util::strToUint(tokens[0], errMsg);
        hours = Util::strToUint(tokens[1], errMsg);
    } else {
        TRACE_FATAL("Bad days::hours string: {}", dh);
    }
    ABORT_IF(!errMsg.empty(), errMsg.c_str());
    return 24 * days + hours;
}

/**
 * struct used to schedule a time step change event
 */
struct TimeStepChangeData {
    uint timeStep;
};

/**
 * This function is called from the event scheduler.
 */
static void changeTimeStep(
    double stime, 
    double now, 
    TimeStepChangeData *data)
{
    TRACE_INFO("Changing time step to {}: scheduled time={} now={}",
               data->timeStep, stime, now);
    timeStep = data->timeStep;
    nsSystem->calcRates();
    delete data;
}

/**
 * struct used to schedule a reactivate event
 */
struct ReactivateData {
};

/**
 * Reactivate
 * This function is called from the event scheduler.
 * @param stime Scheduled time for this event
 * @param now Current time
 * @param data Reactivation data
 */
static void reactivate(
    double stime, 
    double now, 
    ReactivateData *data)
{
    TRACE_INFO("Reactivating: scheduled time={} now={}", stime, now);
    nsSystem->reactivate();
    delete data;
}

/**
 * struct used to schedule a freezing/unfreezing event
 */
struct FreezeData {
    FreezeData(const string &layerId, bool state)
        : layerId(layerId), state(state)
    {}
    const string &layerId;
    bool state;
};

/**
 * Freeze/unfreeze a layer
 * This function is called from the event scheduler.
 * @param stime Scheduled time for this event
 * @param now Current time
 * @param data Freeze data
 */
static void setFrozen(
    double stime, 
    double now, 
    FreezeData *data)
{
    TRACE_INFO("{} {}: scheduled time={} now={}",
               data->state ? "Freezing" : "Unfreezing",
               data->layerId, stime, now);

    nsSystem->setFrozen(data->layerId, data->state);

    delete data;
}

/**
 * struct used to schedule a lesioning event
 */
struct LesionData {
    LesionData(const string &layerId)
        : layerId(layerId)
    {}
    const string &layerId;
};

/**
 * Lesion a layer
 * This function is called from the event scheduler.
 * @param stime Scheduled time for this event
 * @param now Current time
 * @param data Lesion data
 */
static void lesion(
    double stime, 
    double now, 
    LesionData *data)
{
    TRACE_INFO("Lesioning {}: scheduled time={} now={}",
               data->layerId, stime, now);

    nsSystem->lesion(data->layerId);

    delete data;
}

/**
 * struct used to schedule Psi on/off event
 */
struct PsiData {
    string layerId;
    bool state;
    PsiData(string layerId, bool state)
        : layerId(layerId), state(state)
    {}
};

/**
 * Toggle PSI on or off in the specified layer
 * This function is called from the event scheduler.
 * @param stime Scheduled time for this event
 * @param now Current time
 * @param data PSI event data
 */
static void togglePsi(
    double stime, 
    double now, 
    PsiData *data)
{
    TRACE_INFO("{} PSI in {}: scheduled time={} now={}",
               (data->state ? "Starting" : "Stopping"),
               data->layerId, stime, now);
    nsSystem->togglePsi(data->layerId, data->state);
    delete data;
}

/**
 * Schedule events to toggle PSI on or off in a specified layer
 * @param layerId Layer ID
 * @param psiTimes Vector of times at which to toggle PSI on or off
 */
void schedulePsiEvents(const string &layerId, const vector<string> psiTimes)
{
    for (uint i = 0; i <  psiTimes.size(); i++) {
        PsiData *pd = new PsiData(layerId, Util::isEven(i));
        Sched::scheduleEvent(
            dhToH(psiTimes[i]),
            (Sched::VoidPtrCallback) togglePsi, pd);
    }
}

/**
 * Schedule events
 */
static void scheduleEvents()
{
    // Schedule timeStep changes
    //
    vector<string> timeStepChanges = 
        props.getStringVector("timeStepChanges", vector<string>());
    ABORT_IF(Util::isOdd(timeStepChanges.size()),
             "timeStepChanges must have even number of elements");
    for (uint i = 0; i <  timeStepChanges.size(); i += 2) {
        TimeStepChangeData *tscd = new TimeStepChangeData();
        tscd->timeStep = strtoul(timeStepChanges[i + 1].c_str(), NULL, 10);
        Sched::scheduleEvent(
            dhToH(timeStepChanges[i]),
            (Sched::VoidPtrCallback) changeTimeStep, tscd);
    }

    // schedule reactivations
    //
    vector<string> reactivateTimes = 
        props.getStringVector("reactivateTimes", vector<string>());
    for (uint i = 0; i <  reactivateTimes.size(); i++) {
        ReactivateData *rd = new ReactivateData();
        Sched::scheduleEvent(
            dhToH(reactivateTimes[i]),
            (Sched::VoidPtrCallback) reactivate, rd);
    }

    // schedule HPC freezing/unfreezing
    //
    vector<string> hpcFreezeTimes = 
        props.getStringVector("hpcFreezeTimes", vector<string>());
    for (uint i = 0; i <  hpcFreezeTimes.size(); i++) {
        FreezeData *ld = new FreezeData(hpcLayerId, Util::isEven(i));
        Sched::scheduleEvent(
            dhToH(hpcFreezeTimes[i]),
            (Sched::VoidPtrCallback) setFrozen, ld);
    }

    // schedule ACC freezing/unfreezing
    //
    vector<string> accFreezeTimes = 
        props.getStringVector("accFreezeTimes", vector<string>());
    for (uint i = 0; i <  accFreezeTimes.size(); i++) {
        FreezeData *ld = new FreezeData(accLayerId, Util::isEven(i));
        Sched::scheduleEvent(
            dhToH(accFreezeTimes[i]),
            (Sched::VoidPtrCallback) setFrozen, ld);
    }

    // schedule HPC lesioning
    //
    string hpcLesionTime = props.getString("hpcLesionTime", "");
    if (!hpcLesionTime.empty()) {
        LesionData *ld = new LesionData(hpcLayerId);
        Sched::scheduleEvent(
            dhToH(hpcLesionTime),
            (Sched::VoidPtrCallback) lesion, ld);
    }

    // schedule ACC lesioning
    //
    string accLesionTime = props.getString("accLesionTime", "");
    if (!accLesionTime.empty()) {
        LesionData *ld = new LesionData(accLayerId);
        Sched::scheduleEvent(
            dhToH(accLesionTime),
            (Sched::VoidPtrCallback) lesion, ld);
    }

    // Schedule PSI infusions
    //
    vector<string> none; // default: no PSI events
    schedulePsiEvents(hpcLayerId, props.getStringVector("hpcPsiTimes", none));
    schedulePsiEvents(accLayerId, props.getStringVector("accPsiTimes", none));
    schedulePsiEvents(sc0LayerId, props.getStringVector("sc0PsiTimes", none));
    schedulePsiEvents(sc1LayerId, props.getStringVector("sc1PsiTimes", none));

}

/**
 * Create and present a random pattern to all layers
 * @param id Pattern ID
 */
static void presentPattern(const string &id)
{
    nsSystem->getLayer(sc0LayerId)->makePattern(id);
    nsSystem->getLayer(sc0LayerId)->setPattern(id);

    nsSystem->getLayer(sc1LayerId)->makePattern(id);
    nsSystem->getLayer(sc1LayerId)->setPattern(id);

    nsSystem->getLayer(hpcLayerId)->makePattern(id);
    nsSystem->getLayer(hpcLayerId)->setPattern(id);

    nsSystem->getLayer(accLayerId)->makePattern(id);
    nsSystem->getLayer(accLayerId)->setPattern(id);

    nsSystem->printGrids(fmt::format("Pattern {}", id));
}

/**
 * Execute a time step of simulation
 */
static void iterate()
{
    Sched::processEvents(simTime);
    nsSystem->runBackgroundProcesses();
    simTime += timeStep;
}

/**
 * Test recall by cueing the CS pattern in the SC0 and evaluating
 * the retrieved pattern in the SC1 layer against the US pattern.
 */
void test()
{
    nsSystem->test(sc0LayerId, "CS-US", "intact");

    bool accWasFrozen = nsSystem->getLayer(accLayerId)->isFrozen;
    if (!accWasFrozen) {
        nsSystem->setFrozen(accLayerId, true);
    }
    nsSystem->test(sc0LayerId, "CS-US", "acc-frozen");
    if (!accWasFrozen) {
        nsSystem->setFrozen(accLayerId, false);
    }

    bool hpcWasFrozen = nsSystem->getLayer(hpcLayerId)->isFrozen;
    if (!hpcWasFrozen) {
        nsSystem->setFrozen(hpcLayerId, true);
    }
    nsSystem->test(sc0LayerId, "CS-US", "hpc-frozen");
    if (!hpcWasFrozen) {
        nsSystem->setFrozen(hpcLayerId, false);
    }
}

/**
 * Run the simulation
 */
static void run()
{
    simTime = 0;
    nsSystem->calcRates();
    
    // Process events scheduled for time=0, if any.
    //
    Sched::processEvents(simTime);

    // Present background pattens if defined
    //
    for (uint i = 0; i < numBackgroundPatterns; i++) {
        presentPattern(fmt::format("dummy-{}", i));
        nsSystem->train();
        iterate();
    }

    // Present the training (CS-US) pattern
    //
    presentPattern("CS-US");
    nsSystem->train();

    // Print the initial system state
    //
    printSystem();

    // Print state headers
    //
    nsSystem->printStateHdrs();
    
    // Run the simulation, printing out state after every iteration
    //
    while (simTime < stopTime) {
        iterate();
        if (TRACE_INFO_IS_ON) nsSystem->printState();
        test();
    }
}

// Data structures for commandline processing

// OptSpec type abbreviations
//
const int NONE = Util::OPTARG_NONE;
const int INT  = Util::OPTARG_INT;
const int UINT = Util::OPTARG_UINT;
const int DBLE = Util::OPTARG_DBLE;
const int STR  = Util::OPTARG_STR;

bool   help            = false;
const char *traceLevel = "undefined";
const char *traceTags  = "undefined";

char *pname;
vector<Util::ParseOptSpec> optSpecs = {
    { "tl",       STR,  &traceLevel,    "traceLevel", ""                  },
    { "tt",       STR,  &traceTags,     "traceTags",  ""                  },
    { "help",     NONE, &help,          "",           ""                  },
};
vector<string>nonFlags = { "[propname=value...] propsFilePath" };

/**
 * Construct a syntax string for use in an invocation error message
 */
static const char *syntax()
{
    return parseOptsUsage(pname, optSpecs, true, nonFlags).c_str();
}

/**
 * Main program
 */
int main(int argc, char *argv[])
{
    // Initialize the random number generator
    //
    Util::initRand();

    // Process command line arguments
    //
    pname = argv[0];

    if (Util::parseOpts(argc, argv, optSpecs) != 0 || help) 
    {
        Util::usageExit(syntax(), NULL);
    }

    // The rest of the arguments must consist of exactly one
    // propertyFileName and zero or more command line
    // props of the name=value format. Command-line property
    // values override those given in the props file.

    typedef struct { string name; string value; } NameValue;
    const char *propsFilePath = NULL;
    std::vector<NameValue> cmdLineProps;

    while (optind < argc) {
        TRACE_DEBUG("argv[{}]='{}'\n", optind, argv[optind]);
        if (strchr(argv[optind], '=') != NULL) {
            const char *errMsg;
            std::vector<string> tokens
                = Util::tokenize(argv[optind], "=", errMsg, "'\"");
            if (errMsg != NULL) {
                Util::usageExit(syntax(), errMsg, NULL);
            }
            if (tokens.size() != 2) {
                Util::usageExit(syntax(), NULL);
            }
            NameValue prop = { tokens[0], tokens[1] };
            cmdLineProps.push_back(prop);
        } else {
            if (propsFilePath == NULL) {
                propsFilePath = argv[optind];
            } else {
                Util::usageExit(syntax(), "Extra arg: {}", argv[optind]);
            }
        }
        optind++;
    }

    if (propsFilePath == NULL) {
        Util::usageExit(syntax(), "No propsFilePath.");
    }
    
    // Set command line props.
    //
    // - Processing cmd line props before props file allows props variable
    //   substitution to work as expected, e.g if a = 5 and b = a in the
    //   props file, then cmd line a = 7 will set both to 7.
    //
    // - Specifying immutable = true prevents props file values from
    //   overriding cmd line values.
    //
    for (uint i = 0; i < cmdLineProps.size(); i++) {
        props.setString(cmdLineProps[i].name, cmdLineProps[i].value, true);
    }

    // Load properties from the props file
    //
    props.readProps(propsFilePath);

    // Retrieve 'title' property to suppress 'unused property' error
    //
    volatile string title = props.getString("title", "");

    // Allow traceLevel and and traceTag to be set using tl and tt
    // properties, unless they've already been set by -tl and -tt flags
    // (cmdline args override cmdline properties override prop file
    //
    if (Util::strEq(traceLevel, "undefined")) {
        traceLevel = props.getString("tl", "warn").c_str();
    }

    if (Util::strEq(traceTags, "undefined")) {
        traceTags = props.getString("tt", "").c_str();
    }

    // Now set traceLevel and traceTags
    //
    if (!Trace::setTraceLevel(traceLevel)) {
        Util::usageExit(syntax(), NULL);
    }

    string errMsg;
    for (auto tag : Util::tokenize(traceTags, ",", errMsg)) {
        Trace::setTraceTag(tag);
    }

    // Print out the property value
    //
    fmt::print("===================================\n");
    fmt::print("{}", props.toString());
    fmt::print("===================================\n");

    // Create the system
    //
    nsSystem = new NsSystem(props);

    // Initialize the simulation time step to 24h; It may be changed
    // dynamically during the simulation, as specified by the
    // 'timeStepChanges' property to allow more fine-grained
    // simulation during selected intervals.

    timeStep = 24;

    stopTime = dhToH(props.getString("stopTime"));
    numBackgroundPatterns = props.getUint("numBackgroundPatterns");

    // Initialize the system and schedule events
    //
    buildSystem();
    printSystem();
    scheduleEvents();

    props.reportUnused(true);

    // Run the simulation
    //
    run();
}
