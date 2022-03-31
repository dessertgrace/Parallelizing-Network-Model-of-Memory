#include "NsSystem.hh"
#include "NsUnit.hh"
#include "MathUtil.hh"

NsUnit::NsUnit(const NsLayer *layer, uint index)
    : layer(layer), 
      id(layer->id + "." + fmt::format("{:02}", index)),
      actFuncK(props.getDouble("actFuncK")),
      actThreshold(props.getDouble("actThreshold")),
      isFrozen(false),
      isActive(false),
      newIsActive(false),
      lastNetInput(0.0)
{}

/**
 * Probability of activation is a sigmoid function of net input
 * @param netInput Net input
 */
bool NsUnit::activationFunction(double netInput)
{
    if (netInput <= actThreshold) return false;

    double probOfActivation =
        MathUtil::asigmoid(netInput, actFuncK, layer->inhibition);
    //infoTrace("XXX {}\n", netInput);

    double ret = Util::randDouble(0.0, 1.0) < probOfActivation;

    //fmt::print("activationFunction: {} {}\n",
    //           netInput - layer->inhibition, ret);
    
    return ret;
}

/**
 * Calculate net input as sum of the weights of those inbound connection
 * whose sending units are active, i.e. count "true" activity level as 1 and
 * "false" as zero. Then use activationFunction to determine the unit's new
 * activation state and store it in newIsActive.
 */
void NsUnit::computeNewActivation()
{
    if (isFrozen) {
        newIsActive = false;
    } else {
        // Calculate net input
        //
        double netInput = 0.0;
        uint numActiveInputs = 0;
        for (auto c : inConnections) {
            if (c->fromUnit->isActive && c->getStrength() > 0.0) {
                netInput += c->getStrength();
                numActiveInputs++;
            }
        }
#if 0
        // TODO: this didn't work, because it kills everything when there
        // are many connections. -- It would be nice to find another way to
        // handle different system sizes without twiddling parameters.

        // Normalize the input to the [0, 1] range. (This reflects the idea
        // of homeostatic synaptic plasticity, a.k.a. synaptic scaling)
        //
        netInput *= (double) numActiveInputs / inConnections.size();
#endif
        // Use the activation function to decide whether to become/remain
        // active
        //
        newIsActive = activationFunction(netInput);
        lastNetInput = netInput;
    }
}

void NsUnit::applyNewActivation()
{
    isActive = newIsActive;
}

void NsUnit::setFrozen(bool state)
{
    isFrozen = state;
    if (isFrozen) {
        isActive = false;
    }
}

void NsUnit::maintain()
{
}

void NsUnit::printStateHdr()
{
    infoTrace("time unit ID ACTIVE\n");
}

void NsUnit::printState() const
{
    infoTrace("{} unit {} {}\n", simTime / 24., id, isActive ? 'a' : 'i');
}

string NsUnit::toStr(uint iLvl, const string &iStr) const
{
    return fmt::format("{}[{} {}]",
                       Util::repeatStr(iStr, iLvl),
                       id, isActive ? 'a' : 'i');
}
