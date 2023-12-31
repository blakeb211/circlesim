# 2d game to go inside of larger 3d viewer
* merge branch with master
* write a readme

* Add quit action to the 3d version
* Add 3d controls for the 3d version 
* add intermediate locations for more fluid movement
* Make it beautiful and post

# high level design
* add additional classes derived from State to change actor behaviors
* add additional classes derived from Action for user or ai actions 
* objects constrained to unit grid
* update is driven by state if the actor has a state, otherwise the actor update method 

# done
* render threehats
* vary colors and materials
* Load all the worlds up front
* make light follow camera
* decouple the game from the PGE so that it can be compiled in either 3d or 2d mode
* write a 3d ui that interfaces with it 
* make WantNeighborsThreeHats jump towards a cluster if its within a certain number of units
* add a circling state
* tweak the bsp to be a little more dense on walls
* add multiple worlds with different initializations and toggle active world
* write world factory
* create 10 worlds
* add spatial data structure so we can have states that favor neighbor related behaviors 
* keep an updated map of position to cell_context (id, type, distance)
* add rotation to move action; move facing var to a different action 
* add different sized entities
* add state-based movement and hero movement

