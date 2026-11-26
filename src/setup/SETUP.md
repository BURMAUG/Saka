### what is the most important entry point
- knowing when cd is happening
    - shell hook  
        -> I only needs to run when it needs to be running. 
    - always running process 
        -> downside is it will be running even when it is not needed at all this is a problem 
            I do not want to ride with

- how to time cd periods  


### knowing when cd is happening


###  how to time cd periods  



- I need to listen for terminal start
- then listen for when we cd'd into a dir 
        - then start a duration counter
        - listen to when cd'd out and stop time for that dir 

