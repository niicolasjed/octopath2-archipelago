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
- [Archipelago](https://archipelago.gg) 0.6.8+

## Installation

### 1. Install the mod (C++ / UE4SS)

1. Copy the `OT2AP` folder into `<game folder>\Octopath_Traveler2\Binaries\Win64\Mods\`
2. Add `OT2AP : 1` to `mods.txt` in that same `Mods` folder (create the line if it doesn't exist)
3. Launch the game once to confirm the mod loads (check the UE4SS console for `OT2AP charge !`)

### 2. Install the APWorld

1. Open the Archipelago Launcher
2. Click **Install APWorld** and select `octopath2.apworld`
3. Restart the launcher

### 3. Generate and play

1. In the **Options Creator**, select **Octopath Traveler 2** and configure your options (starting character, whether to include equipment/treasures/materials/side quests, character randomization, etc.)
2. Generate your YAML and put it in Archipelago's `Players` folder, then generate the multiworld as usual
3. Host the server with your generated file
4. Run the included Python client (`Octopath2Client.py`) and `/connect` to your server
5. Launch Octopath Traveler 2 and start a **new game** — this is important, see note below

## Important notes

- **Always start a fresh save for a new Archipelago run.** The mod detects a new seed automatically and resets its local tracking files, but the in-game save itself should also be new to keep story progression consistent with what the multiworld expects.
- **Some areas become temporarily locked by the base game's story** (this is normal Octopath Traveler 2 behavior, not a mod bug). For example, Osvald's prologue prison is inaccessible again until you finish his full storyline. Grab every chest you can see before leaving a story-locked area.
- **Character safety mechanism**: if you're given a different starting character (or recruit one through the multiworld) than the one you picked at the title screen, the mod will **not** swap characters mid-scene — it waits until that character's own prologue is properly finished before making the switch, to avoid softlocking any story sequence.
- The mod writes a few small tracking files to `Mods\OT2AP\ap\` — you never need to touch these manually.

## Known limitations

- The victory condition (finishing the epilogue) has been tested for false positives but not yet confirmed by an actual full playthrough — feedback on this is very welcome.
- Quest items and story-critical items are intentionally **not** shuffled to avoid softlocks; only chests, side quests, equipment/consumables/treasures/materials, gold, and characters are part of the pool.
