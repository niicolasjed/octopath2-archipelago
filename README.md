# Octopath Traveler 2 Archipelago — Installation Guide

This mod adds full [Archipelago](https://archipelago.gg) multiworld randomizer support to **Octopath Traveler 2**.

## What's included

- **892 treasure chests** as checks
- **104 side quests** as checks (detected automatically, safe — completing them normally never blocks your progress)
- **8 playable characters** as randomizable items, unlocked from your inventory or the multiworld
- **Equipment, consumables, treasures, and materials** as shuffleable items (all with real in-game names)
- **Gold** in various amounts
- **Goal**: defeat Vide the Wicked and reach the epilogue

## Requirements

- Octopath Traveler 2 (PC / Steam)
- [UE4SS](https://github.com/UE4SS-RE/RE-UE4SS) installed in the game folder
- [Archipelago](https://archipelago.gg) — developed against the `main` branch (post-0.6.7 release)

## Installation

### 1. Install the mod (C++ / UE4SS)

1. Copy the `OT2AP` folder into `<game folder>\Octopath_Traveler2\Binaries\Win64\Mods\`
2. Add `OT2AP : 1` to `mods.txt` in that same `Mods` folder (create the line if it doesn't exist)
3. Launch the game once to confirm the mod loads (check the UE4SS console for `OT2AP charge !`)

### 2. Install the APWorld

1. Open the Archipelago Launcher
2. Click **Install APWorld** and select `octopath2.apworld`
3. Restart the launcher

If the option isn't there, copy `octopath2.apworld` directly into the
`custom_worlds` folder of your Archipelago installation.

### 3. Generate and play

1. In the **Options Creator**, select **Octopath Traveler 2** and configure your options (starting character, whether to include equipment/treasures/materials/side quests, character randomization, etc.)
2. Generate your YAML and put it in Archipelago's `Players` folder, then generate the multiworld as usual
3. Host the server with your generated file
4. In the Archipelago Launcher, search for "Octopath 2 Client" and click **Open** — no separate script needed, the client is bundled inside the APWorld
   - **First time only**: a window will pop up asking you to select your game's `Win64` folder (e.g. `...\Octopath_Traveler2\Octopath_Traveler2\Binaries\Win64`). This is saved automatically so you won't be asked again.
5. `/connect` to your server, then launch Octopath Traveler 2 and start a **new game**

Connect before starting your in-game save — the client resets its tracking
   files when it detects a new seed.

## Important notes

- **Always start a fresh save for a new Archipelago run.** The client automatically detects when you connect to a different seed than last time and resets the mod's tracking files for you. You should still start a genuinely new in-game save as well, to keep story progression consistent with what the multiworld expects.
- **Some areas become temporarily locked by the base game's story** (this is normal Octopath Traveler 2 behavior, not a mod bug). For example, Osvald's prologue prison is inaccessible again until you finish his full storyline. Grab every chest you can see before leaving a story-locked area.
- **Character safety mechanism**: if you're given a different starting character (or recruit one through the multiworld) than the one you picked at the title screen, the mod will **not** swap characters mid-scene — it waits until that character's own prologue is properly finished before making the switch, to avoid softlocking any story sequence.
- The mod writes a few small tracking files to `Mods\OT2AP\ap\` — you never need to touch these manually.

## Known limitations

- The victory condition (finishing the epilogue) has been tested for false positives but not yet confirmed by an actual full playthrough — feedback on this is very welcome.
- Quest items and story-critical items are intentionally **not** shuffled to avoid softlocks; only chests, side quests, equipment/consumables/treasures/materials, gold, and characters are part of the pool.

## Troubleshooting

- **Game crashes on startup with the mod enabled**: this has happened intermittently for at least one user and was resolved (in their case) after enabling the UE4SS GUI console (`GuiConsoleEnabled = 1` / `GuiConsoleVisible = 1` in `UE4SS-settings.ini`, under `[Debug]`). However, this doesn't reproduce consistently, so it may not be the actual root cause — if you hit this crash, try that setting, but please also report it on the Discord thread with your `UE4SS.log` and a screenshot of the debug panel at the moment of the crash so we can investigate further.