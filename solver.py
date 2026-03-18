#!/usr/bin/env python3
"""BFS Sokoban solver to verify all levels are solvable."""
from collections import deque

WALL, FLOOR, TARGET, BOX, BOX_ON_TARGET, PLAYER, PLAYER_ON_TARGET = range(7)

levels = {
    # Easy 1
    "Easy-1": {
        "w": 8, "h": 6,
        "data": [
            [WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL],
            [WALL, FLOOR, FLOOR, FLOOR, WALL, FLOOR, FLOOR, WALL],
            [WALL, PLAYER, BOX, FLOOR, FLOOR, FLOOR, TARGET, WALL],
            [WALL, FLOOR, FLOOR, WALL, FLOOR, FLOOR, FLOOR, WALL],
            [WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL],
            [WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL],
        ]
    },
    # Easy 2
    "Easy-2": {
        "w": 8, "h": 7,
        "data": [
            [WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL],
            [WALL, FLOOR, FLOOR, FLOOR, WALL, TARGET, FLOOR, WALL],
            [WALL, FLOOR, BOX, FLOOR, FLOOR, FLOOR, FLOOR, WALL],
            [WALL, FLOOR, FLOOR, WALL, FLOOR, FLOOR, FLOOR, WALL],
            [WALL, FLOOR, FLOOR, BOX, FLOOR, TARGET, FLOOR, WALL],
            [WALL, FLOOR, FLOOR, PLAYER, FLOOR, FLOOR, FLOOR, WALL],
            [WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL],
        ]
    },
    # Easy 3
    "Easy-3": {
        "w": 9, "h": 7,
        "data": [
            [WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL],
            [WALL, FLOOR, FLOOR, FLOOR, WALL, FLOOR, FLOOR, FLOOR, WALL],
            [WALL, FLOOR, TARGET, FLOOR, WALL, FLOOR, BOX, FLOOR, WALL],
            [WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL],
            [WALL, FLOOR, BOX, FLOOR, WALL, FLOOR, TARGET, FLOOR, WALL],
            [WALL, FLOOR, FLOOR, PLAYER, WALL, FLOOR, FLOOR, FLOOR, WALL],
            [WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL],
        ]
    },
    # Medium 1
    "Medium-1": {
        "w": 9, "h": 8,
        "data": [
            [WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL],
            [WALL, FLOOR, FLOOR, FLOOR, WALL, FLOOR, FLOOR, FLOOR, WALL],
            [WALL, FLOOR, BOX, FLOOR, FLOOR, FLOOR, TARGET, FLOOR, WALL],
            [WALL, FLOOR, FLOOR, WALL, WALL, WALL, FLOOR, FLOOR, WALL],
            [WALL, FLOOR, FLOOR, WALL, TARGET, FLOOR, FLOOR, FLOOR, WALL],
            [WALL, FLOOR, BOX, FLOOR, FLOOR, BOX, FLOOR, FLOOR, WALL],
            [WALL, FLOOR, FLOOR, PLAYER, FLOOR, TARGET, FLOOR, FLOOR, WALL],
            [WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL],
        ]
    },
    # Medium 2
    "Medium-2": {
        "w": 10, "h": 8,
        "data": [
            [WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL],
            [WALL, FLOOR, TARGET, FLOOR, WALL, FLOOR, FLOOR, FLOOR, FLOOR, WALL],
            [WALL, FLOOR, FLOOR, FLOOR, WALL, FLOOR, BOX, FLOOR, FLOOR, WALL],
            [WALL, FLOOR, BOX, FLOOR, FLOOR, FLOOR, FLOOR, WALL, FLOOR, WALL],
            [WALL, FLOOR, FLOOR, WALL, FLOOR, FLOOR, FLOOR, WALL, FLOOR, WALL],
            [WALL, TARGET, FLOOR, WALL, FLOOR, BOX, FLOOR, FLOOR, FLOOR, WALL],
            [WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, PLAYER, TARGET, FLOOR, WALL],
            [WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL],
        ]
    },
    # Medium 3
    "Medium-3": {
        "w": 10, "h": 9,
        "data": [
            [WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL],
            [WALL, FLOOR, FLOOR, TARGET, WALL, FLOOR, FLOOR, FLOOR, FLOOR, WALL],
            [WALL, FLOOR, FLOOR, FLOOR, WALL, FLOOR, BOX, FLOOR, FLOOR, WALL],
            [WALL, FLOOR, BOX, FLOOR, FLOOR, FLOOR, FLOOR, WALL, FLOOR, WALL],
            [WALL, WALL, FLOOR, WALL, WALL, WALL, FLOOR, WALL, FLOOR, WALL],
            [WALL, FLOOR, FLOOR, FLOOR, TARGET, FLOOR, FLOOR, FLOOR, FLOOR, WALL],
            [WALL, FLOOR, BOX, FLOOR, FLOOR, WALL, BOX, FLOOR, TARGET, WALL],
            [WALL, FLOOR, FLOOR, PLAYER, FLOOR, FLOOR, FLOOR, FLOOR, TARGET, WALL],
            [WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL],
        ]
    },
    # Hard 1
    "Hard-1": {
        "w": 10, "h": 9,
        "data": [
            [WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL],
            [WALL, TARGET, FLOOR, WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL],
            [WALL, FLOOR, FLOOR, FLOOR, FLOOR, BOX, FLOOR, WALL, FLOOR, WALL],
            [WALL, FLOOR, BOX, WALL, WALL, FLOOR, FLOOR, FLOOR, FLOOR, WALL],
            [WALL, FLOOR, FLOOR, FLOOR, WALL, FLOOR, WALL, WALL, FLOOR, WALL],
            [WALL, WALL, FLOOR, FLOOR, FLOOR, FLOOR, BOX, FLOOR, FLOOR, WALL],
            [WALL, FLOOR, FLOOR, WALL, FLOOR, WALL, FLOOR, TARGET, FLOOR, WALL],
            [WALL, TARGET, FLOOR, FLOOR, BOX, FLOOR, FLOOR, FLOOR, TARGET, WALL],
            [WALL, WALL, WALL, WALL, WALL, WALL, WALL, PLAYER, WALL, WALL],
        ]
    },
    # Hard 2
    "Hard-2": {
        "w": 11, "h": 9,
        "data": [
            [WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL],
            [WALL, FLOOR, FLOOR, FLOOR, WALL, TARGET, FLOOR, FLOOR, FLOOR, FLOOR, WALL],
            [WALL, FLOOR, BOX, FLOOR, WALL, FLOOR, FLOOR, WALL, BOX, FLOOR, WALL],
            [WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL, WALL, FLOOR, TARGET, WALL],
            [WALL, WALL, WALL, FLOOR, WALL, FLOOR, FLOOR, FLOOR, FLOOR, FLOOR, WALL],
            [WALL, TARGET, FLOOR, FLOOR, WALL, FLOOR, BOX, FLOOR, WALL, FLOOR, WALL],
            [WALL, FLOOR, FLOOR, BOX, FLOOR, FLOOR, FLOOR, FLOOR, WALL, FLOOR, WALL],
            [WALL, FLOOR, TARGET, FLOOR, BOX, FLOOR, PLAYER, FLOOR, TARGET, FLOOR, WALL],
            [WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL],
        ]
    },
    # Hard 3
    "Hard-3": {
        "w": 12, "h": 10,
        "data": [
            [WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL],
            [WALL, FLOOR, FLOOR, FLOOR, WALL, TARGET, FLOOR, FLOOR, FLOOR, WALL, FLOOR, WALL],
            [WALL, FLOOR, BOX, FLOOR, FLOOR, FLOOR, FLOOR, WALL, FLOOR, FLOOR, FLOOR, WALL],
            [WALL, FLOOR, FLOOR, WALL, WALL, FLOOR, FLOOR, WALL, FLOOR, BOX, FLOOR, WALL],
            [WALL, TARGET, FLOOR, FLOOR, WALL, FLOOR, BOX, FLOOR, FLOOR, WALL, FLOOR, WALL],
            [WALL, FLOOR, WALL, FLOOR, FLOOR, FLOOR, FLOOR, WALL, FLOOR, FLOOR, TARGET, WALL],
            [WALL, FLOOR, FLOOR, BOX, FLOOR, WALL, FLOOR, FLOOR, FLOOR, WALL, FLOOR, WALL],
            [WALL, TARGET, FLOOR, FLOOR, WALL, FLOOR, FLOOR, BOX, FLOOR, FLOOR, FLOOR, WALL],
            [WALL, FLOOR, FLOOR, FLOOR, FLOOR, BOX, FLOOR, FLOOR, TARGET, FLOOR, PLAYER, WALL],
            [WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL, WALL],
        ]
    },
}


def parse_level(data, w, h):
    """Extract player pos, box positions, target positions, and wall set."""
    player = None
    boxes = set()
    targets = set()
    walls = set()
    for y in range(h):
        for x in range(w):
            t = data[y][x]
            if t == WALL:
                walls.add((x, y))
            elif t == PLAYER:
                player = (x, y)
            elif t == PLAYER_ON_TARGET:
                player = (x, y)
                targets.add((x, y))
            elif t == BOX:
                boxes.add((x, y))
            elif t == BOX_ON_TARGET:
                boxes.add((x, y))
                targets.add((x, y))
            elif t == TARGET:
                targets.add((x, y))
    return player, frozenset(boxes), frozenset(targets), frozenset(walls)


def solve(name, level_data):
    w, h = level_data["w"], level_data["h"]
    player, boxes, targets, walls = parse_level(level_data["data"], w, h)
    
    print(f"\n{'='*50}")
    print(f"Solving {name} ({w}x{h}, {len(boxes)} boxes)")
    print(f"  Player: {player}")
    print(f"  Boxes:  {sorted(boxes)}")
    print(f"  Targets:{sorted(targets)}")
    
    # BFS
    # State = (player_pos, frozenset_of_boxes)
    start = (player, boxes)
    queue = deque()
    queue.append((start, ""))
    visited = set()
    visited.add(start)
    
    dirs = {'U': (0, -1), 'D': (0, 1), 'L': (-1, 0), 'R': (1, 0)}
    
    iterations = 0
    max_iterations = 20_000_000  # safety limit
    
    if len(boxes) != len(targets):
        print(f"  INVALID: {len(boxes)} boxes but {len(targets)} targets!")
        return False, ""
    
    while queue and iterations < max_iterations:
        iterations += 1
        (px, py), cur_boxes = queue[0][0]
        path = queue[0][1]
        queue.popleft()
        
        # Win check
        if cur_boxes == targets:
            print(f"  SOLVABLE! Solution length: {len(path)}")
            print(f"  Solution: {path}")
            print(f"  States explored: {iterations}")
            return True, path
        
        for dname, (dx, dy) in dirs.items():
            nx, ny = px + dx, py + dy
            
            if (nx, ny) in walls:
                continue
            
            new_boxes = cur_boxes
            if (nx, ny) in cur_boxes:
                # Push box
                bx, by = nx + dx, ny + dy
                if (bx, by) in walls or (bx, by) in cur_boxes:
                    continue
                # Simple deadlock: box pushed to corner (not on target)
                if (bx, by) not in targets:
                    # Check if box is in a corner (wall on two adjacent sides)
                    wall_or_oob = lambda xx, yy: (xx, yy) in walls
                    h_blocked = wall_or_oob(bx - 1, by) or wall_or_oob(bx + 1, by)
                    v_blocked = wall_or_oob(bx, by - 1) or wall_or_oob(bx, by + 1)
                    if (wall_or_oob(bx - 1, by) and wall_or_oob(bx, by - 1)) or \
                       (wall_or_oob(bx - 1, by) and wall_or_oob(bx, by + 1)) or \
                       (wall_or_oob(bx + 1, by) and wall_or_oob(bx, by - 1)) or \
                       (wall_or_oob(bx + 1, by) and wall_or_oob(bx, by + 1)):
                        continue
                new_boxes = frozenset((cur_boxes - {(nx, ny)}) | {(bx, by)})
            
            new_state = ((nx, ny), new_boxes)
            if new_state not in visited:
                visited.add(new_state)
                queue.append((new_state, path + dname))
    
    if iterations >= max_iterations:
        print(f"  TIMEOUT after {iterations} states (may still be solvable)")
        return None, ""
    else:
        print(f"  UNSOLVABLE! Explored {iterations} states")
        return False, ""


def main():
    results = {}
    solutions = {}
    for name, data in levels.items():
        solvable, path = solve(name, data)
        results[name] = solvable
        solutions[name] = path
    
    print(f"\n{'='*50}")
    print("SUMMARY:")
    for name, solvable in results.items():
        status = "SOLVABLE" if solvable else ("TIMEOUT" if solvable is None else "UNSOLVABLE")
        sol = solutions[name]
        print(f"  {name}: {status}" + (f" ({len(sol)} moves)" if sol else ""))
    
    print("\nSolutions for code:")
    for name, sol in solutions.items():
        if sol:
            print(f'  // {name}: "{sol}"')


if __name__ == "__main__":
    main()
