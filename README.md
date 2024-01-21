# Soft-body dev #

This is a test project where I develop a soft-body 2D physics game in C++. It is heavily inspired by this video from the author of JellyCar: https://www.youtube.com/watch?v=3OmkehAJoyo

I usually do full stack web development these days, but I have a background in writing fairly low-level C++ code.

## Goals ##

* Lightning fast compile times. Good header file organization, forward declares and very restrictive use of templates.
* No STL. This might seem odd, but I am challenging myself and will write my own containers. There are always different opionions on STL, my biggest gripes are the snowball effect on compilation times and over-engineering. For more serious projects, I have always used STL, but this is a hobby project so why not challenge your perspective. I've also never been a fan of too clever template code that takes ages to compile (Boost et al).
* Simple, data-oriented structures inspired by Mike Actons talks.

## Approach ##

I will start using SDL2 for rendering. Further on I will integrate some more advanced rendering engine. Why not completely torture myself and write a Vulkan backend? :)