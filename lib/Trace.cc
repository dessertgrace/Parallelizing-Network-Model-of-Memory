/**
 * @file Trace.cc
 *
 * Author: Peter Helfer
 * Date: 2011-05-23
 */

#include "Trace.hh"

Trace::TraceLevel Trace::traceLevel = Trace::TRACE_Warn;
unordered_set<string> Trace::traceTags;
uint Trace::indentLevel = 0;
