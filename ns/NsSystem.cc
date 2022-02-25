#include "NsSystem.hh"

/**
 * Constructor
 * @param props Properties
 */
NsSystem::NsSystem(Props &props)
    : trainNumStimCycles(props.getUint("trainNumStimCycles")),
      consNumStimCycles(props.getUint("consNumStimCycles")),
      reactNumStimCycles(props.getUint("reactNumStimCycles")),
      numSettleCycles(props.getUint("numSettleCycles"))
{
}

/**
 * Add a layer
 * @param id Layer ID
 */
void NsSystem::addLayer(const string &id, const string &type)
{
    NsLayer *layer = new NsLayer(id, type);
    std::pair<string, NsLayer *> pair(id, layer);
    layers.insert(pair);
}

/**
 * Add a tract
 * @param fromLayerId ID of "from" layer
 * @param toLayerId ID of "to" layer
 * @param type Type of tract
 */
void NsSystem::addTract(const string &fromLayerId,
                        const string &toLayerId,
                        const string &type)
{
    string id = fromLayerId + "-" + toLayerId;

    NsLayer *fromLayer = layers.at(fromLayerId);
    NsLayer *toLayer = layers.at(toLayerId);

    NsTract *tract =
        new NsTract(id, fromLayer, toLayer, type);
    std::pair<string, NsTract *> pair(id, tract);
    tracts.insert(pair);
}

/**
 * Add bi-directional tract, i.e. two tracts, one in either
 * direction between two layers.
 * @param layer1Id ID of one layer
 * @param layer2Id ID of another layer
 * @param type Type of tract
 */
void NsSystem::addBiTract(const string &layer1Id, const string &layer2Id,
                          const string &type)
{
    addTract(layer1Id, layer2Id, type);
    addTract(layer2Id, layer1Id, type);
}

/**
 * Calculate rates current timeStep value for all tracts
 */
void NsSystem::calcRates()
{
    for (auto &t : tracts) {
        t.second->calcRates();
    }
}

/**
 * Acquire the currently presented pattern
 */
void NsSystem::acquire(uint numStimCycles, const char *tag)
{
    for (auto &t : tracts) {
        t.second->acquire(numStimCycles, tag);
    }
}

/**
 * Cycle unit activations for a fixed number of times, and call it
 * settled. Cycling is synchronous: first calculate all units' new
 * activation, then update them all in parallel.
 */
void NsSystem::settle()
{
    for (uint c = 0; c < numSettleCycles; c++) {
        for (auto &l : layers) {
            if (!l.second->isFrozen) {
                l.second->computeNewActivations();
            }
        }
        for (auto &l : layers) {
            if (!l.second->isFrozen) {
                l.second->applyNewActivations();
                l.second->adjustInhibition();
            }
        }
    }
}

/**
 * Activate and clamp a randomly chosen trained pattern in HPC,
 * clear the other layers, then cycle and learn whatever pattern
 * is settled on.
 */
void NsSystem::consolidate()
{
    // TODO: maybe configurable number of
    // consolidation trials per day?

    // Clear all the layers
    //
    clear();

    // If HPC is frozen (which includes lesioned), then
    // do nothing.

    NsLayer *hpcLayer = layers.at(hpcLayerId);

    if (hpcLayer->isFrozen) {
        return;
    }

#if 1
    // Activate and clamp a randomly chosen defined pattern in
    // the HPC layer
    //
    const string &hpcPid = hpcLayer->setRandomPattern();
    hpcLayer->isClamped = true;

#else
    // Randomize the HPC layer
    const string &hpcPid = "";
    hpcLayer->randomize();
#endif

    printGrids("cons-present");

    settle();

    printGrids("cons-settled", hpcPid);
    
    // Learn the pattern settled into: PSD growth
    //
    for (auto &t : tracts) {
        t.second->consolidate(consNumStimCycles);
    }
}

/**
 * Run maintenance processes
 */
void NsSystem::maintain()
{
    for (auto &t : tracts) {
        t.second->maintain();
    }
    for (auto &l : layers) {
        l.second->maintain();
    }
}

/**
 * Clear activations and unclamp all layers
 */
void NsSystem::clear()
{
    for (auto &l : layers) {
        l.second->clear();
        l.second->isClamped = false;
    }
}

/**
 * Freeze or unfreeze a layer
 */
void NsSystem::setFrozen(const string &layerId, bool state)
{
    NsLayer *layer = layers.at(layerId);
    layer->setFrozen(state);
}

/**
 * Lesion a layer
 */
void NsSystem::lesion(const string &layerId)
{
    NsLayer *layer = layers.at(layerId);
    layer->lesion();
}

/**
 * Run background processes in all layers and tracts
 */
void NsSystem::runBackgroundProcesses()
{
    consolidate();
    maintain();
}

/**
 * Present a cue pattern to one layer, then cycle the system until it settles.
 * Call printGrids to report recall performance. 
 */
void NsSystem::retrieve(const string &cueLayerId,
                        const string &patternId,
                        const string& condition)
{
    clear();

    // Set and clamp the specified pattern in the cue layer
    //
    NsLayer *cueLayer = layers.at(cueLayerId);
    cueLayer->setPattern(patternId);
    cueLayer->isClamped = true;

    printGrids(fmt::format("{}-present", condition));

    // Cycle until settled
    //
    settle();
    printGrids(fmt::format("{}-settled", condition), patternId);
}

/**
 * Test recall by presenting the CS pattern to the specified layer
 * and executing a retrieval.
 * @param cueLayerId ID of layer to cue
 * @param patternId ID of pattern to use as cue
 * @param condition Condition identifier
 */
void NsSystem::test(const string &cueLayerId, const string &patternId,
                    const string &condition)
{
    // Record inhibition levels before the test
    //
    for (auto l : layers) {
        l.second->saveInhibition();
    }

    // Do the test
    //
    retrieve(cueLayerId, patternId, condition);

    // Restore inhibition levels, in order to leave the system unaffected by
    // the test.
    //
    for (auto l : layers) {
        l.second->restoreInhibition();
    }
}

/**
 * Train the currently presented pattern
 */
void NsSystem::train()
{
    acquire(trainNumStimCycles, "train");
}

/**
 * Reactivate a CS-US pattern by presenting the CS pattern to SC0 and
 * executing a retrieval, then  (TODO) do some learning
 */
void NsSystem::reactivate()
{
    // Present the cue (CS) and execute a retrieval
    //
    retrieve(sc0LayerId, "CS-US", "reactivate");

    // Execute reactivation logic in all tracts
    //
    for (auto &t : tracts) {
        t.second->reactivate();
    }

    // Patterns should disappear from HPC layer's (really, any layer's)
    // pattern list as they decay. Here we simply clear HPC's pattern list
    // at reactivation time. This is ok a long as reactivation happens long
    // after training pattern HPC components have decayed, which is the case
    // in all current reactivation test cases.
    //
    getLayer(hpcLayerId)->clearPatterns();

    // Activate a random HPC pattern execute a learning cycle.
    //
    getLayer(hpcLayerId)->makePattern("react");
    getLayer(hpcLayerId)->setPattern("react");

    printGrids(fmt::format("Pattern {}", "react"));

    acquire(reactNumStimCycles, "react");
}

/**
 * Toggle PSI on or off in all tracts that terminate on the
 * specified layer.
 */
void NsSystem::togglePsi(string layerId, bool state)
{
    NsLayer *layer = getLayer(layerId);
    for (auto &t : tracts) {
        if (t.second->fromLayer == layer || t.second->toLayer == layer) {
            t.second->togglePsi(state);
        }
    }
}

void NsSystem::printStateHdrs()
{
    NsConnection::printStateHdr();
    NsUnit::printStateHdr();
    NsLayer::printScoreHdr();
    NsLayer::printNumActiveHdr();
    NsTract::printNumPotentiatedHdr();
}

void NsSystem::printState() const
{
    for (auto &l : layers) {
        l.second->printState();
    }
    for (auto &t : tracts) {
        t.second->printState();
    }
}

void NsSystem::printGrids(const string &tag,
                          const string &targetId) const
{
    for (auto &l : layers) {
        l.second->printGrid(tag, targetId);
    }
}

void NsSystem::printSize()
{
    uint tot = 0;
    for (auto &l : layers) {
        uint n = l.second->units.size();
        infoTrace("Layer {}: {} units\n", l.second->id, n);
        tot += n;
    }
    infoTrace("Total: {} units\n", tot);

    tot = 0;
    for (auto &t : tracts) {
        uint n = t.second->connections.size();
        infoTrace("Tract {}: {} connections\n", t.second->id, n);
        tot += n;
    }
    infoTrace("Total: {} connections\n", tot);
    
}

string NsSystem::toStr(uint iLvl, const string &iStr) const
{
    string ret = Util::repeatStr(iStr, iLvl) + "NsSystem:";
    for (auto &l : layers) {
        ret += "\n" + l.second->toStr(iLvl + 1, iStr);
    }
    for (auto &t : tracts) {
        ret += "\n" + t.second->toStr(iLvl + 1, iStr);
    }

    return ret;
}

