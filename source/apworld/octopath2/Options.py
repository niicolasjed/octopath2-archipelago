# Options.py
# Définit les réglages que le joueur choisit avant de générer sa partie.
# Pour le squelette minimal, on met une seule option d'exemple.

from dataclasses import dataclass
from Options import Choice, PerGameCommonOptions, Toggle


class StartingCharacter(Choice):
    """Quel personnage le joueur commence avec.
    Mettre sur 'random' dans le YAML pour un tirage au sort."""
    display_name = "Personnage de départ"
    option_hikari = 1
    option_ochette = 2
    option_castti = 3
    option_partitio = 4
    option_temenos = 5
    option_osvald = 6
    option_throne = 7
    option_agnea = 8
    default = 7  # Throné par défaut


class DeathLink(Toggle):
    """Si activé, quand un joueur meurt, tous les joueurs liés meurent aussi.
    (option classique d'Archipelago, on la met pour l'exemple)"""
    display_name = "Death Link"

class IncludeEquipment(Toggle):
    """Inclure les equipements (armes, armures) dans le pool d'items."""
    display_name = "Inclure l'equipement"

class RandomizeCharacters(Toggle):
    """Randomiser les personnages : ils sont caches dans le multiworld et se debloquent en jouant."""
    display_name = "Randomiser les personnages"

class IncludeTreasures(Toggle):
    """Inclure les objets de valeur (tresors) dans le pool d'items."""
    display_name = "Inclure les tresors"

class IncludeMaterials(Toggle):
    """Inclure les materiaux dans le pool d'items."""
    display_name = "Inclure les materiaux"

class Goal(Choice):
    """L'objectif de victoire de la partie."""
    display_name = "Objectif"
    option_defeat_vide = 0
    default = 0

class IncludeSubquests(Toggle):
    """Inclure les quetes secondaires comme checks (detectees automatiquement, sans risque de bloquer la progression)."""
    display_name = "Inclure les quetes secondaires"

# Archipelago attend une dataclass qui regroupe toutes les options du jeu.
@dataclass
class Octopath2Options(PerGameCommonOptions):
    goal: Goal
    starting_character: StartingCharacter
    include_equipment: IncludeEquipment
    randomize_characters: RandomizeCharacters
    include_treasures: IncludeTreasures
    include_materials: IncludeMaterials
    include_subquests: IncludeSubquests
    death_link: DeathLink
