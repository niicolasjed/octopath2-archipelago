# Octopath Traveler 2 Setup Guide

## Required Software

- Octopath Traveler 2 (PC / Steam)
- [UE4SS](https://github.com/UE4SS-RE/RE-UE4SS) installed in your game folder
- [Archipelago](https://archipelago.gg)
- The Octopath Traveler 2 APWorld and mod files

## Installing UE4SS

The mod runs on top of UE4SS, so install that first.

1. Download the latest UE4SS release.
2. Extract it into `<game folder>\Octopath_Traveler2\Binaries\Win64\`.
3. Launch the game once. If UE4SS is working, a console window appears alongside
   the game. Close the game before continuing.

## Installing the Mod

1. Copy the `OT2AP` folder into
   `<game folder>\Octopath_Traveler2\Binaries\Win64\Mods\`.
2. Open `mods.txt` in that same `Mods` folder and add this line:

   ```
   OT2AP : 1
   ```

3. Launch the game once and check the UE4SS console for the mod's startup message.
   If you see it, the mod is loaded correctly. Close the game before continuing.

## Installing the APWorld

1. Open the Archipelago Launcher.
2. Click **Install APWorld** and select `octopath2.apworld`.
3. Restart the Launcher.

## Configuring Your Game

Open the **Options Creator** from the Archipelago website or Launcher, select
**Octopath Traveler 2**, and choose your settings. The main ones:

- **Starting character** — which of the eight travelers you begin with.
- **Randomize characters** — whether the other seven are shuffled into the item pool.
- **Include equipment / treasures / materials** — which item categories join the pool.
- **Include side quests** — adds 104 side quest completions as checks.

Export your YAML and place it in Archipelago's `Players` folder, then generate the
multiworld as usual.

## Joining a Multiworld Game

1. Start the Archipelago server with your generated file.
2. In the Archipelago Launcher, find **Octopath 2 Client** and click **Open**.
   The client ships inside the APWorld, so there is no separate script to download.
   - **First launch only:** a window asks you to select your game's `Win64` folder
     (for example `...\Octopath_Traveler2\Octopath_Traveler2\Binaries\Win64`).
     This is saved, so you will not be asked again.
3. Use `/connect` in the client to join your server.
4. **Wait until the client is connected**, then launch Octopath Traveler 2 and
   start a **new game**.

Connecting before you start playing matters: the client resets its tracking files
when it detects a new seed, so checks collected before connecting can be lost.

## What Gets Randomized

- **892 treasure chests**
- **104 side quests**, detected automatically — completing them normally never
  blocks your progression
- **8 playable characters**, unlocked from your inventory or received from the
  multiworld
- **Equipment, consumables, treasures, and materials**, all using their real
  in-game names
- **Gold**, in various amounts

**Goal:** defeat Vide the Wicked and reach the epilogue.

Quest items and story-critical items are deliberately left out of the pool to
avoid softlocks.

## Things to Know Before Playing

**Always start a fresh in-game save for a new run.** The client detects when you
connect to a different seed and resets the mod's tracking files automatically, but
your Octopath save must match what the multiworld expects.

**Some areas lock themselves behind story progress.** This is base-game behaviour,
not a mod bug — Osvald's prologue prison, for instance, stays inaccessible until
his storyline is complete. Collect every chest you can see before leaving an area
tied to a story sequence.

**Character swaps wait for a safe moment.** If the multiworld gives you a different
starting character than the one you picked at the title screen, the mod will not
swap mid-scene. It waits until that character's own prologue has properly finished,
so no story sequence can softlock.

**Tracking files** are written to `Mods\OT2AP\ap\`. You never need to edit these
by hand.

## Troubleshooting

**The game crashes on startup with the mod enabled.**
One user hit this and resolved it by enabling the UE4SS GUI console — set
`GuiConsoleEnabled = 1` and `GuiConsoleVisible = 1` under `[Debug]` in
`UE4SS-settings.ini`. This does not reproduce consistently, so it may not be the
real cause. If it happens to you, try that setting, and please report it on the
Discord thread with your `UE4SS.log` and a screenshot of the debug panel at the
moment of the crash.

**Checks are not reaching the server.**
Confirm the client shows a `Watching:` line pointing at your `Mods\OT2AP\ap\`
folder. If the path is wrong, delete the saved config file and relaunch the client
to pick the folder again.

**The mod does not appear to load.**
Check that `mods.txt` contains the `OT2AP : 1` line and that the `OT2AP` folder
sits directly inside `Mods`, not nested one level deeper.