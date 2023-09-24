# 2d game to go inside of larger 3d viewer
* add rotation for different sized entities
* add spatial data structure so we can have states that favor neighbor related behaviors 
* put colors in a config file
* add multiple worlds with different initializations and toggle active world
* connect to 3d program

# high level design
* add additional classes derived from State to change actor behaviors
* add additional classes derived from Action for user or ai actions 
* objects move themselves within a State update or Actor update method
* objects constrained to unit grid (pos_valid validates position)

# done
* add different sized entities
* add state-based movement and hero movement
