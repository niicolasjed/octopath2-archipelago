r"""
Client.py - Archipelago client for Octopath Traveler 2 (bundled with the APWorld)

Bridges:
  - the Archipelago server (network, handled by CommonContext)
  - the C++ mod in the game (via shared text files)

Exchange files (in <game>\Binaries\Win64\Mods\OT2AP\ap\):
  - ap_items.txt  : the CLIENT writes items received from the server; the MOD reads and gives them
  - ap_checks.txt : the MOD writes completed checks; the CLIENT reads and sends them
"""

import asyncio
import os

from CommonClient import CommonContext, server_loop, gui_enabled, ClientCommandProcessor, logger
import Utils

# These are set once, inside launch(), so that just IMPORTING this module
# (e.g. when the server loads the APWorld to generate a game) never prompts
# for a path or touches the filesystem.
GAME_DIR = None
ITEMS_FILE = None
CHECKS_FILE = None

CONFIG_FILE = os.path.join(Utils.user_path(), "octopath2_client_config.txt")


def resolve_game_dir() -> str:
    """Find (or ask for, once) the game's Win64 folder, and return the
    Mods\\OT2AP\\ap subfolder path, creating it if needed.
    Uses a GUI folder picker (not console input()) since this client may be
    launched without an attached console (e.g. from the Archipelago Launcher)."""
    win64_dir = None

    if os.path.exists(CONFIG_FILE):
        try:
            with open(CONFIG_FILE, "r", encoding="utf-8") as f:
                saved = f.read().strip()
            if saved and os.path.isdir(saved):
                win64_dir = saved
        except Exception:
            pass

    if not win64_dir:
        import tkinter as tk
        from tkinter import filedialog, messagebox

        root = tk.Tk()
        root.withdraw()
        root.attributes("-topmost", True)

        messagebox.showinfo(
            "Octopath Traveler 2 - First time setup",
            "Please select your game's Win64 folder.\n\n"
            r"Example: ...\Octopath_Traveler2\Octopath_Traveler2\Binaries\Win64",
            parent=root,
        )

        while not win64_dir:
            selected = filedialog.askdirectory(
                title="Select Octopath Traveler 2's Win64 folder", parent=root
            )
            if not selected:
                messagebox.showerror(
                    "Octopath Traveler 2",
                    "You need to select a folder to continue.",
                    parent=root,
                )
                continue
            if os.path.isdir(selected):
                win64_dir = selected
            else:
                messagebox.showerror(
                    "Octopath Traveler 2",
                    "That folder doesn't seem to exist, please try again.",
                    parent=root,
                )

        root.destroy()

    try:
        with open(CONFIG_FILE, "w", encoding="utf-8") as f:
            f.write(win64_dir)
    except Exception as e:
        logger.error(f"[OT2] Could not save config file: {e}")

    ap_dir = os.path.join(win64_dir, "Mods", "OT2AP", "ap")
    os.makedirs(ap_dir, exist_ok=True)
    return ap_dir


class Octopath2CommandProcessor(ClientCommandProcessor):
    pass


class Octopath2Context(CommonContext):
    game = "Octopath Traveler 2"
    command_processor = Octopath2CommandProcessor
    tags = {"AP"}

    def __init__(self, server_address, password):
        super().__init__(server_address, password)
        self.items_already_given = 0
        self.items_handling = 0b111
        self.sent_checks = set()

    async def server_auth(self, password_requested: bool = False):
        if password_requested and not self.password:
            await super().server_auth(password_requested)
        await self.get_username()
        await self.send_connect()

    def on_package(self, cmd: str, args: dict):
        super().on_package(cmd, args)

        if cmd == "RoomInfo":
            self.ot2_seed_name = args.get("seed_name", "")

        if cmd == "Connected":
            current_seed = getattr(self, "ot2_seed_name", "") or ""
            seed_file = os.path.join(GAME_DIR, "ap_seed.txt")
            last_seed = None
            try:
                with open(seed_file, "r") as f:
                    last_seed = f.read().strip()
            except Exception:
                pass

            if current_seed and current_seed != last_seed:
                logger.info(f"[OT2] New game detected (different seed) -> resetting tracking files")
                for fname in ["ap_checks.txt", "ap_items.txt", "ap_given.txt", "ap_quests_sent.txt"]:
                    try:
                        open(os.path.join(GAME_DIR, fname), "w").close()
                    except Exception as e:
                        logger.error(f"[OT2] error resetting {fname}: {e}")
                self.sent_checks = set()
                try:
                    with open(seed_file, "w") as f:
                        f.write(current_seed)
                except Exception as e:
                    logger.error(f"[OT2] error writing seed: {e}")

            slot_data = args.get("slot_data", {})
            start_char = slot_data.get("starting_character", 7)
            logger.info(f"[OT2] Connected! slot_data={slot_data}, start_char={start_char}")
            try:
                with open(os.path.join(GAME_DIR, "ap_start_char.txt"), "w") as f:
                    f.write(str(start_char))
            except Exception as e:
                logger.error(f"[OT2] error writing start_char: {e}")

            chest_items = slot_data.get("chest_items", {})
            try:
                with open(os.path.join(GAME_DIR, "ap_chest_items.txt"), "w") as f:
                    for loc_id, item_id in chest_items.items():
                        f.write(f"{loc_id};{item_id}\n")
                logger.info(f"[OT2] ap_chest_items.txt written: {len(chest_items)} chests")
            except Exception as e:
                logger.error(f"[OT2] error writing chest_items: {e}")

            

        if cmd == "ReceivedItems":
            self.write_received_items()

    def write_received_items(self):
        try:
            with open(ITEMS_FILE, "w", encoding="utf-8") as f:
                for network_item in self.items_received:
                    f.write(f"{network_item.item}\n")
            logger.info(f"[OT2] {len(self.items_received)} items written for the game")
        except Exception as e:
            logger.error(f"[OT2] Error writing items: {e}")


async def check_reader_loop(ctx: Octopath2Context):
    logged_once = False
    while not ctx.exit_event.is_set():
        try:
            if ctx.slot is None:
                await asyncio.sleep(0.5)
                continue
            if os.path.exists(CHECKS_FILE):
                if not logged_once:
                    logger.info(f"[OT2] Watching: {CHECKS_FILE}")
                    logged_once = True
                with open(CHECKS_FILE, "r", encoding="utf-8") as f:
                    lines = f.read().splitlines()
                new_locations = []
                for line in lines:
                    line = line.strip()
                    if line and line not in ctx.sent_checks:
                        new_locations.append(int(line))
                        ctx.sent_checks.add(line)
                if new_locations:
                    await ctx.send_msgs([{
                        "cmd": "LocationChecks",
                        "locations": new_locations
                    }])
                    logger.info(f"[OT2] Checks sent: {new_locations}")
            else:
                if not logged_once:
                    logger.info(f"[OT2] File NOT FOUND: {CHECKS_FILE}")
                    logged_once = True
        except Exception as e:
            logger.error(f"[OT2] Error reading checks: {e}")
        await asyncio.sleep(0.5)


async def main():
    Utils.init_logging("Octopath2Client")

    ctx = Octopath2Context(None, None)
    ctx.server_task = asyncio.create_task(server_loop(ctx), name="server loop")

    asyncio.create_task(check_reader_loop(ctx), name="check reader")

    if gui_enabled:
        ctx.run_gui()
    ctx.run_cli()

    await ctx.exit_event.wait()
    await ctx.shutdown()


def launch(*args):
    """Entry point called by the Archipelago launcher (via LauncherComponents)."""
    global GAME_DIR, ITEMS_FILE, CHECKS_FILE
    GAME_DIR = resolve_game_dir()
    ITEMS_FILE = os.path.join(GAME_DIR, "ap_items.txt")
    CHECKS_FILE = os.path.join(GAME_DIR, "ap_checks.txt")

    Utils.local_path = Utils.user_path
    import colorama
    colorama.init()
    asyncio.run(main())
    colorama.deinit()


if __name__ == "__main__":
    launch()