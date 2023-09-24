# 2d game to go inside of larger 3d viewer
* add spatial data structure so we can do neighbor analysis (types)
* add different sized entities
* add multiple worlds with different initializations and toggle active world
* connect to 3d program

# high level design
* add additional classes derived from State to change actor behaviors
* add additional classes derived from Action for user or ai actions 
* objects move themselves within a State update or Actor update method
* objects constrained to unit grid (pos_valid validates position)

# done
* add state-based movement and hero movement
