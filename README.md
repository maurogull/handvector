# handvector

With this tool I try to find bezier curves from hand written strokes, i.e. "vectorize".
This program is supposed to be used with a drawing tablet (Wacom...).

At the time of this development I was studying typography (typeface) design. Initial paper drawings are key to this design process.

Bezier curves used in typefaces have to follow certain criteria, in order to be later rasterized on a screen properly.
This program implement that rules.

## Design

Algorithm works in three steps:
1. Extract points from a raw drawing (bitmap) on a canvas
2. Discover paths from step 1 points list
3. Discover nodes in step 2 strokes list and proceed to estimate beziers

## Techologies

ANSI C language, GTK and Cairo libraries for GUI.

There is a lot of trigonometry and geometry calculation.

## Demo

![Screen capture showing interface 1](screenshots/Screenshot-0.png)

![Screen capture showing interface 2](screenshots/Screenshot-1.png)

![Screen capture showing interface 3](screenshots/Screenshot-2.png)

![Screen capture showing interface 4](screenshots/Screenshot-3.png)
