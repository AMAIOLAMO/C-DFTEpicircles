# DFTEpicircle Drawer

![](https://raw.githubusercontent.com/AMAIOLAMO/C-DFTEpicircles/main/examples/showcase_cute_cat.gif)

this is a simple discrete fourier transform implementation of using epicircles to draw points

## dependencies
> this requires `raylib`, `cc` to be preinstalled

this application also default compiles to `X11`, for `wayland`, modify the `MakeFile` with wayland specific instructions(I will not provide such here)


## running
It is pretty easy, just `cd` into the directory you have unpacked the content in, and run `make clean && make`


## Load from point file
`.pt` is a format I created specifically for loading points, it follows a simple format of the following:
```
x1,y1
x2,y2
x3,y3
```
where it inteprets `x1,y1` as a single sampled point, here's an example of a square:
```
0.0f,0.0f
1.0f,0.0f
1.0f,1.0f
0.0f,1.0f
```
which creates 4 points in sequence, and draws a 1 unit by 1 unit square

> NOTE: currently the maximum points that can be read is **2000** points


## Custom SVG
Currently it is not supported yet, but I am thinking of adding such. For now you need to modify the variable `discreteValues` array in `main.c`, to contain all the points of your specified drawing


## FUTURE TODO
1. allow instead to load the `.pt` file by dragging directly in the window
2. switch to Fast Fourier transform
3. Refactor Code into a more modular system
