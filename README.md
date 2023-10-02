# 2d game to go inside of larger 3d viewer
* debug overlap of guys with walls 
* add a circling state
* add a wrapping state
* add multiple worlds with different initializations and toggle active world
* write a 3d ui that interfaces with it 

# high level design
* write world factory
* register objects in occupation map as they are created
* create 10 worlds
* 3d ui will be separate thread with minimal interface
* add additional classes derived from State to change actor behaviors
* add additional classes derived from Action for user or ai actions 
* objects constrained to unit grid
* update is driven by state if the actor has a state, otherwise the actor update method 

# done
* add spatial data structure so we can have states that favor neighbor related behaviors 
* keep an updated map of position to cell_context (id, type, distance)
* add rotation to move action; move facing var to a different action 
* add different sized entities
* add state-based movement and hero movement

