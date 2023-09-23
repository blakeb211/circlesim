# 2d game to go inside of larger 3d viewer
* add spatial data structure so we can do neighbor analysis 
* add multiple worlds with different initializations and toggle active world
* connect to 3d program

# high level design
* add additional classes derived from State to change actor behaviors
* add additional classes dervied from Action to accept user input
* objects move themselves within a State update method ; pos_valid is checked
* hero moves from user input ; pos_valid is checked

# done
* add state-based movement and hero movement
