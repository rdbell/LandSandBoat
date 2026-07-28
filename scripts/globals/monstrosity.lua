-----------------------------------
-- Monstrosity (MON)
--
-- === How does it work? ===
--
-- Monstrosity is enabled through two mechanisms: setting your job to JOB_MON (23) and zoning.
-- Currently, there are some details that seemingly can only be populated at zone-time, so switching in/out
-- of MON mode is reliant on zoning.
--
-- When you zone your job will be checked, and if it is JOB_MON, then PChar->m_PMonstrosity will get
-- populated with your relevant Monstrosity data from table defined in char_monstrosity.sql. If you don't have
-- this information yet, it'll be created and saved for you with the defaults (the starting 3 MONs and the basic instincts).
--
-- Most other logic for determining stats, exp, exp ranges, traits, etc. will check you are either JOB_MON
-- or have m_PMonstrosity populated, and then look up what main/sub job your current species is, and then
-- forward that information into the relevant code for working out stats, etc.
--
-- IT IS VITAL that m_PMonstrosity is managed correctly, or that it's existance is constantly checked.
--
-- There is _a lot_ of client-side validation for MON, but we have all the information available server-side,
-- so we make sure to validate everything that comes through the zone_in and MON equip packets. It's also important
-- to validate all things for MON, because if they're invalid the client will get stuck in a state where they can't change
-- jobs, species, instincts, or names without GM intervention.
--
-- MONs main and subjob are in lock-step, so if you are a MNK15/NIN, the NIN will also be Lv15, and you'll get all the abilities,
-- traits, and stat contributions (TODO?) from both - except for the 2H which comes from the main job.
-----------------------------------
require('scripts/globals/npc_util')
require('scripts/globals/quests')
-----------------------------------
xi = xi or {}
xi.monstrosity = xi.monstrosity or {}

-----------------------------------
-- Enums
-----------------------------------

xi.monstrosity.species =
{
    RABBIT    = 1,
    BEHEMOTH  = 2,
    TIGER     = 3,
    SHEEP     = 4,
    RAM       = 5,
    DHALMEL   = 6,
    COEURL    = 7,
    OPO_OPO   = 8,
    MANTICORE = 9,
    BUFFALO   = 10,
    MARID     = 11,
    CERBERUS  = 12,
    GNOLE     = 13,

    FUNGUAR        = 15,
    TREANT_SAPLING = 16,
    MORBOL         = 17,
    MANDRAGORA     = 18,
    SABOTENDER     = 19,
    FLYTRAP        = 20,
    GOOBBUE        = 21,
    RAFFLESIA      = 22,
    PANOPT         = 23,

    BEE          = 27,
    BEETLE       = 28,
    CRAWLER      = 29,
    FLY          = 30,
    SCORPION     = 31,
    SPIDER       = 32,
    ANTLION      = 33,
    DIREMITE     = 34,
    CHIGOE       = 35,
    WAMOURACAMPA = 36,
    LADYBUG      = 37,
    GNAT         = 38,

    LIZARD      = 43,
    RAPTOR      = 44,
    ADAMANTOISE = 45,
    BUGARD      = 46,
    EFT         = 47,
    WIVRE       = 48,
    PEISTE      = 49,

    SLIME    = 52,
    HECTEYES = 53,
    FLAN     = 54,
    SLUG     = 56,
    SANDWORM = 57,
    LEECH    = 58,

    CRAB     = 60,
    PUGIL    = 61,
    SEA_MONK = 62,
    URAGNITE = 63,
    OROBON   = 64,
    RUSZOR   = 65,
    TOAD     = 66,

    BIRD       = 69,
    COCKATRICE = 70,
    ROC        = 71,
    BAT        = 72,
    HIPPOGRYPH = 73,
    APKALLU    = 74,
    COLIBRI    = 75,
    AMPHIPTERE = 76,

    ASTOLTIAN_SLIME  = 126,
    EORZEAN_SPRIGGAN = 127,
}

xi.monstrosity.variants =
{
    -- Rabbit
    ONYX_RABBIT      = 0,
    ALABASTER_RABBIT = 1,
    LAPINION         = 2,

    -- Behemoth
    ELASMOTH = 3,

    -- Tiger
    LEGENDARY_TIGER = 5,
    SMILODON        = 6,

    -- Sheep
    KARAKUL = 7,

    -- Coeurl
    LYNX = 10,
    COLLARED_LYNX = 11,

    -- Manticore
    LEGENDARY_MANTICORE = 12,

    -- Cerberus
    ORTHRUS = 13,

    -- Gnole
    BIPEDAL_GNOLE = 14,

    -- Funguar
    COPPERCAP = 15,

    -- Treant Sapling
    TREANT                = 16,
    FLOWERING_TREANT      = 17,
    SCARLET_TINGED_TREANT = 18,
    BARREN_TREANT         = 19,
    NECKLACED_TREANT      = 20,

    -- Morbol
    PYGMY_MORBOL = 21,
    SCARE_MORBOL = 22,
    AMERETAT     = 23,
    PURBOL       = 24,

    -- Mandragora
    KORRIGAN               = 25,
    LYCOPODIUM             = 26,
    PYGMY_MANDRAGORA       = 27,
    ADENIUM                = 28,
    PACHYPODIUM            = 29,
    ENLIGHTENED_MANDRAGORA = 30,
    NEW_YEAR_MANDRAGORA    = 31,

    -- Sabotender
    SABOTENDER_FLORIDO = 32,

    -- Rafflesia
    MITRASTEMA = 33,

    -- Bee
    VERMILLION_AND_ONYX_BEE = 34,
    ZAFFRE_BEE              = 35,

    -- Beetle
    ONYX_BEETLE    = 36,
    GAMBOGE_BEETLE = 37,

    -- Crawler
    ERUCA                 = 38,
    EMERALD_CRAWLER       = 39,
    PYGMY_EMERALD_CRAWLER = 40,

    -- Fly
    VERMILLION_FLY = 41,

    -- Scorpion
    SCOLOPENDRID         = 42,
    UNUSUAL_SCOLOPENDRID = 43,

    -- Spider
    RETICULATED_SPIDER         = 44,
    VERMILLION_AND_ONYX_SPIDER = 45,

    -- Antlion
    ONYX_ANTLION = 46,
    FORMICEROS   = 47,

    -- Diremite
    ARUNDIMITE = 48,

    -- Chigoe
    AZURE_CHIGOE = 49,

    -- Wamouracampa
    COILED_WAMOURACAMPA = 50,

    -- Wamoura
    WAMOURA       = 51,
    CORAL_WAMOURA = 52,

    -- Ladybug
    GOLD_LADYBUG = 53,

    -- Gnat
    MIDGE = 54,

    -- Lizard
    ASHEN_LIZARD = 59,

    -- Raptor
    EMERALD_RAPTOR    = 60,
    VERMILLION_RAPTOR = 61,

    -- Adamantoise
    PYGMY_ADAMANTOISE     = 62,
    LEGENDARY_ADAMANTOISE = 63,
    FERROMANTOISE         = 64,

    -- Bugard
    ABYSSOBUGARD = 65,

    -- Eft
    TARICHUK = 66,

    -- Wivre
    UNUSUAL_WIVRE = 67,

    -- Peiste
    SIBILUS = 68,

    -- Slime
    CLOT       = 73,
    GOLD_SLIME = 74,
    BOIL       = 75,

    -- Flan
    GOLD_FLAN  = 76,
    BLANCMANGE = 77,

    -- Sandworm
    PYGMY_SANDWORM = 78,
    GIGAWORM       = 79,

    -- Leech
    AZURE_LEECH = 80,
    OBDELLA     = 81,

    -- Crab
    VERMILLION_CRAB                 = 84,
    BASKET_BURDENED_CRAB            = 85,
    VERMILLION_BASKET_BURDENED_CRAB = 86,
    PORTER_CRAB                     = 87,

    -- Pugil
    JAGIL = 88,

    -- Sea Monk
    AZURE_SEA_MONK = 89,

    -- Uragnite
    LIMASCABRA = 90,

    -- Orobon
    PYGMY_OROBON = 91,
    OGREBON      = 92,

    -- Toad
    AZURE_TOAD      = 93,
    VERMILLION_TOAD = 94,

    -- Bird
    ONYX_BIRD = 95,

    -- Cockatrice
    ZIZ = 96,

    -- Roc
    LEGENDARY_ROC = 97,
    GAGANA        = 98,

    -- Bat
    BATS            = 99,
    VERMILLION_BAT  = 100,
    VERMILLION_BATS = 101,

    -- Apkallu
    INGUZA = 102,

    -- Colibri
    TOUCALIBRI = 103,

    -- Amphiptere
    SANGUIPTERE = 104,

    -- Slime
    SHE_SLIME   = 252,
    METAL_SLIME = 253,

    -- Spriggan
    SPRIGGAN_C = 254,
    SPRIGGAN_G = 255,
}

xi.monstrosity.purchasableInstincts =
{
    -- Default (0x1F)
    HUME_I   = 0,
    ELVAAN_I = 1,
    TARU_I   = 2,
    MITHRA_I = 3,
    GALKA_I  = 4,

    HUME_II   = 5,
    ELVAAN_II = 6,
    TARU_II   = 7,
    MITHRA_II = 8,
    GALKA_II  = 9,

    WAR = 10,
    MNK = 11,
    WHM = 12,
    BLM = 13,
    RDM = 14,
    THF = 15,
    PLD = 16,
    DRK = 17,
    BST = 18,
    BRD = 19,
    RNG = 20,
    SAM = 21,
    NIN = 22,
    DRG = 23,
    SMN = 24,
    BLU = 25,
    COR = 26,
    PUP = 27,
    DNC = 28,
    SCH = 29,
    GEO = 30,
    RUN = 31,
}

local limitBreakQuests =
{
    [xi.job.BLU] = { xi.questLog.AHT_URHGAN,  xi.quest.id.ahtUrhgan.THE_BEAST_WITHIN           },
    [xi.job.COR] = { xi.questLog.AHT_URHGAN,  xi.quest.id.ahtUrhgan.BREAKING_THE_BONDS_OF_FATE },
    [xi.job.PUP] = { xi.questLog.BASTOK,      xi.quest.id.bastok.ACHIEVING_TRUE_POWER          },
    [xi.job.DNC] = { xi.questLog.JEUNO,       xi.quest.id.jeuno.A_FURIOUS_FINALE               },
    [xi.job.SCH] = { xi.questLog.OTHER_AREAS, xi.quest.id.otherAreas.SURVIVAL_OF_THE_WISEST    },
    [xi.job.GEO] = { xi.questLog.ADOULIN,     xi.quest.id.adoulin.ELEMENTARY_MY_DEAR_SYLVIE    },
    [xi.job.RUN] = { xi.questLog.ADOULIN,     xi.quest.id.adoulin.ENDEAVORING_TO_AWAKEN        },
}

-- NOTE: Cost and granted species/variant are hardcoded into Terynon's event; however, the requirements
-- to get each of these purchasable MONs is not displayed, and can be modified to a different set or
-- level.  The requirements are limited to species!
local terynonMonData =
{
    [0] = -- Beasts
    {
        [0] =
        {
            monVariant   = xi.monstrosity.variants.LAPINION,
            infamyCost   = 7500,
            requirements =
            {
                { xi.monstrosity.species.RABBIT, 90 },
            },
        },

        [1] =
        {
            monSpecies = xi.monstrosity.species.SHEEP,
            infamyCost = 3000,
        },

        [2] =
        {
            monSpecies   = xi.monstrosity.species.BEHEMOTH,
            infamyCost   = 10000,
            requirements =
            {
                { xi.monstrosity.species.RABBIT,  75 },
                { xi.monstrosity.species.OPO_OPO, 75 },
                { xi.monstrosity.species.GNOLE,   75 },
            },
        },

        [3] =
        {
            monVariant   = xi.monstrosity.variants.ELASMOTH,
            infamyCost   = 25000,
            requirements =
            {
                { xi.monstrosity.species.BEHEMOTH, 50 },
            },
        },

        [4] =
        {
            monSpecies   = xi.monstrosity.species.CERBERUS,
            infamyCost   = 10000,
            requirements =
            {
                { xi.monstrosity.species.BUFFALO,   60 },
                { xi.monstrosity.species.MANTICORE, 60 },
                { xi.monstrosity.species.MARID,     60 },
                { xi.monstrosity.species.SHEEP,     60 },
                { xi.monstrosity.species.DHALMEL,   60 },
            },
        },

        [5] =
        {
            monVariant   = xi.monstrosity.variants.ORTHRUS,
            infamyCost   = 25000,
            requirements =
            {
                { xi.monstrosity.species.CERBERUS, 50 },
            },
        },
    },

    [1] = -- Plantoids
    {
        [0] =
        {
            monVariant   = xi.monstrosity.variants.PYGMY_MANDRAGORA,
            infamyCost   = 7500,
            requirements =
            {
                { xi.monstrosity.species.MANDRAGORA, 45 },
            },
        },

        [1] =
        {
            monSpecies = xi.monstrosity.species.TREANT,
            infamyCost = 3000,
        },

        [2] =
        {
            monVariant   = xi.monstrosity.variants.PYGMY_MORBOL,
            infamyCost   = 7500,
            requirements =
            {
                { xi.monstrosity.species.MORBOL, 1 },
            },
        },

        [3] =
        {
            monVariant   = xi.monstrosity.variants.PURBOL,
            infamyCost   = 15000,
            requirements =
            {
                { xi.monstrosity.species.MORBOL, 75 },
            },
        },
    },

    [2] = -- Vermin
    {
        [0] =
        {
            monVariant   = xi.monstrosity.variants.GOLD_LADYBUG,
            infamyCost   = 7500,
            requirements =
            {
                { xi.monstrosity.species.LADYBUG, 50 },
            },
        },

        [1] =
        {
            monSpecies = xi.monstrosity.species.BEETLE,
            infamyCost = 3000,
        },

        [2] =
        {
            monVariant   = xi.monstrosity.variants.UNUSUAL_SCOLOPENDRID,
            infamyCost   = 10000,
            requirements =
            {
                { xi.monstrosity.species.SCORPION, 60 },
            },
        },

        [3] =
        {
            monSpecies   = xi.monstrosity.species.ANTLION,
            infamyCost   = 7500,
            requirements =
            {
                { xi.monstrosity.species.SCORPION, 60 },
            },
        },

        [4] =
        {
            monVariant   = xi.monstrosity.variants.FORMICEROS,
            infamyCost   = 15000,
            requirements =
            {
                { xi.monstrosity.species.ANTLION, 60 },
            },
        },

        [5] =
        {
            monVariant   = xi.monstrosity.variants.PYGMY_EMERALD_CRAWLER,
            infamyCost   = 6000,
            requirements =
            {
                { xi.monstrosity.species.CRAWLER, 60 },
            },
        },

        [6] =
        {
            monVariant   = xi.monstrosity.variants.CORAL_WAMOURA,
            infamyCost   = 15000,
            requirements =
            {
                { xi.monstrosity.species.WAMOURACAMPA, 60 },
            },
        },

        [7] =
        {
            monSpecies   = xi.monstrosity.species.GNAT,
            infamyCost   = 5000,
            requirements =
            {
                { xi.monstrosity.species.LADYBUG,      50 },
                { xi.monstrosity.species.WAMOURACAMPA, 50 },
            },
        },
    },

    [3] = -- Lizards
    {
        [0] =
        {
            monVariant   = xi.monstrosity.species.UNUSUAL_WIVRE,
            infamyCost   = 7500,
            requirements =
            {
                { xi.monstrosity.species.WIVRE, 60 },
            },
        },

        [1] =
        {
            monSpecies   = xi.monstrosity.species.ADAMANTOISE,
            infamyCost   = 10000,
            requirements =
            {
                { xi.monstrosity.species.BUGARD, 60 },
                { xi.monstrosity.species.LIZARD, 60 },
                { xi.monstrosity.species.WIVRE,  60 },
            },
        },

        [2] =
        {
            monVariant   = xi.monstrosity.variants.FERROMANTOISE,
            infamyCost   = 20000,
            requirements =
            {
                { xi.monstrosity.species.ADAMANTOISE, 70 },
            },
        },

        [3] =
        {
            monSpecies = xi.monstrosity.species.RAPTOR,
            infamyCost = 3000,
        },

        [4] =
        {
            monSpecies   = xi.monstrosity.species.PEISTE,
            infamyCost   = 8000,
            requirements =
            {
                { xi.monstrosity.species.EFT,    50 },
                { xi.monstrosity.species.RAPTOR, 50 },
            },
        },

        [5] =
        {
            monVariant   = xi.monstrosity.variants.SIBILUS,
            infamyCost   = 15000,
            requirements =
            {
                { xi.monstrosity.species.PEISTE, 50 },
            },
        },
    },

    [4] = -- Amorphs
    {
        [0] =
        {
            monSpecies = xi.monstrosity.species.SLIME,
            infamyCost = 3000,
        },

        [1] =
        {
            monVariant   = xi.monstrosity.variants.BOIL,
            infamyCost   = 25000,
            requirements =
            {
                { xi.monstrosity.species.SLIME, 50 },
            },
        },

        [2] =
        {
            monVariant   = xi.monstrosity.variants.PYGMY_SANDWORM,
            infamyCost   = 10000,
            requirements =
            {
                { xi.monstrosity.species.SANDWORM, 1 },
            },
        },

        [3] =
        {
            monVariant   = xi.monstrosity.variants.GIGAWORM,
            infamyCost   = 25000,
            requirements =
            {
                { xi.monstrosity.species.SANDWORM, 60 },
            },
        },

        [4] =
        {
            monSpecies = xi.monstrosity.species.LEECH,
            infamyCost = 2000,
        },
    },

    [5] = -- Aquans
    {
        [0] =
        {
            monSpecies = xi.monstrosity.species.CRAB,
            infamyCost = 2000,
        },

        [1] =
        {
            monVariant   = xi.monstrosity.variants.BASKET_BURDENED_CRAB,
            infamyCost   = 20000,
            requirements =
            {
                { xi.monstrosity.species.CRAB, 1 },
            },
        },

        [2] =
        {
            monVariant   = xi.monstrosity.variants.VERMILLION_BASKET_BURDENED_CRAB,
            infamyCost   = 20000,
            requirements =
            {
                { xi.monstrosity.species.CRAB, 15 },
            },
        },

        [3] =
        {
            monVariant   = xi.monstrosity.variants.PORTER_CRAB,
            infamyCost   = 15000,
            requirements =
            {
                { xi.monstrosity.species.CRAB, 60 },
            },
        },

        [4] =
        {
            monSpecies = xi.monstrosity.species.PUGIL,
            infamyCost = 3000,
        },

        [5] =
        {
            monVariant   = xi.monstrosity.variants.LIMASCABRA,
            infamyCost   = 15000,
            requirements =
            {
                { xi.monstrosity.species.URAGNITE, 50 },
            },
        },

        [6] =
        {
            monVariant   = xi.monstrosity.variants.PYGMY_OROBON,
            infamyCost   = 10000,
            requirements =
            {
                { xi.monstrosity.species.OROBON, 1 },
            },
        },

        [7] =
        {
            monVariant   = xi.monstrosity.variants.OGREBON,
            infamyCost   = 18000,
            requirements =
            {
                { xi.monstrosity.species.OROBON, 50 },
            },
        },

        [8] =
        {
            monSpecies   = xi.monstrosity.species.RUSZOR,
            infamyCost   = 10000,
            requirements =
            {
                { xi.monstrosity.species.OROBON,   75 },
                { xi.monstrosity.species.URAGNITE, 75 },
            },
        },
    },

    [6] = -- Birds
    {
        [0] =
        {
            monSpecies = xi.monstrosity.species.COCKATRICE,
            infamyCost = 3000,
        },

        [1] =
        {
            monVariant   = xi.monstrosity.variants.GAGANA,
            infamyCost   = 15000,
            requirements =
            {
                { xi.monstrosity.species.ROC, 75 },
            },
        },

        [2] =
        {
            monSpecies = xi.monstrosity.species.BAT,
            infamyCost = 2000,
        },

        [3] =
        {
            monVariant   = xi.monstrosity.variants.INGUZA,
            infamyCost   = 15000,
            requirements =
            {
                { xi.monstrosity.species.APKALLU, 50 },
            },
        },

        [4] =
        {
            monSpecies = xi.monstrosity.species.COLIBRI,
            infamyCost = 5000,
            requirements =
            {
                { xi.monstrosity.species.BAT,  50 },
                { xi.monstrosity.species.BIRD, 45 },
            },
        },

        [5] =
        {
            monVariant   = xi.monstrosity.variants.TOUCALIBRI,
            infamyCost   = 15000,
            requirements =
            {
                { xi.monstrosity.species.COLIBRI, 50 },
            },
        },

        [6] =
        {
            monSpecies   = xi.monstrosity.species.AMPHIPTERE,
            infamyCost   = 10000,
            requirements =
            {
                { xi.monstrosity.species.COCKATRICE, 75 },
                { xi.monstrosity.species.ROC,        75 },
                { xi.monstrosity.species.HIPPOGRYPH, 75 },
            },
        },

        [7] =
        {
            monVariant   = xi.monstrosity.variants.SANGUIPTERE,
            infamyCost   = 20000,
            requirements =
            {
                { xi.monstrosity.species.AMPHIPTERE, 50 },
            },
        },
    },
}

xi.monstrosity.teleports =
{
    [xi.zone.EAST_RONFAURE] =
    {
        { 120,     0.5, -530, 192 },
        { 115, -59.684,  247,  16 },
    },

    [xi.zone.QUFIM_ISLAND] =
    {
        {  -2, -20.001, 324,  64 },
        { 161,     -20,  37, 192 },
    },

    [xi.zone.SOUTH_GUSTABERG] =
    {
        { -115, -0.136, -165, 64 },
    },

    [xi.zone.VALKURM_DUNES] =
    {
        { 838, 0, -162, 64 },
    },

    [xi.zone.WESTERN_ALTEPA_DESERT] =
    {
        { 685.548, -1.744, -50.395, 128 },
    },
}

-- NOTE: The zones in this list are not customisable, but the level caps are!
xi.monstrosity.belligerencyCaps =
{
    [xi.zone.BUBURIMU_PENINSULA] = 30,
    [xi.zone.XARCABARD]          = 60,
    [xi.zone.ULEGUERAND_RANGE]   = 90,
}

-----------------------------------
-- Helpers
-----------------------------------
-- Use xi.monstrosity.species
xi.monstrosity.startingMONData = function(choice)
    return
    {
        monstrosityId = choice,
        species       = choice,
    }
end

xi.monstrosity.unlockStartingMONs = function(player, choice)
    local data = xi.monstrosity.startingMONData(choice)
    player:setMonstrosityData(data)
end

-- Use xi.monstrosity.species
xi.monstrosity.speciesLevel = function(levels, species)
    return levels[species]
end

xi.monstrosity.getSpeciesLevel = function(player, species)
    return xi.monstrosity.speciesLevel(player:getMonstrosityData().levels, species)
end

-- Use xi.monstrosity.species
xi.monstrosity.speciesUnlocked = function(levels, species)
    return xi.monstrosity.speciesLevel(levels, species) > 0
end

xi.monstrosity.hasUnlockedSpecies = function(player, species)
    return xi.monstrosity.speciesUnlocked(player:getMonstrosityData().levels, species)
end

-- Use xi.monstrosity.species
xi.monstrosity.setSpeciesLevel = function(player, species, level)
    local data = player:getMonstrosityData()
    data.levels[species] = level
    player:setMonstrosityData(data)
end

-- Use xi.monstrosity.species
xi.monstrosity.unlockSpeciesData = function(levels, species)
    if levels[species] == 0 then
        levels[species] = 1
        return true
    end

    return false
end

xi.monstrosity.unlockSpecies = function(player, species)
    local data = player:getMonstrosityData()
    if xi.monstrosity.unlockSpeciesData(data.levels, species) then
        player:setMonstrosityData(data)
    end
end

-- Use xi.monstrosity.variants
xi.monstrosity.hasUnlockedVariant = function(player, variant)
    local data = player:getMonstrosityData()

    local byteOffset  = math.floor(variant / 8)
    local shiftAmount = variant % 8

    if byteOffset < 32 then
        return bit.band(data.variants[byteOffset] or 0, bit.lshift(0x01, shiftAmount)) > 0
    end

    return false
end

-- Use xi.monstrosity.variants
xi.monstrosity.unlockVariantData = function(variants, variant)
    local byteOffset   = math.floor(variant / 8)
    local shiftAmount  = variant % 8

    if byteOffset >= 32 then
        return false
    end

    local unlockMask = bit.lshift(0x01, shiftAmount)
    if bit.band(variants[byteOffset] or 0, unlockMask) > 0 then
        return false
    end

    variants[byteOffset] = bit.bor(variants[byteOffset] or 0, unlockMask)
    return true
end

xi.monstrosity.unlockVariant = function(player, variant)
    local data = player:getMonstrosityData()
    if xi.monstrosity.unlockVariantData(data.variants, variant) then
        player:setMonstrosityData(data)
    end
end

-- Purchasable instincts live in the four instinct bytes at offsets 20..23 of
-- the monstrosity data blob. These pure helpers own that addressing so it is
-- testable without a player.

-- Byte offset and bit position holding a purchasable instinct's ownership flag.
xi.monstrosity.instinctByteOffset = function(purchasableInstinctId)
    return 20 + math.floor(purchasableInstinctId / 8)
end

xi.monstrosity.instinctBitShift = function(purchasableInstinctId)
    return purchasableInstinctId % 8
end

-- Only offsets 20..23 hold instinct bytes; anything else is out of range.
xi.monstrosity.instinctByteOffsetValid = function(byteOffset)
    return byteOffset >= 20 and byteOffset < 24
end

-- Ownership flag for a purchasable instinct, read out of the instinct bytes.
-- Returns nil for an out-of-range instinct, matching upstream's fall-through.
xi.monstrosity.instinctPurchased = function(instinctBytes, purchasableInstinctId)
    local byteOffset  = xi.monstrosity.instinctByteOffset(purchasableInstinctId)
    local shiftAmount = xi.monstrosity.instinctBitShift(purchasableInstinctId)

    if not xi.monstrosity.instinctByteOffsetValid(byteOffset) then
        return nil
    end

    return bit.band(instinctBytes[byteOffset] or 0, bit.lshift(1, shiftAmount)) > 0
end

-- Sets a purchasable instinct's ownership flag in the instinct bytes, in place.
-- Returns whether the instinct was addressable.
xi.monstrosity.instinctSetPurchased = function(instinctBytes, purchasableInstinctId)
    local byteOffset  = xi.monstrosity.instinctByteOffset(purchasableInstinctId)
    local shiftAmount = xi.monstrosity.instinctBitShift(purchasableInstinctId)

    if not xi.monstrosity.instinctByteOffsetValid(byteOffset) then
        return false
    end

    instinctBytes[byteOffset] = bit.bor(instinctBytes[byteOffset] or 0, bit.lshift(0x01, shiftAmount))

    return true
end

-- Terynon's discount mask over the purchasable instincts from HUME_II up,
-- rebased so HUME_II is bit 0. The five default racial instincts below HUME_II
-- are excluded.
xi.monstrosity.purchasedInstinctsMask = function(isPurchased)
    local instinctMask = 0

    for _, purchasableInstinctId in pairs(xi.monstrosity.purchasableInstincts) do
        if
            purchasableInstinctId >= xi.monstrosity.purchasableInstincts.HUME_II and
            isPurchased(purchasableInstinctId)
        then
            instinctMask = utils.mask.setBit(instinctMask, purchasableInstinctId - xi.monstrosity.purchasableInstincts.HUME_II, true)
        end
    end

    return instinctMask
end

-- Terynon's limit-break mask over WAR..RUN, with each job at bit jobId-1.
xi.monstrosity.limitBreakMask = function(isCompleted)
    local limitMask = 0

    for jobId = xi.job.WAR, xi.job.RUN do
        if isCompleted(jobId) then
            limitMask = utils.mask.setBit(limitMask, jobId - 1, true)
        end
    end

    return limitMask
end

local function hasPurchasedInstinct(player, purchasableInstinctId)
    local data   = player:getMonstrosityData()
    local result = xi.monstrosity.instinctPurchased(data.instincts, purchasableInstinctId)

    if result == nil then
        print('byteOffset out of range')
    end

    return result
end

local function addPurchasedInstinct(player, purchasableInstinctId)
    local data = player:getMonstrosityData()

    if not xi.monstrosity.instinctSetPurchased(data.instincts, purchasableInstinctId) then
        print('byteOffset out of range')
    end

    player:setMonstrosityData(data)
end

-- When generating Terynon's mask for discounts, we need a bitmask for
-- specific jobs.  Since only one quest exists for pre-ToAU jobs, use
-- Maat's Cap tracking for those.
local function hasCompletedLimitBreak(player, jobId)
    if jobId <= xi.job.SMN then
        local maatsCap = player:getCharVar('maatsCap')

        return utils.mask.getBit(maatsCap, jobId - 1)
    else
        return player:hasCompletedQuest(unpack(limitBreakQuests[jobId]))
    end
end

-- Determines whether a Terynon MON offer unlocks something new and satisfies
-- all of its species-level prerequisites. Host player lookups are injected.
xi.monstrosity.purchaseRequirementsMet = function(selectedMonData, speciesLevel, hasUnlockedVariant)
    local eligibleSpecies = selectedMonData.monSpecies and speciesLevel(selectedMonData.monSpecies) == 0
    local eligibleVariant = selectedMonData.monVariant and not hasUnlockedVariant(selectedMonData.monVariant)

    if
        eligibleSpecies or
        eligibleVariant
    then
        if selectedMonData.requirements then
            for _, reqTable in ipairs(selectedMonData.requirements) do
                if speciesLevel(reqTable[1]) < reqTable[2] then
                    return false
                end
            end
        end

        return true
    end

    return false
end

local function hasPurchaseRequirements(player, monCategory, selectedMon)
    local selectedMonData = terynonMonData[monCategory][selectedMon]

    return xi.monstrosity.purchaseRequirementsMet(
        selectedMonData,
        function(species) return xi.monstrosity.getSpeciesLevel(player, species) end,
        function(variant) return xi.monstrosity.hasUnlockedVariant(player, variant) end
    )
end

-- Builds Terynon's client-visible offer bitmask for one MON category. Offer
-- data and player state lookups are injected so the decision stays pure.
xi.monstrosity.purchasePageMask = function(offers, speciesLevel, hasUnlockedVariant)
    local pageMask = 0

    if offers then
        for bitPos, offer in pairs(offers) do
            if xi.monstrosity.purchaseRequirementsMet(offer, speciesLevel, hasUnlockedVariant) then
                pageMask = utils.mask.setBit(pageMask, bitPos, true)
            end
        end
    end

    return pageMask
end

-- Plans a Terynon MON purchase after the host has decoded a valid offer and
-- read the player's infamy. A species unlock takes precedence over a variant,
-- matching the event-finish branch.
xi.monstrosity.monPurchasePlan = function(monData, infamy)
    if infamy < monData.infamyCost then
        return { deny = true }
    end

    local plan = { cost = monData.infamyCost }
    if monData.monSpecies then
        plan.unlockSpecies = monData.monSpecies
    elseif monData.monVariant then
        plan.unlockVariant = monData.monVariant
    end

    return plan
end

-- Plans a Terynon instinct purchase after the host has decoded the option.
-- Advanced instincts use their paired limit-break completion for a discount.
xi.monstrosity.instinctPurchasePlan = function(selectedInstinct, checkValue, limitBreakCompleted, infamy)
    if checkValue ~= 119 then
        return { invalid = true }
    end

    local price = selectedInstinct > xi.monstrosity.purchasableInstincts.GALKA_II and 10000 or 500
    if selectedInstinct > xi.monstrosity.purchasableInstincts.GALKA_II and limitBreakCompleted then
        price = price / 2
    end

    if infamy < price then
        return { deny = true }
    end

    return { cost = price, purchaseInstinct = selectedInstinct }
end

xi.monstrosity.protectPlan = function(mainLevel, enhancedReceived)
    local power, tier = 220, 5
    if mainLevel < 27 then power, tier = 20, 1
    elseif mainLevel < 47 then power, tier = 50, 2
    elseif mainLevel < 63 then power, tier = 90, 3
    elseif mainLevel < 76 then power, tier = 140, 4 end
    if enhancedReceived then power = power + 2 * tier end
    return { power = power, tier = tier, duration = 1800 }
end

xi.monstrosity.shellPlan = function(mainLevel, enhancedReceived)
    local power, tier = 2930, 5
    if mainLevel < 37 then power, tier = 1055, 1
    elseif mainLevel < 57 then power, tier = 1641, 2
    elseif mainLevel < 68 then power, tier = 2188, 3
    elseif mainLevel < 76 then power, tier = 2617, 4 end
    if enhancedReceived then power = power + 39 * tier end
    return { power = power, tier = tier, duration = 1800 }
end

xi.monstrosity.specialEffectPlan = function(selectedEffect, mainLevel, enhancedReceived, infamy)
    local plan
    if selectedEffect == 0 then
        plan = { cost = 3000, effect = 'dedication', power = 50, duration = 3600, subpower = 10000 }
    elseif selectedEffect == 1 then
        plan = { cost = 400, effect = 'dedication', power = 100, duration = 3600, subpower = 2000 }
    elseif selectedEffect == 2 then
        plan = { cost = 10, effect = 'regen', power = 1, duration = 3600, tick = 3 }
    elseif selectedEffect == 3 then
        plan = { cost = 10, effect = 'refresh', power = 1, duration = 3600, tick = 3 }
    elseif selectedEffect == 4 then
        plan = xi.monstrosity.protectPlan(mainLevel, enhancedReceived)
        plan.cost, plan.effect = 100, 'protect'
    elseif selectedEffect == 5 then
        plan = xi.monstrosity.shellPlan(mainLevel, enhancedReceived)
        plan.cost, plan.effect = 100, 'shell'
    elseif selectedEffect == 6 then
        return { cost = 0, effect = 'haste', power = 1000, duration = 600 }
    else
        return nil
    end

    if infamy < plan.cost then return { deny = true } end
    return plan
end

-- Plans the eight event-update arguments Terynon returns for the MON catalog
-- and instinct pages. Player state reads are injected by the event host.
xi.monstrosity.eventUpdatePlan = function(csid, option, monData, speciesLevel, hasUnlockedVariant, isInstinctPurchased, hasCompletedLimitBreak)
    if csid ~= 7 then
        return nil
    end

    local optionType = bit.band(option, 0xFF)
    if optionType == 0 then
        local monPage = bit.rshift(option, 16)
        return { xi.monstrosity.purchasePageMask(monData[monPage], speciesLevel, hasUnlockedVariant), 0, 0, 0, 0, 0, 0, 0 }
    elseif optionType == 1 then
        return { xi.monstrosity.purchasedInstinctsMask(isInstinctPurchased), xi.monstrosity.limitBreakMask(hasCompletedLimitBreak), 0, 0, 0, 0, 0, 0 }
    end

    return nil
end

-- Decodes Terynon's type-1 event-finish option and selects its MON purchase
-- work. Missing categories or offers are invalid rather than host errors.
xi.monstrosity.monEventFinishPlan = function(option, monData, infamy)
    if bit.band(option, 0xFF) ~= 1 then
        return nil
    end

    local selectedCategory = bit.band(bit.rshift(option, 8), 0xF) - 1
    local selectedMon      = bit.rshift(option, 16)
    local categoryData     = monData[selectedCategory]
    local selectedMonData  = categoryData and categoryData[selectedMon]
    if not selectedMonData then
        return { invalid = true }
    end

    local plan = xi.monstrosity.monPurchasePlan(selectedMonData, infamy)
    plan.selectedCategory = selectedCategory
    plan.selectedMon      = selectedMon
    return plan
end

-- Decodes Terynon's type-2 event-finish option and selects its instinct
-- purchase work. Advanced instincts use their paired limit-break completion.
xi.monstrosity.instinctEventFinishPlan = function(option, hasCompletedLimitBreak, infamy)
    if bit.band(option, 0xFF) ~= 2 then
        return nil
    end

    local selectedInstinct = bit.band(bit.rshift(option, 8), 0xFF)
    local checkValue       = bit.rshift(option, 16)
    local limitBreakCompleted = selectedInstinct > xi.monstrosity.purchasableInstincts.GALKA_II and
        hasCompletedLimitBreak(selectedInstinct - xi.monstrosity.purchasableInstincts.GALKA_II)

    return xi.monstrosity.instinctPurchasePlan(selectedInstinct, checkValue, limitBreakCompleted, infamy)
end

-- Decodes Terynon's type-3 event-finish option and selects its special-effect
-- purchase work. Effect application remains the event host's responsibility.
xi.monstrosity.specialEffectEventFinishPlan = function(option, mainLevel, enhancedReceived, infamy)
    if bit.band(option, 0xFF) ~= 3 then
        return nil
    end

    return xi.monstrosity.specialEffectPlan(bit.rshift(option, 8), mainLevel, enhancedReceived, infamy)
end

-- Plans Terynon's opening event. The source setting must be exactly one,
-- matching the NPC's ENABLE_MONSTROSITY gate.
xi.monstrosity.teyrnonTriggerPlan = function(monstrosityEnabled, infamy)
    if monstrosityEnabled ~= 1 then
        return nil
    end

    return { csid = 7, args = { infamy, 0, 0, 0, 0, 0, 0, 0 } }
end

-- Plans Maccus's opening event. The source setting must be exactly one,
-- matching the NPC's ENABLE_MONSTROSITY gate.
xi.monstrosity.maccusTriggerPlan = function(monstrosityEnabled)
    if monstrosityEnabled ~= 1 then
        return nil
    end

    return { csid = 9, args = { 285, 2, 2, 0, 0, 0, 0, 0 } }
end

-- Plans Aengus's opening event. The source setting must be exactly one,
-- matching the NPC's ENABLE_MONSTROSITY gate.
xi.monstrosity.aengusTriggerPlan = function(monstrosityEnabled, belligerency, infamy)
    if monstrosityEnabled ~= 1 then
        return nil
    end

    return { csid = 13, args = { belligerency and 1 or 0, infamy, 0, 0, 0, 0, 0, 0 } }
end

-- Plans Aengus's event-finish belligerency update. Only event 13 option 1
-- changes the flag; the player mutation remains in the event host.
xi.monstrosity.aengusEventFinishPlan = function(csid, option, belligerency)
    if csid ~= 13 or option ~= 1 then
        return nil
    end

    return { belligerency = not belligerency }
end

-- Plans the Odyssean Passage opening event. The source Monstrosity and PvP
-- bypass settings use exact-one gates.
xi.monstrosity.odysseanPassageTriggerPlan = function(monstrosityEnabled, monSize, belligerency, pvpZoneBypass)
    if monstrosityEnabled ~= 1 then
        return nil
    end

    local hasBelligerency = belligerency and 1 or 0
    -- Show the full menu, not the restricted one.
    if pvpZoneBypass == 1 then
        hasBelligerency = 0
    end

    -- The client derives available zones from visited-zone state. Its fifth
    -- parameter is observed as 0, 1, or 2, but this source path sends zero.
    return { csid = 5, args = { 0, monSize, hasBelligerency, 0, 0, 0, 0, 0 } }
end

-- Plans the Odyssean Passage's event-update response. The caller-provided
-- option selects its zone at bits 4 and above.
xi.monstrosity.odysseanPassageEventUpdatePlan = function(option)
    local zoneSelected = bit.rshift(option, 4)
    return { xi.monstrosity.belligerencyCaps[zoneSelected], 0, 0, 0, 1, 0, 0, 0 }
end

-- Plans the Odyssean Passage's event-finish work. The caller owns the random
-- destination choice and the player mutations.
xi.monstrosity.odysseanPassageEventFinishPlan = function(option, chooseTeleport)
    if bit.band(option, 0xF) ~= 1 then
        return nil
    end

    local zoneSelected = bit.rshift(option, 4)
    if zoneSelected == 0 then
        return { returnToEntrance = true }
    end

    local teleports = xi.monstrosity.teleports[zoneSelected]
    if teleports then
        return {
            position = teleports[chooseTeleport(#teleports)],
            zone     = zoneSelected,
        }
    end

    return {
        fallback = true,
        position = { 0, 0, 0, 0 },
        zone     = zoneSelected,
    }
end

-----------------------------------
-- Bound by C++ (DO NOT CHANGE SIGNATURE)
-----------------------------------

xi.monstrosity.updateLevelBasedInstincts = function(levels, instincts)
    -- Tap level-based unlocks

    -- Instincts by MON level
    -- NOTE: Since this is a bitfield, it's zero-indexed!
    for _, val in pairs(xi.monstrosity.species) do
        local speciesKey   = val
        local speciesLevel = levels[val]
        local byteOffset   = math.floor(speciesKey / 4)
        local unlockAmount = math.floor(speciesLevel / 30)
        local shiftAmount  = (speciesKey * 2) % 8

        -- Special case for writing Slime & Spriggan data at the end of the 64-byte array
        if byteOffset == 31 then
            byteOffset = 63
        end

        if byteOffset < 64 then
            instincts[byteOffset] = bit.bor(instincts[byteOffset] or 0, bit.lshift(unlockAmount, shiftAmount))
        else
            print('byteOffset out of range')
        end
    end
end

xi.monstrosity.onMonstrosityUpdate = function(player, data)
    xi.monstrosity.updateLevelBasedInstincts(data.levels, data.instincts)

    -- TODO: Handle level-based variants here
end

-- Plans the actions that return a player from Monstrosity. Status-effect
-- removal remains common to both paths; entity mutations remain host work.
xi.monstrosity.returnToEntrancePlan = function(teleportToFeretory, currentZone, data)
    if teleportToFeretory == 1 and currentZone ~= xi.zone.FERETORY then
        return {
            clearEffects = true,
            position     = { -358, -3.4, -440, 64 },
            zone         = xi.zone.FERETORY,
        }
    end

    return {
        clearEffects = true,
        restoreJobs  = true,
        mainJob      = data.entry_mjob,
        subJob       = data.entry_sjob,
        position     = { data.entry_x, data.entry_y, data.entry_z, data.entry_rot },
        zone         = data.entry_zone_id,
    }
end

xi.monstrosity.onMonstrosityReturnToEntrance = function(player)
    local data = player:getMonstrosityData()
    local plan = xi.monstrosity.returnToEntrancePlan(
        xi.settings.main.MONSTROSITY_TELEPORT_TO_FERETORY,
        player:getZoneID(),
        data
    )

    -- TODO: Sanity check

    if plan.clearEffects then
        for _, effect in pairs(player:getStatusEffects()) do
            player:delStatusEffectSilent(effect:getEffectType())
        end
    end

    if plan.restoreJobs then
        player:changeJob(plan.mainJob)
        player:changesJob(plan.subJob)
    end

    player:setPos(plan.position[1], plan.position[2], plan.position[3], plan.position[4], plan.zone)
end

-----------------------------------
-- Relinquish
-----------------------------------

-- Plans one timer tick of the Relinquish countdown. Counters remain clamped
-- on subsequent recursive ticks, preserving the source's repeated return path.
xi.monstrosity.relinquishTickPlan = function(relinquishCountdown)
    local step = utils.clamp(relinquishCountdown, 0, 4)

    if step == 4 then
        return { returnToEntrance = true, nextCountdown = step + 1 }
    end

    return { countdown = 4 - step, nextCountdown = step + 1 }
end

xi.monstrosity.relinquishFuncBody = function(player)
    -- TODO: Make this countdown interruptable
    player:timer(1000, function(playerArg)
        local plan = xi.monstrosity.relinquishTickPlan(playerArg:getLocalVar('RELINQUISH_COUNTDOWN'))

        if plan.countdown then
            playerArg:messageBasic(xi.msg.basic.FERETORY_COUNTDOWN, 0, plan.countdown)
        elseif plan.returnToEntrance then
            xi.monstrosity.onMonstrosityReturnToEntrance(playerArg)
        end

        playerArg:setLocalVar('RELINQUISH_COUNTDOWN', plan.nextCountdown)
        xi.monstrosity.relinquishFuncBody(playerArg)
    end)
end

xi.monstrosity.relinquishOnAbility = function(player, target, ability)
    xi.monstrosity.relinquishFuncBody(player)
end

-----------------------------------
-- Debug
-----------------------------------

xi.monstrosity.applyUnlockAllData = function(data)
    -- Set all levels to 99
    for _, val in pairs(xi.monstrosity.species) do
        data.levels[val] = 99
    end

    xi.monstrosity.updateLevelBasedInstincts(data.levels, data.instincts)

    -- Instincts (Purchasable)
    for _, val in pairs(xi.monstrosity.purchasableInstincts) do
        local byteOffset   = 20 + math.floor(val / 8)
        local shiftAmount  = val % 8

        if byteOffset >= 20 and byteOffset < 24 then
            data.instincts[byteOffset] = bit.bor(data.instincts[byteOffset] or 0, bit.lshift(0x01, shiftAmount))
        else
            print('byteOffset out of range')
        end
    end

    -- Variants
    -- Force unlock all
    for _, val in pairs(xi.monstrosity.variants) do
        local speciesKey   = val
        local byteOffset   = math.floor(speciesKey / 8)
        local shiftAmount  = speciesKey % 8

        if byteOffset < 32 then
            data.variants[byteOffset] = bit.bor(data.variants[byteOffset] or 0, bit.lshift(0x01, shiftAmount))
        else
            print('byteOffset out of range')
        end
    end
end

xi.monstrosity.unlockAll = function(player)
    -- Complete quest
    local logId = xi.questLog.OTHER_AREAS
    player:completeQuest(logId, xi.quest.id[xi.quest.area[logId]].MONSTROSITY)

    -- Add Monstrosity key item
    player:addKeyItem(xi.keyItem.RING_OF_SUPERNAL_DISJUNCTION)

    local data = player:getMonstrosityData()

    xi.monstrosity.applyUnlockAllData(data)

    -- Set data
    player:setMonstrosityData(data)
end

-----------------------------------
-- Odyssean Passage (Feretory Only)
-----------------------------------

xi.monstrosity.odysseanPassageOnTrade = function(player, npc, trade)
end

xi.monstrosity.odysseanPassageOnTrigger = function(player, npc)
    local plan = xi.monstrosity.odysseanPassageTriggerPlan(
        xi.settings.main.ENABLE_MONSTROSITY,
        player:getMonstrositySize(),
        player:getBelligerencyFlag(),
        xi.settings.main.MONSTROSITY_PVP_ZONE_BYPASS
    )

    if plan then
        player:startEvent(plan.csid, unpack(plan.args))
    end
end

xi.monstrosity.odysseanPassageOnEventUpdate = function(player, csid, option, npc)
    local plan = xi.monstrosity.odysseanPassageEventUpdatePlan(option)
    player:updateEvent(unpack(plan, 1, 8))
end

xi.monstrosity.odysseanPassageOnEventFinish = function(player, csid, option, npc)
    local plan = xi.monstrosity.odysseanPassageEventFinishPlan(option, function(count)
        return math.random(1, count)
    end)

    if not plan then
        return
    elseif plan.returnToEntrance then
        xi.monstrosity.onMonstrosityReturnToEntrance(player)
    else
        if plan.fallback then
            print('Monstrosity Teleport - No Valid Entries for Zone ' .. plan.zone .. '. Setting pos to (0, 0, 0)!')
        end

        player:setPos(plan.position[1],
            plan.position[2],
            plan.position[3],
            plan.position[4],
            plan.zone
        )
    end
end

-----------------------------------
-- Feretory
-----------------------------------

-- Plans Feretory entry actions. Position correction precedes the exact-one
-- enable gate; entity mutations remain host work.
xi.monstrosity.feretoryZoneInPlan = function(monstrosityEnabled, x, y, z, mainJob)
    local plan = {}

    if x == 0 and y == 0 and z == 0 then
        plan.position = { -358, -3.4, -440, 63 }
    end

    if monstrosityEnabled ~= 1 then
        return plan
    end

    if mainJob ~= xi.job.MON then
        plan.changeJob = true
        plan.mainJob   = xi.job.MON
    end

    plan.clearEffects = true
    return plan
end

xi.monstrosity.feretoryOnZoneIn = function(player, prevZone)
    local cs = -1
    local plan = xi.monstrosity.feretoryZoneInPlan(
        xi.settings.main.ENABLE_MONSTROSITY,
        player:getXPos(),
        player:getYPos(),
        player:getZPos(),
        player:getMainJob()
    )

    if plan.position then
        player:setPos(plan.position[1], plan.position[2], plan.position[3], plan.position[4])
    end

    if plan.changeJob then
        player:changeJob(plan.mainJob)
    end

    if plan.clearEffects then
        for _, effect in pairs(player:getStatusEffects()) do
            player:delStatusEffectSilent(effect:getEffectType())
        end
    end

    return cs
end

-- Plans the effect flags removed so status effects survive Feretory zone-out.
-- Entity mutation remains host work.
xi.monstrosity.feretoryZoneOutPlan = function(monstrosityEnabled)
    if monstrosityEnabled ~= 1 then
        return nil
    end

    return { effectFlags = { xi.effectFlag.ON_ZONE, xi.effectFlag.LOGOUT } }
end

xi.monstrosity.feretoryOnZoneOut = function(player)
    local plan = xi.monstrosity.feretoryZoneOutPlan(xi.settings.main.ENABLE_MONSTROSITY)
    if not plan then
        return
    end

    -- Mark all status effects so they'll survive zoning
    -- (there are some routines that will force them off anyway)
    for _, effect in pairs(player:getStatusEffects()) do
        for _, effectFlag in ipairs(plan.effectFlags) do
            effect:delEffectFlag(effectFlag)
        end
    end
end

xi.monstrosity.feretoryOnEventUpdate = function(player, csid, option, npc)
end

xi.monstrosity.feretoryOnEventFinish = function(player, csid, option, npc)
end

-----------------------------------
-- Aengus (Feretory NPC)
-----------------------------------

xi.monstrosity.aengusOnTrade = function(player, npc, trade)
end

xi.monstrosity.aengusOnTrigger = function(player, npc)
    local plan = xi.monstrosity.aengusTriggerPlan(
        xi.settings.main.ENABLE_MONSTROSITY,
        player:getBelligerencyFlag(),
        player:getCurrency('infamy')
    )

    if plan then
        player:startEvent(plan.csid, unpack(plan.args))
    end
end

xi.monstrosity.aengusOnEventUpdate = function(player, csid, option, npc)
end

xi.monstrosity.aengusOnEventFinish = function(player, csid, option, npc)
    local plan = xi.monstrosity.aengusEventFinishPlan(csid, option, player:getBelligerencyFlag())

    if plan then
        player:setBelligerencyFlag(plan.belligerency)
    end
end

-----------------------------------
-- Teyrnon (Feretory NPC)
-----------------------------------

xi.monstrosity.teyrnonOnTrade = function(player, npc, trade)
end

xi.monstrosity.teyrnonOnTrigger = function(player, npc)
    local plan = xi.monstrosity.teyrnonTriggerPlan(
        xi.settings.main.ENABLE_MONSTROSITY,
        player:getCurrency('infamy')
    )

    if plan then
        player:startEvent(plan.csid, unpack(plan.args))
    end
end

xi.monstrosity.teyrnonOnEventUpdate = function(player, csid, option, npc)
    local plan = xi.monstrosity.eventUpdatePlan(
        csid,
        option,
        terynonMonData,
        function(species) return xi.monstrosity.getSpeciesLevel(player, species) end,
        function(variant) return xi.monstrosity.hasUnlockedVariant(player, variant) end,
        function(instinct) return hasPurchasedInstinct(player, instinct) end,
        function(job) return hasCompletedLimitBreak(player, job) end
    )

    if plan then
        player:updateEvent(unpack(plan))
    end
end

xi.monstrosity.teyrnonOnEventFinish = function(player, csid, option, npc)
    local optionType = bit.band(option, 0xFF)

    if optionType == 1 then
        local plan = xi.monstrosity.monEventFinishPlan(option, terynonMonData, player:getCurrency('infamy'))
        if plan.invalid then
            print(string.format('Invalid Event Finish Option received by Terynon! (%s:%d)', player:getName(), option))
            return
        end

        if not plan.deny then
            player:delCurrency('infamy', plan.cost)

            if plan.unlockSpecies then
                xi.monstrosity.unlockSpecies(player, plan.unlockSpecies)
            elseif plan.unlockVariant then
                xi.monstrosity.unlockVariant(player, plan.unlockVariant)
            end

            player:messageSpecial(zones[xi.zone.FERETORY].text.MAY_POSSESS_BEASTS + 3 * plan.selectedCategory, 0, plan.selectedMon)
        else
            player:messageSpecial(zones[xi.zone.FERETORY].text.THY_BRAZEN_DISREGARD)
        end

    elseif optionType == 2 then
        -- Instincts: Costs are hardcoded, and adjusted based on having completed certain
        -- prerequisites.  This data is not tabled with Terynon, as it cannot be controlled.

        local selectedInstinct = bit.band(bit.rshift(option, 8), 0xFF)
        local plan = xi.monstrosity.instinctEventFinishPlan(
            option,
            function(job) return hasCompletedLimitBreak(player, job) end,
            player:getCurrency('infamy')
        )

        if plan.invalid then
            print(string.format('Invalid Event Finish Option received by Terynon! (%s:%d)', player:getName(), option))
            return
        end

        if not plan.deny then
            player:delCurrency('infamy', plan.cost)
            addPurchasedInstinct(player, plan.purchaseInstinct)

            -- NOTE: The offset below is the beginning parameter for purchased instincts used by this message, and
            -- lower values will result in an item being placed in the message.  Base offset for all instincts
            -- is 29696 (29696 + 3 -> Rabbit Instinct I)
            player:messageSpecial(zones[xi.zone.FERETORY].text.YOU_LEARNED_INSTINCT, 30464 + selectedInstinct)
        else
            player:messageSpecial(zones[xi.zone.FERETORY].text.THY_BRAZEN_DISREGARD)
        end

    elseif optionType == 3 then
        -- TODO: The casting effects and animations

        local plan = xi.monstrosity.specialEffectEventFinishPlan(
            option,
            player:getMainLvl(),
            player:getMod(xi.mod.ENHANCES_PROT_SHELL_RCVD) > 0,
            player:getCurrency('infamy')
        )
        if not plan then
            return
        elseif plan.deny then
            player:messageSpecial(zones[xi.zone.FERETORY].text.THY_BRAZEN_DISREGARD)
            return
        end

        if plan.cost > 0 then
            player:delCurrency('infamy', plan.cost)
        end

        if plan.effect == 'dedication' then
            player:delStatusEffectSilent(plan.power)
            xi.itemUtils.addItemExpEffect(player, xi.effect.DEDICATION, plan.power, plan.duration, plan.subpower)
        elseif plan.effect == 'regen' then
            player:delStatusEffectSilent(xi.effect.REGEN)
            player:addStatusEffect(xi.effect.REGEN, { power = plan.power, duration = plan.duration, origin = player, tick = plan.tick })
        elseif plan.effect == 'refresh' then
            player:delStatusEffectSilent(xi.effect.REFRESH)
            player:addStatusEffect(xi.effect.REFRESH, { power = plan.power, duration = plan.duration, origin = player, tick = plan.tick }) -- Does indeed get overwriten by regular refresh.
        elseif plan.effect == 'protect' then
            player:delStatusEffectSilent(xi.effect.PROTECT)
            player:addStatusEffect(xi.effect.PROTECT, { power = plan.power, duration = plan.duration, origin = player, tier = plan.tier })
        elseif plan.effect == 'shell' then
            player:delStatusEffectSilent(xi.effect.SHELL)
            player:addStatusEffect(xi.effect.SHELL, { power = plan.power, duration = plan.duration, origin = player, tier = plan.tier })
        elseif plan.effect == 'haste' then
            player:delStatusEffectSilent(xi.effect.HASTE)
            player:addStatusEffect(xi.effect.HASTE, { power = plan.power, duration = plan.duration, origin = player })
        end
    end
end

-----------------------------------
-- Maccus (Feretory NPC)
-----------------------------------

xi.monstrosity.maccusOnTrade = function(player, npc, trade)
end

xi.monstrosity.maccusOnTrigger = function(player, npc)
    local plan = xi.monstrosity.maccusTriggerPlan(xi.settings.main.ENABLE_MONSTROSITY)

    if plan then
        player:startEvent(plan.csid, unpack(plan.args))
    end
end

xi.monstrosity.maccusOnEventUpdate = function(player, csid, option, npc)
    -- print('update', csid, option)
end

xi.monstrosity.maccusOnEventFinish = function(player, csid, option, npc)
    -- print('finish', csid, option)
end
