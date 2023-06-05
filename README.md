# Fole & Raul: Capylandia Eco-Rescue!

## Introduction ##

After the insane adventures of 'Fole & Raul go Bananas' and 'Fole & Raul: Flower Power!', 
our heroes are once again on the loose. They have taken up eco-tourism as a hobby. 
They travel around using sustainable methods of transport, and undertake volunteer 
work to support local ecosystems. 

So imagine their reaction when they arrive at Capylandia! The idyllic island atol is 
overrun by tourists! This invasive species drives around in noisy cars, litters the
beautiful beaches. It's gotten so bad that the azure sea is now covered with
an iridescent sheen of oil pollution!

## How to play ##

The goal of the game is to rescue all trapped capybaras in each level, before your time
runs out. You can play in single player mode or with two players behind the
same computer in cooperative mode (split-screen).

In the menu you can change the sound and music volume, change the number of players and reconfigure
keys.

Default keys:

Player 1: Fole the cat
Left = Left
Right = Right
Up = Up
Down = Down
Attack = Right Ctrl

Player 2: Raul the monkey
Left = A
Right = D
Up = W
Down = S
Attack = Left Ctrl

Find trapped capybaras. Simply walk up to them to rescue them.

Do not touch the annoying tourists, as they will hurt you, and even kill you after a number
of hits. You can stun them with your soaker gun, by pressing the attack key.
They might drop some money. 

You can spend money with the traveling salesman bunny, which is usually located somewhere on the atol.
He's got only one item for sale, so you have to decide if it's worth it. Simply
walk up to him with enough money in pocket to make the trade.

You can move between islands through bridges or teleporters
(that look like blue round swirly pads).

The atol is a bit of a maze. To find your way around easier, look for a nautical chart!
It will display your position, as well as the position of NPCs, locked doors, and keys.

If you manage to rescue all capybaras before the time runs out and before you both
die, you will advance to the next atol. The atol gets a bit larger and the tourists are 
a bit hardier.

## How the TINS 2023 rules were implemented ##

Rules:

```
genre rule #41: Fable: all characters are animals
```

The game has the following animals in it:

The enemies: a bothersome pelican, a littering chicken and a driving duck
The friendly NPCs: the bunny salesman, the capybara rescuee
The protagonists: Fole the cat and Raul the monkey.

As for a Fable: there is probably a lesson in here somewhere about taking good care of the environment.

```
art rule #18: Iridescence
```

The iridescent oil sheen surrounding all the islands is implemented using a custom hand-written GLSL shader.

```
art rule #32: All Dialog must be implemented comic-book bubble style
```

The protagonists and NPCs will talk to you with speech bubbles.

```
tech rule #65: Implement a chart (bar chart, pie chart, any kind of chart)
```

We opted for a nautical chart. Keep in mind that this is a chart, not a map! So what's
the difference between a map and a nautical chart you say? A nautical chart 
has information about coastlines (checks) and is a living document
updated with relevant information to your navigation, such as blocked passages
or way makers (check).

The chart requirement was the basis for setting the game on a series of islands.

Once you pick up the nautical chart collectible, there is a lot of dynamic data
displayed on it, such as locations of NPCs, keys, and locked doors.

```
bonus rule #24: Dynamic Duo - If you make your game suitable for two players, you may skip one rule.
```

Since two-player mode was already in the base game, we don't want to take credit for this in TINS 2023.

## Code re-use ##

Source can be found here: https://github.com/amarillion/tins2023

This game is a sequel to our 2020 TINS entry 'Fole & Raul: Flower Power!'.
We've taken that game as a base, replaced most assets, and added some new features.
To proper judge our entry, you should know what is old and what is new:

Here is what was already there in our TINS 2020:
* 2 player split-screen mode
* Random level generation with locks and keys
* Title menu
* Player movement, enemy movement
* Weapons and weapon upgrades
* Sound effects

Here is what is new:
* All animals drawn from scratch
* Tiles and maps redrawn
* Completely new music
* Nautical chart
* Iridescent effect
* Speech balloons
* Shopping mechanic

Other re-used assets:

Some tiles were copied from https://stealthix.itch.io/rpg-beach-tileset (CC-0)

A few remaining assets come from the base game from 2020, such as
the teleporter pad, and some sound effects. 

The guitar strum sounds were recorded by SUPERDAZE.  

The sample Alien Siren was derived from:
http://soundbible.com/1753-Alien-Siren.html
License: Public Domain | Recorded by KevanGC

The latest version of Amarillion's 'twist' engine was uploaded to github 
just before the start of the competition.

## Credits ##

Code: Amarillion (@mpvaniersel on twitter)
Gfx: Max
Music: AniCator (@AniCator on twitter)
