#include "NsSystem.hh"
#include "NsLayer.hh"

/**
 * Implementation of the NsLayer class
 */

NsLayer::NsLayer(const string &id, const string &type)
    : id(id),
      type(type),
      width(props.getUint(id + '.' + "width")),
      height(props.getUint(id + '.' + "height")),
      k(props.getDouble(id + '.' + "k")),
      minInhibition(props.getDouble("minInhibition")),
      maxInhibition(props.getDouble("maxInhibition")),
      initInhibition(props.getDouble("initInhibition")),
      inhibIncr(props.getDouble("inhibIncr")),
      inhibition(initInhibition),
      isClamped(false),
      isFrozen(false),
      isLesioned(false),
      orthogonalPatterns(props.getBool("orthogonalPatterns")),
      nextPatternUnit(0),
      printPatterns(props.getBool("printPatterns"))
{
    uint numUnits = width * height;
    for (uint i = 0; i < numUnits; i++) {
        units.push_back(new NsUnit(this, i));
    }
}

void NsLayer::makePattern(const string &patId)
{
    ABORT_IF(definedPatterns.count(patId) != 0, "Duplicate pattern ID");
    NsPattern p;
    if (orthogonalPatterns) {
        for (uint i = 0; i < k * units.size(); i++) {
            ABORT_IF(nextPatternUnit >= units.size(), "too many patterns");
            p.push_back(nextPatternUnit++);
        }
    } else {
        p = Util::randUniqueUintList(k * units.size(), units.size());
    }
    definedPatterns.insert({patId, p});
    definedPatternIds.push_back(patId);

    TRACE_DEBUG("{}.{} {}\n", id, patId, patternToStr(p));
}

void NsLayer::setPattern(const NsPattern &pat)
{
    if (!isFrozen) {
        clear();
        for (auto id : pat) {
            units[id]->isActive = true;
        }
    }
}

void NsLayer::setPattern(const string &patId)
{
    setPattern(definedPatterns.at(patId));
}

void NsLayer::clearPatterns()
{
    definedPatternIds.clear();
    definedPatterns.clear();
}

/**
 * Randomly select one of the trained patterns and activate it
 */
const string &NsLayer::setRandomPattern()
{
    uint i = Util::randInt(0, definedPatternIds.size());
    const string &pid = definedPatternIds[i];
    setPattern(pid);
    TRACE_INFO("Layer {}, pattern {}", id, pid);
    return pid;
}

void NsLayer::clear()
{
    for(auto u : units) {
        u->isActive = false;
    }
}

/**
 * Adjust inhibition level to drive the layer towards settling with the
 * number of active units = k
 */
void NsLayer::adjustInhibition()
{
    ABORT_IF(isFrozen, "Makes no sense");
    int target = k * units.size();
    int error = (int) getNumActive() - target;

    // make an adjustment to the inhibition level
    // in proportion to the magnitude of the error

    inhibition = Util::bracket(
        inhibition + (double) error / target * inhibIncr,
        minInhibition,
        maxInhibition);

    TTRACE_DEBUG("inhib", "{} active: {}  inhib: {}", id, getNumActive(), inhibition);
}

/**
 * Randomly set each unit to active or inactive with probability k 
 */
void NsLayer::randomize()
{
    ABORT_IF(isFrozen, "Makes no sense");
    for(auto u : units) {
        u->isActive = (Util::randDouble(0.0, 1.0) < k);
    }
}

/**
 * Compute new activations for all units()
 */
void NsLayer::computeNewActivations()
{
    ABORT_IF(isFrozen, "Makes no sense");
    if (!isClamped) {
        for(auto u : units) {
            u->computeNewActivation();
        }
    }
}

/**
 * Apply new activations for all units()
 */
void NsLayer::applyNewActivations()
{
    ABORT_IF(isFrozen, "Makes no sense");
    if (!isClamped) {
        for(auto u : units) {
            u->applyNewActivation();
        }
    }
}

void NsLayer::setFrozen(bool state)
{
    if (!isLesioned) {
        isFrozen = state;

        for(auto u : units) {
            u->setFrozen(state);
        }
    }
}

void NsLayer::lesion()
{
    setFrozen(true);
    isLesioned = true;
}


void NsLayer::maintain()
{
    for(auto u : units) {
        u->maintain();
    }
}

uint NsLayer::getNumActive() const
{
    uint numActive = 0;
    for(auto u : units) {
        if (u->isActive) {
            numActive++;
        }
    }
    return numActive;
}

void NsLayer::printState() const
{
    printNumActive();
    for(auto u : units) {
        u->printState();
    }
}

/**
 * Count number of active target units
 * @param targetId ID of target pattern
 * @return Count of targeted active units
 */
uint NsLayer::getNumHits(const string &targetId) const
{
    NsPattern target = definedPatterns.at(targetId);
    uint ret = 0;
    for (auto id : target) {
        if(units[id]->isActive) ret++;
    }
    return ret;
}

void NsLayer::printScoreHdr()
{
    fmt::print("time score condition layer target hits extras\n");
}

void NsLayer::printNumActiveHdr()
{
    infoTrace("time layer id numActive\n");
}

void NsLayer::printNumActive() const
{
      infoTrace("{} layer {} {}\n", simTime, id, getNumActive());
}

void NsLayer::printGrid(const string &tag, const string &targetId) const
{
    bool targetKnown = (definedPatterns.count(targetId) != 0);

    if (targetKnown) {
        uint targetSize = definedPatterns.at(targetId).size();
        uint numHits = getNumHits(targetId);
        uint numExtras = getNumActive() > numHits ?
            getNumActive() - numHits : 0;
        fmt::print("{} score {} {} {} {} {}\n",
                   simTime / 24., tag, id, targetSize, numHits, numExtras);
    } else {
        if (printPatterns) {
            infoTrace("{} {} {}\n", simTime / 24., tag, id);
        }
    }

    if (printPatterns) {
        infoTrace("+{}+\n", string(2 * width -1, '-'));
        for (uint row = 0; row < height; row++) {
            infoTrace("|");
            for (uint col = 0; col < width; col++) {
                infoTrace("{}{}",
                           units[row * width + col]->isActive ? '*' : ' ',
                           (col < width - 1) ? " " : "");
            }
            infoTrace("|\n");
        }
        infoTrace("+{}+\n", string(2 * width -1, '-'));
    }
}

string NsLayer::toStr(uint iLvl, const string &iStr) const
{
    string ret = fmt::format("{}NsLayer[{}]: ",
                             Util::repeatStr(iStr, iLvl), id);
    for (auto u: units) {
        ret += "\n" + u->toStr(iLvl + 1, iStr);
    }
    return ret;
}
