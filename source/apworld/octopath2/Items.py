# Items.py
# Définit tous les "items" de notre monde : ce qui peut être reçu/distribué
# dans le multiworld Archipelago.

from BaseClasses import Item, ItemClassification
import typing


# Un ItemData regroupe les infos d'un item : son code (ID unique) et sa "classification".
# La classification dit à Archipelago si l'item est important pour la progression.
class ItemData(typing.NamedTuple):
    code: typing.Optional[int]          # ID unique de l'item (None = item "event", sans code)
    classification: ItemClassification  # progression / useful / filler


# Notre classe Item concrète (Archipelago attend une sous-classe de Item)
class OctopathItem(Item):
    game: str = "Octopath Traveler 2"


# IMPORTANT : chaque item a besoin d'un ID numérique UNIQUE et STABLE.
# On choisit une "base" arbitraire (ici 6520000) et on incrémente.
# Ces IDs sont internes à Archipelago, ils n'ont RIEN à voir avec les ItemId du jeu.
BASE_ID = 6520000

# Le dictionnaire de tous nos items : nom -> ItemData
# Pour le squelette minimal, on met quelques objets + de l'or.
item_table: typing.Dict[str, ItemData] = {}

# On ajoute les 189 consommables extraits du jeu
from .items_apworld import consumable_items
for _name, _ap_id in consumable_items.items():
    item_table[_name] = ItemData(_ap_id, ItemClassification.filler)

# Equipement (ajoute au table, mais mis dans le pool seulement si l'option est active)
from .equipment_apworld import equipment_items
for _name, _ap_id in equipment_items.items():
    item_table[_name] = ItemData(_ap_id, ItemClassification.filler)

# On ajoute des items d'or de montants varies
gold_items = {
    "50 Leaves":   6540000,
    "100 Leaves": 6540001,
    "200 Leaves":  6540002,
    "500 Leaves":  6540003,
    "1000 Leaves": 6540004,
    "3000 Leaves": 6540005,
}
for _name, _ap_id in gold_items.items():
    item_table[_name] = ItemData(_ap_id, ItemClassification.filler)

# Personnages jouables (comme items, pour l'option "randomiser les persos")
from .characters_apworld import character_items
for _name, _ap_id in character_items.items():
    item_table[_name] = ItemData(_ap_id, ItemClassification.progression)

from .treasures_apworld import treasure_items
for _name, _ap_id in treasure_items.items():
    item_table[_name] = ItemData(_ap_id, ItemClassification.filler)

from .materials_apworld import material_items
for _name, _ap_id in material_items.items():
    item_table[_name] = ItemData(_ap_id, ItemClassification.filler)

# Table inversée pratique : ID -> nom (utile pour le réseau plus tard)
item_id_to_name: typing.Dict[int, str] = {
    data.code: name for name, data in item_table.items() if data.code is not None
}
