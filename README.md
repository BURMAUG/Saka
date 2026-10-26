### Description
This is to start the idea I have in my head. to create a terminal application that will observe my most 
used directory path and feed them into my terminal session when I start a terminal session. 
Down the line I need a TUI app to show me the statistics about where I have been the most. 
Something like a heat-map. the calc should be based on time and count.  

- I do not want to use dependencies make it ur self policy.
- I want this to make use of File-desciptors

### TOOLS
- C
- Meson

### Dependencies  (may have to be a diff app)
- CLI tool that will list all the freqently dwelled on directories. ls -d
- CLI tool that will list all the freqently dwelled on directories, and major statistics. ls -ds

d = freqently dwelled directory
ds = freqently dwelled directory and duration I stayed in that directory
