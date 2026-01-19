#pragma once

// Will need the list of objects we can collide with (ECS would be amazing, but idk if I have time at this point)
// as well as the position of the current object, it's updated velocity, and delta time. That way we can find everything along our path of our line segment of where we will head
// We will assume our character is a AABB instead of a capsule for this specific implementation (unless I have extra time later, which I doubt)
// Off the top of my head, Oct-tree for partioning the space of our level, could severely speed up finding objects, as we do O(log n) top find our space(s) of our line segment, and the objects
// within that we can loop through.

// BSP tree like quake is a nice idea, but I may have to keep with a sweeping check, and currently iterate over all objects, and change my data structure as necessary.
// ^ don't think i could, as it's typically pre-computed and I have moving parts? So a quad/oct tree space partitioned is probably the best approach.
