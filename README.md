# DFTEpicircle Drawer

this is a simple discrete fourier transform implementation of using epicircles to draw points

## dependencies
> this requires `raylib`, `cc` to be preinstalled

this application also default compiles to `X11`, for `wayland`, modify the `MakeFile` with wayland specific instructions(I will not provide such here)


## running
It is pretty easy, just `cd` into the directory you have unpacked the content in, and run `make clean && make`



## Custom SVG
Currently it is not supported yet, but I am thinking of adding such. For now you need to modify the variable `discreteValues` array in `main.c`, to contain all the points of your specified drawing
