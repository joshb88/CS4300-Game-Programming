--------------------------------------------------------------------------------
COMP 4300 - Assignment 3
--------------------------------------------------------------------------------
Assignment Notes:
--------------------------------------------------------------------------------

Submission:

For this assignment you will zip and submit the entire 'src' folder of the
assignment. You are *NOT* allowed to add any files in addition to the ones
alraeady in the src folder from the assignment zip file. This means that you
will not be submitting ANY changes you have made to the visual studio project,
if you used it for development.

Exception:
- Add your custom leve.txt file to the src directory before zipping

YOU MUST INCLUDE THE FOLLOWING INFO IN A COMMENT AT THE TOP OF main.cpp
- ALL students' full names, cs usernames, and student numbers
- If you did not get a specific feature to work, please explain what you tried
  to do to get it to work, and partial marks may be given for effort.

--------------------------------------------------------------------------------
Program Specification
--------------------------------------------------------------------------------

In this assignment you will be writing the game that was presented in class.
This game must have the following features:

Assets:
- Entities in the game will be rendered useing various Textures and Animations
  which we will be calling Assets (along with Fonts)
- Assets are loaded once at the beginning of the program and stored in the
  Assets class, will be singleton (which is stored by the GameEngine class)
- All Assets are defined in assets.txt, with the syntax defined below

Note:
- All entity positions denote the center of  their rectangular sprite
  It also denotes the center of the bounding box, if it has one
  This is set via sprite.setOrigin() in the Animation class constructor

Player:
- The player Entity in the game is represented by Megaman, which has several
  different Animations: Stand, Run, and Air. You must determine which state
  the player is currently in and assign the correct Animations.
- The player moves with the following controls:
  Left: A key, Right: D key, Jump: W key, Shoot: Space key
- The player can move left, move right, or shoot at any time during the game
  This means the player can move left/right while in the air
- The player can only jump it is currently standing on a tile
- If the jump key is held, the player should not continuously jump, but
  instead it should only jump once per button press. If the player lets
  go of the jump key mid-jump, it should start falling back down immediately
- If the player moves left/right, the player's sprite will facew in that
  direction until the other direction has been pressed
- Bullets shot by the player will travel in the direction the player is facing
- The player collides with 'Tile' entities in the level (see level syntax) and
  cannot move them. The player land on a Tile entity and stand in place
  if it falls on it from above.
- The player does not collide with 'Dec' (decoration) entities in the level
- If the player falls below the bottom of the screen, they can respawn at the start
- The player should have a Gravity component which constantly accelerates it
  downward on the screen until it collides with a tile
- The player has a maximum speed specified in the Level file (see below) which
  it should not exceed in either x or y direction
- The player will be given a CBoundingBox of a size specified in the level file
- The player's sprite and bounding box are centered on the player's positions

Animations:
- See below for Animation asset specification
- Animations are implemented by storing multiple frames inside a texture
- The Animation class handles frame advancement based on animation speed
- You need to implement Animation::update() to properly progress animations
- You need to implement Animation::hasEnded() which returns true if an
  animation has finished its last frame, false otherwise
- Animations can be repeating (loop forever) or non-repeating (play once)
- Any entity with a non-repeating animation should be destroyed once its
  Animation's hasEnded() returns true (has finished one cycle)

Decoration Entities:
- Decoration entities ('Dec') in a level file are simply drawn to the screen,
  and do not interact with any other entities in the game in any way
- Decorations can be given any Animation in the game, but intuitively they
  should be reserved for things like clouds, bushes, etc

Tiles:
- Tiles are entities that define the level geometry and interact with players
- Tiles can be given any animation that is defined in the Assets file
- Tiles will bev given a CBoundingBox equal to the size of the animation
  tile->get<CAnimation>().animation.getSize()
- The current animation displayted for a tile can be retrieved with:
  tile->get<CAnimation>().animation.getName()
- Tiles have different behaviour depending on which Animation they are given

  Brick tiles:
  - Brick tiles are given the 'Brick' Animation
  - When a brike tile collides with a bullet, or is hit by a player from below:
    - Its animation should change to 'Explosion' (non-repeat)
    - Non-repeating animation entites are destroyed when hasEnded() is true
    - Its CBoundingBox component should be removed

  Question Tiles:
  - Question tiles are given the 'Question' Animation when created
  - When a Question tile is hit by a player from below, 2 things happen:
    - Its ANimation changes to the darker 'Question2' animation
    - A temporary lifespawn entity with the 'Coin' animation should appear
      for 30 frames, 64 pixels above the location of the Question entity

GUI:
- You must construct a GUI using ImGui which has the following functionality:
- The GUI must display options to turn off each system independently
- For each entity in the game, the GUI must list the ID, tag, and position
  of that entity. You must display a list of all entities, as well as
  list of entities by their tag. You must also have some way of destroying
  a given entity by interacting with the UI element associated with it.
  (this is essentially the same as A2, to help you in debugging the game)

Rendering:
- Entity rendering has been implemented for you, no need to change the system
- Entities are rendered in the order that they are stored in the EntityManager.
  with later entities being drawn on top of previous ones.

Bonus (total 10%):
- Any special effects which do not alter game play can be added fro up to
  5% bonus marks on the assignment. Note that assignments cannot go above
  100% total marks, but the 5% bonus can overwrite any marks lost in other
  areas of the assignment.
- You may develop a 'special weapon' that has special efects which can
  also contribute anothjer 5% bonus marks available on the assignment.
- I will show off some of the more impressive bonus submissions in class

Misc:
- The 'P' key should pause the game
- Pressing the 'T' key toggles drawing textures
- Pressing the 'C' key toggles drawing the bounding boxes of entities
- Pressing the 'G' key toggles drawing of the grid
  This should be very helpful for debugging
- The 'ESC' key should go 'back' to the Main Menu, or quit if on the Main Menu

--------------------------------------------------------------------------------
Level Creation
--------------------------------------------------------------------------------

For this assignment you are also required to create your own level. This level
shouldinclude some interesting gameplay. Include this lewvel in the zip file as
level.txt, and I will show off some of the more interesting levels in class
after the assignment is due.

--------------------------------------------------------------------------------
Config Files
--------------------------------------------------------------------------------

There wil be two configuration files in this assignment. The Assets config
file, and the Level configuration file.

--------------------------------------------------------------------------------
Assets File Specification
--------------------------------------------------------------------------------

There will be three different line types in the Assets file, each of which
correspond to a different type of Asset. They are as follows:
 
Texture Asset Specification:
Texture N P
  Texture Name     N        std::string (it will have no spaces)
  Texture FilePath P        std::string (it will have no spaces)

Animation Asset Specification:
Animation N T F S
  Animation Name   N        std::string (it will have no spaces)
  Texture Name     T        std::string (refers to an existing texture)
  Frame Count      F        int (number of frames in the animation)
  Anim Speed       S        int (number of game frames between anim frames)

Font Asset Specification:
Font N P
  Font Name        N        std::string (it will have no spaces)
  Font File Path   P        std::string (it will have no spaces)

--------------------------------------------------------------------------------
Level Specification File
--------------------------------------------------------------------------------

Game levels will be specified by a level file, which will contain a list of
entity specifications, one per line. It will also contain a single line which
specifies properties of the player in that level. In this way, you can define
an entire level in the data file,  rather than in programming code. The syntax
of the lines of the Level file are as follows.

IMPORTANT NOTE:
   All (GX, GY) positions given in the level specification file are given in 
   'grid' coordinates. The 'grid' cells are of size 64 by 64 pixels, and the
   entity should be positioned such that the bottom left corner of its texture
   is aligned with the bottom left corner of the given grid coordinate. The
   grid starts at (0, 0) in the bottom-left of the screen, and can be seen by
   pressing the 'G' key while the game is running.

Tile Entity Specification:
Tile N GX GY
  Animation Name   N       std::string (Animation asset name for this tile)
  GX Grid X Pos    GX      float
  GY Grid Y Pos    GY      float

Decoration Entity Specification:
Dec N X Y
  Animation Name   N       std::string (Animation asset name for this tile)
  X Position       X       float
  Y Position       Y       float

Player Specification
Player GX GY CW CH SX SY SM GY B
  GX, GY Grid Pos  X, Y    float, float (starting position of player)
  BoundingBox W/H  CW, CH  float, float
  Left/Right Speed SX      float
  Jump Speed       SY      float
  Max Speed        SM      float
  Gravity          GY      float
  Bullet Animation Block   std::string (Animation asset to use for bullets)

--------------------------------------------------------------------------------
Assignment Hints
--------------------------------------------------------------------------------

I recommend approaching this assignment in the following order, which will
help you debug your program along the way with minimal errors. Remember to
do one step at a time and test whether what you have just implemented is
working properly before moving on to any additional steps.

-  Rendering system has already been set up for you, to help you debug:
   You can press the T key to toggle drawing textures
   You can press the C key to toggle drawing bounding boxes
   You can press the G key to toggle drawing the grid

-  You can implement Animation::update() and Animation::hasEnded() at any
   time, it will not affect the gameplay mechanics whatsoever, just animation

-  Implement Scene   