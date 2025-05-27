This doc describes implemetations for the passes described in dragon book ch9: Machine-Independent Optimizations.

# Implemented DFA Passes
- reaching definition: forward, small-to-large (get a smallest solution)
- live variable analysis: backward, small-to-large
- available expression: forward, large-to-small
- others
  - constant propagation
  - partial redundancy eliminations

# Later
- try to have a abstract layer to implement common things for data flow analysis

# Sections Skipped So far
- 9.5 PRE
- 9.6 Loops in Flow Graphs
  - done for 9.6.1 dominators
- 9.7 Region based analysis
- 9.8 Symbolic Analysis
