#!/usr/bin/env python3
"""Test new hard level designs."""
from collections import deque

WALL, FLOOR, TARGET, BOX, BOX_ON_TARGET, PLAYER, PLAYER_ON_TARGET = range(7)
W, F, T, B, BT, P, PT = WALL, FLOOR, TARGET, BOX, BOX_ON_TARGET, PLAYER, PLAYER_ON_TARGET

# Hard 1 (9x9, 3 boxes) - "The Detour"
hard1 = {
    "w": 9, "h": 9,
    "data": [
        [W, W, W, W, W, W, W, W, W],
        [W, F, F, F, F, T, F, F, W],
        [W, F, F, W, B, F, F, F, W],
        [W, F, F, F, F, F, W, F, W],
        [W, F, B, F, F, F, F, F, W],
        [W, F, F, F, W, F, F, F, W],
        [W, T, F, F, F, F, B, T, W],
        [W, F, F, P, F, F, F, F, W],
        [W, W, W, W, W, W, W, W, W],
    ]
}

# Hard 2 (10x9, 3 boxes) - "The Crossroads"
hard2 = {
    "w": 10, "h": 9,
    "data": [
        [W, W, W, W, W, W, W, W, W, W],
        [W, F, T, F, F, F, F, F, F, W],
        [W, F, F, F, W, F, F, B, F, W],
        [W, F, F, F, F, F, W, F, F, W],
        [W, F, F, W, F, F, F, F, F, W],
        [W, F, F, F, F, F, F, W, F, W],
        [W, F, B, F, F, W, B, F, T, W],
        [W, T, F, P, F, F, F, F, F, W],
        [W, W, W, W, W, W, W, W, W, W],
    ]
}

# Hard 3 (11x9, 4 boxes) - The gauntlet with many walls
hard3 = {
    "w": 11, "h": 10,
    "data": [
        [W, W, W, W, W, W, W, W, W, W, W],
        [W, F, F, T, W, F, F, F, F, F, W],
        [W, F, B, F, F, F, W, F, B, F, W],
        [W, F, F, W, F, F, F, W, F, F, W],
        [W, W, F, F, W, F, F, F, F, T, W],
        [W, F, F, W, F, F, W, F, F, F, W],
        [W, T, F, F, F, F, F, W, F, F, W],
        [W, F, F, B, F, W, F, F, B, F, W],
        [W, F, F, F, F, F, F, F, T, F, W],
        [W, F, P, F, F, F, F, F, F, F, W],
        [W, W, W, W, W, W, W, W, W, W, W],
    ]
}

def parse_level(data, w, h):
    player = None
    boxes = set()
    targets = set()
    walls = set()
    for y in range(h):
        for x in range(w):
            t = data[y][x]
            if t == W: walls.add((x, y))
            elif t == P: player = (x, y)
            elif t == PT: player = (x, y); targets.add((x, y))
            elif t == B: boxes.add((x, y))
            elif t == BT: boxes.add((x, y)); targets.add((x, y))
            elif t == T: targets.add((x, y))
    return player, frozenset(boxes), frozenset(targets), frozenset(walls)


def solve(name, level_data):
    w, h = level_data["w"], level_data["h"]
    # handle if data has h+1 rows (extra row)
    actual_h = len(level_data["data"])
    h = actual_h
    player, boxes, targets, walls = parse_level(level_data["data"], w, h)
    
    print(f"\nSolving {name} ({w}x{h}, {len(boxes)} boxes, {len(targets)} targets)")
    
    if len(boxes) != len(targets):
        print(f"  INVALID: {len(boxes)} boxes but {len(targets)} targets!")
        return False, ""
    if player is None:
        print(f"  INVALID: no player!")
        return False, ""
    
    start = (player, boxes)
    queue = deque([(start, "")])
    visited = {start}
    
    dirs = {'U': (0, -1), 'D': (0, 1), 'L': (-1, 0), 'R': (1, 0)}
    iterations = 0
    max_iter = 15_000_000
    
    while queue and iterations < max_iter:
        iterations += 1
        (px, py), cur_boxes = queue[0][0]
        path = queue[0][1]
        queue.popleft()
        
        if cur_boxes == targets:
            print(f"  SOLVABLE! {len(path)} moves, {iterations} states")
            print(f"  Solution: {path}")
            return True, path
        
        for dname, (dx, dy) in dirs.items():
            nx, ny = px + dx, py + dy
            if (nx, ny) in walls: continue
            
            new_boxes = cur_boxes
            if (nx, ny) in cur_boxes:
                bx, by = nx + dx, ny + dy
                if (bx, by) in walls or (bx, by) in cur_boxes: continue
                if (bx, by) not in targets:
                    # Corner deadlock
                    wl = lambda xx, yy: (xx, yy) in walls
                    if (wl(bx-1,by) and wl(bx,by-1)) or (wl(bx-1,by) and wl(bx,by+1)) or \
                       (wl(bx+1,by) and wl(bx,by-1)) or (wl(bx+1,by) and wl(bx,by+1)):
                        continue
                new_boxes = frozenset((cur_boxes - {(nx, ny)}) | {(bx, by)})
            
            new_state = ((nx, ny), new_boxes)
            if new_state not in visited:
                visited.add(new_state)
                queue.append((new_state, path + dname))
    
    if iterations >= max_iter:
        print(f"  TIMEOUT after {iterations} states")
    else:
        print(f"  UNSOLVABLE! {iterations} states")
    return False, ""


for name, data in [("Hard-1", hard1), ("Hard-2", hard2), ("Hard-3", hard3)]:
    solve(name, data)
