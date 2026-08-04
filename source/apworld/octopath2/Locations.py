# Locations.py
# Définit toutes les "locations" : les endroits où un item peut être trouvé.
# Pour Octopath 2, ce sont les coffres (qu'on sait détecter par leur Position !),
# et plus tard les PNJ, les récompenses de boss, etc.

from BaseClasses import Location
import typing


class LocationData(typing.NamedTuple):
    code: typing.Optional[int]  # ID unique de la location (None = event)
    region: str                 # dans quelle "région" se trouve cette location


class OctopathLocation(Location):
    game: str = "Octopath Traveler 2"


# Comme pour les items, chaque location a un ID unique et stable.
BASE_ID = 6520000

# Le dictionnaire de nos locations : nom -> LocationData
# Pour le squelette, on met quelques coffres.
# On les nomme de façon lisible, et on les rattache à une région (ici "Menu" pour tout,
# on affinera les régions plus tard).
#
# NOTE : le lien entre ce nom de location et la POSITION réelle du coffre en jeu
# (celle qu'on lit dans le mod C++, ex: X=-3500 Y=4450 Z=499) se fera plus tard,
# côté mod, via une table de correspondance. Ici on ne fait que déclarer les locations.
from .chests_apworld import chest_locations

location_table: typing.Dict[str, LocationData] = {
    label: LocationData(loc_id, "Menu")
    for label, loc_id in chest_locations.items()
}

# Locations de recrutement des personnages (interception du recrutement normal)
recruitment_locations = {
    "Recrutement Hikari": 6560101,
    "Recrutement Ochette": 6560102,
    "Recrutement Castti": 6560103,
    "Recrutement Partitio": 6560104,
    "Recrutement Temenos": 6560105,
    "Recrutement Osvald": 6560106,
    "Recrutement Throne": 6560107,
    "Recrutement Agnea": 6560108,
}
for _name, _loc_id in recruitment_locations.items():
    location_table[_name] = LocationData(_loc_id, "Menu")

from .subquests_apworld import subquest_locations
for _name, _loc_id in subquest_locations.items():
    location_table[_name] = LocationData(_loc_id, "Menu")

# Location de victoire (declenchee quand le mod detecte l'epilogue termine)
location_table["Defeat Vide"] = LocationData(6560200, "Menu")

# Table inversée : ID -> nom
location_id_to_name: typing.Dict[int, str] = {
    data.code: name for name, data in location_table.items() if data.code is not None
}
