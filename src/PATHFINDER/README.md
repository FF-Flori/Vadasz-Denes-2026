# Pathfinder structure

### A) Get data - constructor
- time limit
- map
  - get the absolute path of map file
  - read the file and store

### B) Group ores
- group storage structure
  - group class
  - array of group objects
- read map
  - make an ore group
  - split group to smaller segments if necessary
  - store created group(s)

### C) Calculate path graph
- calculate all path costs between groups and the start position
- calculate the value of groups
- make a structure representing the graph

### D) Genetic algorithm
- generate start paths
- powerful fitness function
- mutation types
