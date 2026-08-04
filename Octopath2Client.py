"""
Octopath2Client.py - Client Archipelago custom pour Octopath Traveler 2

Ce client fait le PONT entre :
  - le serveur Archipelago (via le reseau, gere par CommonContext)
  - le mod C++ dans le jeu (via 2 fichiers texte partages)

Fichiers d'echange (dans le dossier Win64 du jeu) :
  - ap_items.txt  : le CLIENT ecrit les items recus du serveur ; le MOD les lit et les donne
  - ap_checks.txt : le MOD ecrit les checks faits (coffres ouverts) ; le CLIENT les lit et les envoie

Lancement :
  python Octopath2Client.py
Puis dans le client : /connect localhost:38281
"""

import asyncio
import os

# On importe la base commune des clients Archipelago (gere tout le reseau)
from CommonClient import CommonContext, server_loop, gui_enabled, ClientCommandProcessor, logger
import Utils

# Chemin du dossier d'echange avec le mod C++ (le dossier Win64 du jeu).
# ADAPTE ce chemin si ton installation est differente !
GAME_DIR = r"C:\Program Files (x86)\Steam\steamapps\common\Octopath_Traveler2\Octopath_Traveler2\Binaries\Win64\Mods\OT2AP\ap"
ITEMS_FILE = os.path.join(GAME_DIR, "ap_items.txt")
CHECKS_FILE = os.path.join(GAME_DIR, "ap_checks.txt")


class Octopath2CommandProcessor(ClientCommandProcessor):
    """Gere les commandes tapees dans le client (comme /connect)."""
    pass


class Octopath2Context(CommonContext):
    # Nom du jeu : DOIT correspondre exactement au 'game' de ton APWorld
    game = "Octopath Traveler 2"
    command_processor = Octopath2CommandProcessor
    # Tags de connexion (AP = client de jeu normal)
    tags = {"AP"}

    def __init__(self, server_address, password):
        super().__init__(server_address, password)
        # On retient combien d'items on a deja donnes au jeu (pour ne pas les redonner)
        self.items_already_given = 0
        # On dit au serveur qu'on veut recevoir TOUS les items
        # (les notres + ceux des autres joueurs). 0b111 = tout.
        self.items_handling = 0b111

    async def server_auth(self, password_requested: bool = False):
        # Fonction appelee pour s'authentifier aupres du serveur.
        if password_requested and not self.password:
            await super().server_auth(password_requested)
        await self.get_username()
        await self.send_connect()

    def on_package(self, cmd: str, args: dict):
        if cmd == "Connected":
            # Detecter si c'est une NOUVELLE partie (seed different de la derniere fois)
            current_seed = args.get("seed_name", "")
            seed_file = os.path.join(GAME_DIR, "ap_seed.txt")
            last_seed = None
            try:
                with open(seed_file, "r") as f:
                    last_seed = f.read().strip()
            except Exception:
                pass

            if current_seed != last_seed:
                logger.info(f"[OT2] Nouvelle partie detectee (seed different) -> reinitialisation")
                for fname in ["ap_checks.txt", "ap_items.txt", "ap_given.txt", "ap_quests_sent.txt"]:
                    try:
                        open(os.path.join(GAME_DIR, fname), "w").close()
                    except Exception as e:
                        logger.error(f"[OT2] erreur reset {fname}: {e}")
                try:
                    with open(seed_file, "w") as f:
                        f.write(current_seed)
                except Exception as e:
                    logger.error(f"[OT2] erreur ecriture seed: {e}")

            slot_data = args.get("slot_data", {})
            start_char = slot_data.get("starting_character", 7)
            logger.info(f"[OT2] Connected recu ! slot_data={slot_data}, start_char={start_char}")
            try:
                with open(os.path.join(GAME_DIR, "ap_start_char.txt"), "w") as f:
                    f.write(str(start_char))
                logger.info(f"[OT2] ap_start_char.txt ecrit dans {GAME_DIR}")
            except Exception as e:
                logger.error(f"[OT2] erreur ecriture start_char: {e}")

        if cmd == "ReceivedItems":
            self.write_received_items()

    def write_received_items(self):
        """Ecrit dans ap_items.txt tous les items recus (que le mod C++ lira)."""
        # self.items_received est la liste de TOUS les items recus depuis le debut.
        try:
            with open(ITEMS_FILE, "w", encoding="utf-8") as f:
                for network_item in self.items_received:
                    # network_item.item = l'ID Archipelago de l'item
                    item_id = network_item.item
                    # On ecrit une ligne par item : son ID
                    f.write(f"{item_id}\n")
            logger.info(f"[OT2] {len(self.items_received)} items ecrits pour le jeu")
        except Exception as e:
            logger.error(f"[OT2] Erreur ecriture items : {e}")


async def check_reader_loop(ctx: Octopath2Context):
    """Boucle qui lit ap_checks.txt et envoie les checks au serveur."""
    sent_checks = set()
    logged_once = False
    while not ctx.exit_event.is_set():
        try:
            if os.path.exists(CHECKS_FILE):
                if not logged_once:
                    logger.info(f"[OT2] Surveille : {CHECKS_FILE}")
                    logged_once = True
                with open(CHECKS_FILE, "r", encoding="utf-8") as f:
                    for line in f:
                        line = line.strip()
                        if line and line not in sent_checks:
                            location_id = int(line)
                            await ctx.send_msgs([{
                                "cmd": "LocationChecks",
                                "locations": [location_id]
                            }])
                            sent_checks.add(line)
                            logger.info(f"[OT2] Check envoye : location {location_id}")
            else:
                if not logged_once:
                    logger.info(f"[OT2] Fichier PAS TROUVE : {CHECKS_FILE}")
                    logged_once = True
        except Exception as e:
            logger.error(f"[OT2] Erreur lecture checks : {e}")
        await asyncio.sleep(0.5)


async def main():
    Utils.init_logging("Octopath2Client")

    ctx = Octopath2Context(None, None)
    ctx.server_task = asyncio.create_task(server_loop(ctx), name="server loop")

    # On lance la boucle qui surveille les checks du mod
    asyncio.create_task(check_reader_loop(ctx), name="check reader")

    if gui_enabled:
        ctx.run_gui()
    ctx.run_cli()

    await ctx.exit_event.wait()
    await ctx.shutdown()


if __name__ == "__main__":
    Utils.local_path = Utils.user_path  # au cas ou
    import colorama
    colorama.init()
    asyncio.run(main())
    colorama.deinit()
