# 🎮 cub3D - 42 School Raycasting Engine

## 📌 Overview

A Wolfenstein 3D-inspired 3D maze game using raycasting in C.

**Goal:** render a 3D maze from a 2D `.cub` map in real time, with textured walls, movement, rotation, and strict map validation.

**42 Curriculum Context:** this is the final graphics project of the 42 common core, designed to teach computer graphics, parsing, memory safety, and real-time event handling.

---

## ✨ Features

### Mandatory

- [x] Raycasting for 3D view
- [x] Wall textures
- [x] Player movement (WASD + rotation)
- [x] Map parsing (`.cub` files)

### Bonuses

- [ ] Sprite rendering
- [ ] Door opening/closing
- [ ] Minimap
- [ ] Mouse look

---

## 🛠 Installation

### Dependencies

- `cc` / `gcc`
- `make`
- X11 development libraries (`libX11`, `libXext`)
- `libm`
- MinilibX (vendored in `mlx_linux/`)
- `libft`-style helpers (implemented locally in `srcs/utils.c`)

### Build

```bash
git clone <repository-url>
cd cub3D
make
```

### Compilation flags

The project is built with:

```bash
-Wall -Wextra -Werror -Iincludes -Imlx_linux
```

The final binary links with:

```bash
-L. -lmlx -L/lib/x86_64-linux-gnu -l:libX11.so.6 -l:libXext.so.6 -lm
```

---

## ▶️ Usage

Run the game with a valid scene file:

```bash
./cub3D path/to/map.cub
```

### Example `.cub`

```cub
NO ./textures/north.xpm
SO ./textures/south.xpm
WE ./textures/west.xpm
EA ./textures/east.xpm
F 220,100,0
C 70,70,70

111111
100001
10N001
100001
111111
```

### Controls

- `W` / `S`: move forward / backward
- `A` / `D`: strafe left / right
- `←` / `→`: rotate
- `ESC`: exit

---

## 🧱 Project Structure

```text
.
├── includes/      # public headers
├── mlx_linux/     # MinilibX source
├── srcs/          # parser, renderer, app loop, helpers
├── Makefile
├── en.subject.pdf
└── README.md
```

### Main modules

- `srcs/scene.c`: parses `.cub`, validates the map, loads colors/textures
- `srcs/render.c`: raycasting, texture sampling, player movement
- `srcs/app.c`: MLX setup, event hooks, frame lifecycle
- `srcs/utils.c`: custom string/memory helpers
- `srcs/error.c`: error handling and scene path checks
- `srcs/main.c`: entry point

---

## 🧠 Key Concepts

### Why raycasting?

Raycasting is efficient for 2D-to-3D projection because each screen column casts one ray into the map. That gives a convincing first-person view without needing a full 3D engine.

### Core pipeline

```text
  .cub file
      |
      v
  parser -> validated map / textures / colors / player
      |
      v
  MLX window + frame buffer
      |
      v
  input -> movement/rotation -> raycasting -> textured columns
      |
      v
   on-screen frame
```

### Raycasting math

For each screen column:

```text
cameraX   = 2 * x / screenWidth - 1
rayDir    = playerDir + cameraPlane * cameraX
deltaDist = abs(1 / rayDir)
perpDist  = sideDist - deltaDist
height    = screenHeight / perpDist
```

### DDA traversal

```text
while no wall hit:
    if sideDistX < sideDistY:
        sideDistX += deltaDistX
        mapX += stepX
    else:
        sideDistY += deltaDistY
        mapY += stepY
```

### ASCII view of the idea

```text
Player
  P  --->  ray for one screen column
  |
  |      +---+---+---+---+
  v      |   |   |   |   |
         +---+---+---+---+
         |   |###|   |   |   # wall tile hit by the ray
         +---+---+---+---+
```

---

## 🔍 Implementation Details

### 1) Parsing

The parser reads the full file, splits it line by line, and accepts:

- `NO`, `SO`, `WE`, `EA` texture paths
- `F` and `C` RGB colors
- a closed map made of `0`, `1`, spaces, and exactly one spawn (`N`, `S`, `E`, or `W`)

This strict order matters because the renderer depends on having all textures, colors, and spawn data ready before the game loop starts.

#### Why validate early?

Failing fast prevents partial state from reaching the MLX layer, which avoids crashes and makes map errors easier to diagnose.

### 2) Raycasting

Each frame, the engine casts one ray per vertical screen column.

#### Pseudocode

```text
for x in screen_width:
    compute ray direction
    step through the map with DDA
    when a wall is hit:
        compute perpendicular distance
        compute wall slice height
        sample the correct texture column
        draw the vertical stripe
```

#### Deep Dive: how wall height is calculated

The wall appears taller when it is closer to the camera.

```text
lineHeight = screenHeight / perpWallDist
drawStart  = -lineHeight / 2 + screenHeight / 2
drawEnd    =  lineHeight / 2 + screenHeight / 2
```

Why this works:

- `perpWallDist` keeps the projection stable and avoids fish-eye distortion.
- A smaller distance gives a larger projected slice.
- A larger distance gives a smaller projected slice.

### 3) Texture mapping

After a wall hit, the engine computes the horizontal hit position on the wall and converts it into a texture column.

```text
wallX  = fractional hit position on the wall
texX   = wallX * textureWidth
texY   = interpolated from top to bottom of the wall slice
color  = texture[texY][texX]
```

This is why textures must be loaded before rendering starts: every wall slice depends on valid image data.

### 4) Movement and collision

The player moves on the map grid, not in free space.

- Forward/backward uses the direction vector
- Strafing uses the perpendicular vector
- Rotation updates both the direction vector and the camera plane

Collision checks happen before position updates so the player cannot walk through walls.

### 5) Memory and lifetime

The project destroys:

- the frame image
- all loaded textures
- the MLX window/context
- the parsed scene data

This prevents leaks during shutdown and keeps error paths safe.

---

## 🧩 Function Table

| Function | Purpose | Inputs | Outputs |
|---|---|---|---|
| `main` | Program entry point | `argc`, `argv` | Exit code |
| `scene_parse` | Parse and validate a `.cub` file | `t_app *`, path | `0` on success, `1` on error |
| `scene_destroy` | Free scene data | `t_app *` | None |
| `ft_error` | Print an error message | Message string | `1` |
| `scene_validate_path` | Check `.cub` extension and readability | Path | `0` on success, `1` on error |
| `app_init` | Initialize MLX and hooks | `t_app *` | `0` on success |
| `app_prepare` | Create frame and load textures | `t_app *` | `0` on success |
| `app_run` | Enter the MLX loop | `t_app *` | `0` |
| `app_close` | Stop the app and free resources | `t_app *` | `0` |
| `app_close_hook` | Hook wrapper for window close | `void *` | `0` |
| `app_loop` | Per-frame update/render callback | `void *` | `0` |
| `app_render` | Clear and redraw the frame | `t_app *` | `0` |
| `app_move_player` | Update player position/rotation | `t_app *`, delta time | `0` |
| `app_key_press` | Handle key-down events | Keycode, app | `0` |
| `app_key_release` | Handle key-up events | Keycode, app | `0` |
| `app_create_frame` | Allocate the framebuffer image | `t_app *` | `0` on success |
| `app_load_textures` | Load wall textures from XPM files | `t_app *` | `0` on success |
| `app_destroy_frame` | Destroy the frame image | `t_app *` | None |
| `app_destroy_textures` | Destroy loaded textures | `t_app *` | None |
| `app_draw_pixel` | Write one pixel to the frame | Image, x, y, color | `0` |
| `ft_strlen` | Measure string length | String | Length |
| `ft_memset` | Fill memory | Buffer, value, size | Pointer |
| `ft_calloc` | Allocate zeroed memory | Count, size | Pointer |
| `ft_strdup` | Duplicate a string | String | Heap copy |
| `ft_split` | Split a string | String, delimiter | `char **` |
| `ft_strtrim` | Trim whitespace | String | Heap copy |

---

## 🧪 Norminette Compliance

- Keep functions short and single-purpose.
- Keep headers minimal and include only what is needed.
- Prefer explicit cleanup paths for every allocation.
- Use `norminette includes/ srcs/ mlx_linux/` to check style locally.

Why this matters: 42 grading is style-sensitive, and Norminette compliance prevents avoidable point loss.

---

## 📚 Resources

- `en.subject.pdf` in this repository
- MinilibX for Linux
- X11 documentation
- 42 cub3D subject specifications

---

## ❓ FAQ

### Common Pitfalls

- **Open maps:** if flood fill reaches space outside the maze, the scene is rejected.
- **Multiple spawns:** only one player start is allowed.
- **Missing directives:** all four wall textures plus both colors must be present.
- **Texture leaks:** destroy every loaded MLX image on both success and error paths.
- **Wrong file format:** the scene must end in `.cub`.

### How to Extend

- **Add a minimap:** store a 2D overlay renderer that reads the validated map grid.
- **Add sprites:** load extra images, sort them by distance, and render them after walls.
- **Add doors:** introduce a new tile type and add interaction/state handling in the map.
- **Add mouse look:** translate horizontal mouse motion into player rotation.

### Troubleshooting

- If the window does not open, check that the X11 libraries are installed.
- If textures fail to load, verify the `.xpm` paths in the scene file.
- If the game exits immediately, read the error message printed to `stderr`.
