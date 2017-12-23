Dunge
=====

Dunge is a textual PVP game that I wrote several years ago as an experiment.

Plot
----

Plot of the game unintentionally reminds plot of the action movie Die Hard, you are in a burning office building and have to survive, the last player who survives - wins. You are allowed to attack and kill the others.

Running the game
----------------

To host the game on *NIX, use command `./dunge [port]`. To connect to a hosted game use `netcat` or `telnet` from any operating system.

Gameplay
--------

Command           |Description
-----------------------------
 `use    [item]`  |Use specified item, items that are equipable, have to be `take`n at first, for instance, you can use doors without equiping them
 `take   [item]`  |Equip specified item, not all items are equipable though
 `drop   [item]`  |Drop specified item
 `hold   [item]`  |Take one of your equipped items to your hand, in order to help or attack the others with it
 `attack [player]`|"Attack" specified player, with the item you currenlty hold in hand, if it's food or bandage, you'll feed or heal them
 `exit`           |Exit the game (by commiting suicide)
