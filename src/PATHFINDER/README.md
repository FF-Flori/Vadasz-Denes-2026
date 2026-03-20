# Pathfinder

## Planning

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
- make a fitness function to have a close estimate of a genome's fitness
  - implement multi-threading
- mutations
- provide some information about generations and their success rate

### E) Provide calculated data
- genetic generations' data
  - best few fitness values per generation
  - mutations used
- instructions for best path
- time and resources used