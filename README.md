# Bandit-Rush
An OpenGL/C++ based tower defense game

Story:
Your castle is under attack from the burglars throughout the night! You must protect yourself and your family from the bandits who are attempting to attack your house.
To do so, the player must strategize and insert towers and walls over its land in order to kill and delay the intruders. These towers will start off quite weak as militia often are, but over time with more and more capital investment, these towers will start to show their battle experience and become more effective at stopping the bandits. Furthermore, the player will also be able to choose from an array of towers each doing different things. After you fend off all the attackers, the player has now survived another night. However, the bandits are scheming to return the next night with more and stronger members, so players should use this valuable time to reinforce, redesign and upgrade their castle’s defenses. Finally, the money left in your coffers will remain there (if there’s any left), so don't feel the need to spend all your money upfront! Your money will roll over each round so saving your money could be a good strategy!

Technical Elements:
Rendering: Simply opening the game will have sprites. These 2D sprites will be dynamically rendered with shadows.
Geometric/Sprites/Assets: Sprites will have meshes and geometry for collision calculations. (Think enemies, arrows, bombs, castles). HUD as well will use assets
AI Logic: Pathfinding for Bandit. State, Behaviour, Swarm and Advanced decision making will also be part of bandit AI.
Collision: 2D collision with missiles and enemies, Enemy missiles and player structures, Bandit and Castle for stealing gold.
Audio: Sounds will be played during events (explosions, enemy dying, clicks, round start). Also will have background music.
 Advanced Technical Elements:
Bandit/enemy AI: Enemy will use a search algorithm to find the shortest path to the castle. If towers exist on the way, the enemy will either move around them or will try to destroy them.
- Impact: This would result in easier gameplay in the beginning that would continuously grow more difficult.
- Alternative: have one kind of enemy with a certain speed.
Save and Reload: The user will be able to save, close, and reload the project.
- Impact: The user will be able to quit the game and start back where they started.
- Alternative: Not implementing, the player will have to go through all levels from the
beginning.
Different enemy types: Different kinds of enemies to increase the difficulty of the game as the player proceeds to higher levels. Difficulty will be scaled by the speed and level of the enemy.
- Impact: More interactive game with higher difficulties.
- Alternative: Having only one kind of enemy throughout the game.
Different tower types: The user will be able to choose different towers to defend their castle. Each tower will have certain abilities, such as higher durability or longer in length to keep away the bandits, and their price will differ.
- Impact: More interactive game with more strategic choices.
- Alternative: 2 kinds of towers for defense.
Devices:
- Keyboard input for camera control
- Mouse input for placing the castles and walls, also for selecting levels and tower
types to insert on the map.
- Space bar to center the map
Concepts:
 Starting menu screen. Load and Start options will load existing game or start a new one Respectively.

  The “Day-time” view. No bandits have spawned. Can place towers or walls. HUD includes amount of money and Round number. Start Round button on the bottom right.
“Night time phase”. Different types of enemies are spawned . They will path towards the castle. Towers placed and long range enemies will fire projectiles. If bandits reach the castle, you will lose money as the player.
 
Tools:
  OpenGL;
  FreeType;
  nlohmann json;

Development Plan:
Provide a list of tasks that your team will work on for each of the weekly deadlines. Account for some testing time and potential delays, as well as describing alternative options (plan B). Include all the major features you plan on implementing (no code).
Skeletal Game
Week Sep 27:
- Hello World project
- Set up HUD (not implemented), kb/m support
- Planning map
- Spawning Enemies
Week Oct 4:
- Add gold stealing (not implemented)
- Basic AI for enemies to run to middle
- Add in simple Graphics/Animations
- Simple Turret/killing (not implemented)
- Basic collision detection and resolution
Minimum Playability (MS1 Oct 8)
Week Oct 11:
- Help
Week Oct 18:
- Add in different Enemy/Turret types
- Adaptive Resolution
- Add wall
Week Oct 25:
- Debugging/buffer time
Playability (MS2 Oct 29)

Week Nov 1:
 - Camera Control/Map movement (added)
 - Simple path finding (added)
 - Observer pattern (added)
 - Add gold stealing, round start/end calculation
 - Sprite Animation (implemented in M1)
 - Level loading + Save
 Projectiles for turret (added)
 - Collision and resolution of arrow with enemy (added)
 - Main game menu (added)
 - Added in state machine (added)
 - Enemy collision resolution with wall/turret (added)
 - Adjust Enemy AI to route around turrets and walls 
 - Story
Week Nov 8:
- Dynamically generating number of enemies based on round
Week Nov 15:
- Debugging/buffer time
Final Game (MS3 Nov19)
Week Nov 22:
- Different Turret/Enemy
- Animation and rendering 
- Tutorial
- Balancing
- Lighting
- Save and Reload
- Debug Graphic
- Game Bar
Week Nov 29:
- Debugging/buffer time
(MS4 Dec 6)
