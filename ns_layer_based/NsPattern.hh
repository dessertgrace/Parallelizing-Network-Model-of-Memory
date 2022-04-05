#ifndef NS_PATTERN_HH
#define NS_PATTERN_HH

/**
 * An activation pattern is a vector of unit indices, identifying
 * the units to activate when presenting a stimulus to a layer.
 */

typedef vector<uint> NsPattern;

/**
 * String representation of a pattern (used for debugging).
 */
inline string patternToStr(NsPattern &p)
{
    string ret = "{ ";
    for (auto &u : p) {
        ret += std::to_string(u);
        if (u != p.back()) {
            ret += ", ";
        }
    }
    ret += " }";
    return ret;
}


#endif
