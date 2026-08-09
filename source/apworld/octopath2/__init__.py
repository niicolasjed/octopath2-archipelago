# __init__.py
# Le CŒUR de l'APWorld : la classe World qui assemble items, locations,
# régions et logique, et que Archipelago utilise pour générer une partie.

from BaseClasses import Region, Entrance, Tutorial, ItemClassification
from worlds.AutoWorld import World, WebWorld

from .Items import OctopathItem, item_table, item_id_to_name
from .Locations import OctopathLocation, location_table, location_id_to_name
from .Options import Octopath2Options

from worlds.LauncherComponents import Component, components, Type, launch_subprocess

def launch_client():
    from .Client import launch
    launch_subprocess(launch, name="Octopath2Client")

components.append(Component("Octopath 2 Client", func=launch_client, component_type=Type.CLIENT))


# WebWorld = infos affichées sur le site web d'Archipelago (setup, guide...).
class Octopath2Web(WebWorld):
    tutorials = [Tutorial(
        "Multiworld Setup Guide",
        "A guide to setting up Octopath Traveler 2 for Archipelago multiworld.",
        "English",
        "setup_en.md",
        "setup/en",
        ["Nicolas"]
    )]


# La classe principale. Elle DOIT hériter de World.
class Octopath2World(World):
    """Octopath Traveler 2 as an Archipelago multiworld randomizer."""

    game = "Octopath Traveler 2"          # nom du jeu (doit être unique et constant)
    web = Octopath2Web()
    options_dataclass = Octopath2Options  # nos options (défini dans Options.py)
    options: Octopath2Options

    # Archipelago a besoin des tables nom <-> ID pour les items et locations.
    item_name_to_id = {name: data.code for name, data in item_table.items() if data.code is not None}
    location_name_to_id = {name: data.code for name, data in location_table.items() if data.code is not None}

    # --- Création des items ---
    def create_item(self, name: str) -> OctopathItem:
        # Crée un objet item à partir de son nom.
        data = item_table[name]
        return OctopathItem(name, data.classification, data.code, self.player)

    def create_items(self) -> None:
        item_pool = []

        # Ensembles pour filtrer selon les options
        from .equipment_apworld import equipment_items
        from .characters_apworld import character_items
        equipment_names = set(equipment_items.keys())
        character_names = set(character_items.keys())
        from .treasures_apworld import treasure_items
        treasure_names = set(treasure_items.keys())
        include_treasures = self.options.include_treasures.value
        from .materials_apworld import material_items
        material_names = set(material_items.keys())
        include_materials = self.options.include_materials.value

        include_equip = self.options.include_equipment.value
        randomize_chars = self.options.randomize_characters.value
        start_char_id = self.options.starting_character.value

        # Nom du perso de depart (pour l'exclure du pool si on randomise)
        # character_items : nom -> id_ap (656000X). L'id du jeu = X = id_ap - 6560000.
        start_char_name = None
        for cname, cap_id in character_items.items():
            if (cap_id - 6560000) == start_char_id:
                start_char_name = cname
                break

        for name in item_table.keys():
            # Equipement : seulement si l'option est active
            if name in equipment_names and not include_equip:
                continue
            if name in treasure_names and not include_treasures:
                continue
            if name in material_names and not include_materials:
                continue
            # Personnages : seulement si l'option randomize est active
            if name in character_names and not randomize_chars:
                continue
            # Le perso de depart n'est PAS dans le pool (on l'a des le debut)
            if name == start_char_name:
                continue
            item_pool.append(self.create_item(name))

        # Complete avec du filler jusqu'au nombre de locations
        from .subquests_apworld import subquest_locations
        num_locations = len(location_table) - 1  # -1 pour "Defeat Vide"
        if not self.options.include_subquests.value:
            num_locations -= len(subquest_locations)
        while len(item_pool) < num_locations:
            item_pool.append(self.create_item("100 Leaves"))

        # Securite : pas plus d'items que de locations
        item_pool = item_pool[:num_locations]

        self.multiworld.itempool += item_pool

    # --- Création des régions et locations ---
    def create_regions(self) -> None:
        # Une région = une zone du jeu. Pour le squelette, on fait une seule
        # région "Menu" (obligatoire, c'est le point de départ) qui contient
        # toutes nos locations.
        menu_region = Region("Menu", self.player, self.multiworld)

        # On ajoute chaque location à la région Menu.
        from .subquests_apworld import subquest_locations
        include_subquests = self.options.include_subquests.value
        for loc_name, loc_data in location_table.items():
            if loc_name in subquest_locations and not include_subquests:
                continue
            location = OctopathLocation(self.player, loc_name, loc_data.code, menu_region)
            menu_region.locations.append(location)

        self.multiworld.regions.append(menu_region)

        # Item de victoire, place sur la location "Defeat Vide" (deja creee par la boucle)
        victory_location = self.multiworld.get_location("Defeat Vide", self.player)
        victory_item = OctopathItem("Victory", ItemClassification.progression, 6560201, self.player)
        victory_location.place_locked_item(victory_item)

    # --- Objectif de victoire ---
    def set_rules(self) -> None:
        # Victoire = avoir recu l'item "Victory" (place sur la location "Defeat Vide",
        # que le mod declenche quand il detecte que le joueur a fini le jeu / vaincu Vide)
        self.multiworld.completion_condition[self.player] = lambda state: state.has("Victory", self.player)

    # --- Table coffre -> item place (calculee APRES le remplissage) ---
    def post_fill(self) -> None:
        from .chests_apworld import chest_locations
        chest_ids = set(chest_locations.values())
        self.chest_items = {}
        for loc in self.multiworld.get_locations(self.player):
            if loc.address is None or loc.address not in chest_ids:
                continue
            item = loc.item
            if item is None or item.code is None:
                continue
            # item.code si l'objet est A NOUS (donnable nativement par le coffre),
            # 0 si l'objet appartient a un autre joueur (coffre silencieux)
            self.chest_items[loc.address] = item.code if item.player == self.player else 0

    # --- Donnees envoyees au client a la connexion ---
    def fill_slot_data(self) -> dict:
        return {
            "starting_character": self.options.starting_character.value,
            "randomize_characters": self.options.randomize_characters.value,
            "include_equipment": self.options.include_equipment.value,
            "chest_items": getattr(self, "chest_items", {}),
        }