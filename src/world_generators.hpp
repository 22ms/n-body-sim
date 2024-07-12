#ifndef WORLD_GENERATORS_HPP
#define WORLD_GENERATORS_HPP

struct Position;
struct Velocity;

void SphereGenerator(Position* positions, Velocity* velocites, const int n);
void WhateverGenerator(Position* positions, Velocity* velocites, const int n);

#endif