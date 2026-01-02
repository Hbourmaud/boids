# Boids Simulation

Boids simulation system, implementing spatial partitioning and multithreading for trying to reach 60 FPS with 1000 boids.

## Overview

- **Spawn customizable boid swarms** with adjustable parameters (count, radius, speed)
- **Craig Reynolds three behaviors**: Separation, Alignment, and Cohesion
- **Field of View system** - boids only react to neighbors within their view angle
- **Object avoidance** using golden ratio sphere distribution for ray casting
- **Spatial grid partitioning** (Chunks system)
- **Multithreaded calculations** using `ParallelFor` for parallel behavior processing

### Debug Tools
- Spatial grid visualization with cell boundaries

## Configuration Parameters

**Spawner Settings:**
- `NumberOfBoids`: Boid count to spawn
- `SpawnRadius`: Initial spawn area radius
- `BoidSpeed`: Movement speed

**Behavior Settings:**
- `MaxSeparationDistance` / `SeparationStrength`
- `MaxAlignmentDistance` / `AlignmentStrength`
- `MaxCohesionDistance` / `CohesionStrength`
- `ViewAngle`: Field of view in degrees (0-180)

**Object Avoidance:**
- `AvoidanceDistance`: Ray cast distance
- `AvoidanceRayCount`: Number of detection rays
- `AvoidanceStrength`: Avoidance force multiplier

**Spatial Partitioning:**
- `GridCellSize`: Size of each grid cell
- `WorldExtent`: Total grid coverage area

---

In startup scene, there are 500 boids reaching 60 fps simulation.

Unfortunately, spawn 1000 boids affect performance and simulation reach env. 40 fps.