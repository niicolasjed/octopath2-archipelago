# Options.py
# Defines the settings a player chooses before generating their game.

from dataclasses import dataclass
from Options import Choice, PerGameCommonOptions, Toggle


class StartingCharacter(Choice):
    """Which character you start with.
    Set to 'random' in your YAML for a random pick among the 8 heroes."""
    display_name = "Starting Character"
    option_hikari = 1
    option_ochette = 2
    option_castti = 3
    option_partitio = 4
    option_temenos = 5
    option_osvald = 6
    option_throne = 7
    option_agnea = 8
    default = 7  # Throne by default


class DeathLink(Toggle):
    """If enabled, when one player dies, all linked players die too.
    (standard Archipelago option)"""
    display_name = "Death Link"


class IncludeEquipment(Toggle):
    """Include equipment (weapons, armor) in the item pool."""
    display_name = "Include Equipment"


class RandomizeCharacters(Toggle):
    """Randomize playable characters: they are hidden in the multiworld
    and unlocked as you play, instead of being recruited normally."""
    display_name = "Randomize Characters"


class IncludeTreasures(Toggle):
    """Include valuable/collectible items (treasures) in the item pool."""
    display_name = "Include Treasures"


class IncludeMaterials(Toggle):
    """Include crafting materials in the item pool."""
    display_name = "Include Materials"


class IncludeSubQuests(Toggle):
    """Include side quests as checks. Completing a side quest normally
    sends a check instead of giving its usual reward directly; this never
    blocks story progression."""
    display_name = "Include Side Quests"


class Goal(Choice):
    """The victory condition for this game."""
    display_name = "Goal"
    option_defeat_vide = 0
    default = 0


# Archipelago expects a dataclass grouping all of the game's options.
@dataclass
class Octopath2Options(PerGameCommonOptions):
    starting_character: StartingCharacter
    #death_link: DeathLink
    include_equipment: IncludeEquipment
    randomize_characters: RandomizeCharacters
    include_treasures: IncludeTreasures
    include_materials: IncludeMaterials
    include_subquests: IncludeSubQuests
    goal: Goal