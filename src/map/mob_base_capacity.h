#pragma once

#include <cmath>
#include <cstdint>

// Pure mobutils::GetBaseDefEva + CheckSubJobZone zone membership.
// Parity: internal/mobutils GetBaseDefEva / IsSubJobZone (slice 1598).

namespace mobbasehelpers
{

// GetBaseDefEva (level + rank only). rank 1–5 (A–E); invalid → 0.
inline auto GetBaseDefEva(const std::uint8_t rank, const std::uint8_t lvl) -> std::uint16_t
{
    const float lv = static_cast<float>(lvl);
    if (lvl > 50)
    {
        switch (rank)
        {
            case 1:
                return static_cast<std::uint16_t>(std::floor(153 + (lv - 50) * 5.0f));
            case 2:
                return static_cast<std::uint16_t>(std::floor(147 + (lv - 50) * 4.9f));
            case 3:
                return static_cast<std::uint16_t>(std::floor(142 + (lv - 50) * 4.8f));
            case 4:
                return static_cast<std::uint16_t>(std::floor(136 + (lv - 50) * 4.7f));
            case 5:
                return static_cast<std::uint16_t>(std::floor(126 + (lv - 50) * 4.5f));
            default:
                return 0;
        }
    }
    switch (rank)
    {
        case 1:
            return static_cast<std::uint16_t>(std::floor(6 + (lv - 1) * 3.0f));
        case 2:
            return static_cast<std::uint16_t>(std::floor(5 + (lv - 1) * 2.9f));
        case 3:
            return static_cast<std::uint16_t>(std::floor(5 + (lv - 1) * 2.8f));
        case 4:
            return static_cast<std::uint16_t>(std::floor(4 + (lv - 1) * 2.7f));
        case 5:
            return static_cast<std::uint16_t>(std::floor(4 + (lv - 1) * 2.5f));
        default:
            return 0;
    }
}

// Original/RoZ zones where mobs keep subjob stats (CheckSubJobZone).
constexpr auto IsSubJobZone(const std::uint16_t zoneId) -> bool
{
    if (zoneId == 0)
    {
        return false;
    }
    switch (zoneId)
    {
        case 100: // WEST_RONFAURE
        case 101: // EAST_RONFAURE
        case 102: // LA_THEINE_PLATEAU
        case 103: // VALKURM_DUNES
        case 104: // JUGNER_FOREST
        case 105: // BATALLIA_DOWNS
        case 106: // NORTH_GUSTABERG
        case 107: // SOUTH_GUSTABERG
        case 108: // KONSCHTAT_HIGHLANDS
        case 109: // PASHHOW_MARSHLANDS
        case 110: // ROLANBERRY_FIELDS
        case 111: // BEAUCEDINE_GLACIER
        case 112: // XARCABARD
        case 113: // CAPE_TERIGGAN
        case 114: // EASTERN_ALTEPA_DESERT
        case 115: // WEST_SARUTABARUTA
        case 116: // EAST_SARUTABARUTA
        case 117: // TAHRONGI_CANYON
        case 118: // BUBURIMU_PENINSULA
        case 119: // MERIPHATAUD_MOUNTAINS
        case 120: // SAUROMUGUE_CHAMPAIGN
        case 121: // THE_SANCTUARY_OF_ZITAH
        case 122: // ROMAEVE
        case 123: // YUHTUNGA_JUNGLE
        case 124: // YHOATOR_JUNGLE
        case 125: // WESTERN_ALTEPA_DESERT
        case 126: // QUFIM_ISLAND
        case 127: // BEHEMOTHS_DOMINION
        case 128: // VALLEY_OF_SORROWS
        case 139: // HORLAIS_PEAK
        case 140: // GHELSBA_OUTPOST
        case 141: // FORT_GHELSBA
        case 142: // YUGHOTT_GROTTO
        case 143: // PALBOROUGH_MINES
        case 144: // WAUGHROON_SHRINE
        case 145: // GIDDEUS
        case 146: // BALGAS_DAIS
        case 147: // BEADEAUX
        case 148: // QULUN_DOME
        case 149: // DAVOI
        case 150: // MONASTIC_CAVERN
        case 151: // CASTLE_OZTROJA
        case 152: // ALTAR_ROOM
        case 153: // THE_BOYAHDA_TREE
        case 154: // DRAGONS_AERY
        case 157: // MIDDLE_DELKFUTTS_TOWER
        case 158: // UPPER_DELKFUTTS_TOWER
        case 159: // TEMPLE_OF_UGGALEPIH
        case 160: // DEN_OF_RANCOR
        case 161: // CASTLE_ZVAHL_BAILEYS
        case 162: // CASTLE_ZVAHL_KEEP
        case 163: // SACRIFICIAL_CHAMBER
        case 165: // THRONE_ROOM
        case 166: // RANGUEMONT_PASS
        case 167: // BOSTAUNIEUX_OUBLIETTE
        case 168: // CHAMBER_OF_ORACLES
        case 169: // TORAIMARAI_CANAL
        case 170: // FULL_MOON_FOUNTAIN
        case 172: // ZERUHN_MINES
        case 173: // KORROLOKA_TUNNEL
        case 174: // KUFTAL_TUNNEL
        case 176: // SEA_SERPENT_GROTTO
        case 177: // VELUGANNON_PALACE
        case 178: // THE_SHRINE_OF_RUAVITAU
        case 179: // STELLAR_FULCRUM
        case 180: // LALOFF_AMPHITHEATER
        case 181: // THE_CELESTIAL_NEXUS
        case 184: // LOWER_DELKFUTTS_TOWER
        case 190: // KING_RANPERRES_TOMB
        case 191: // DANGRUF_WADI
        case 192: // INNER_HORUTOTO_RUINS
        case 193: // ORDELLES_CAVES
        case 194: // OUTER_HORUTOTO_RUINS
        case 195: // THE_ELDIEME_NECROPOLIS
        case 196: // GUSGEN_MINES
        case 197: // CRAWLERS_NEST
        case 198: // MAZE_OF_SHAKHRAMI
        case 200: // GARLAIGE_CITADEL
        case 201: // CLOISTER_OF_GALES
        case 202: // CLOISTER_OF_STORMS
        case 203: // CLOISTER_OF_FROST
        case 204: // FEIYIN
        case 205: // IFRITS_CAULDRON
        case 206: // QUBIA_ARENA
        case 207: // CLOISTER_OF_FLAMES
        case 208: // QUICKSAND_CAVES
        case 209: // CLOISTER_OF_TREMORS
        case 211: // CLOISTER_OF_TIDES
        case 212: // GUSTAV_TUNNEL
        case 213: // LABYRINTH_OF_ONZOZO
        case 220: // SHIP_BOUND_FOR_SELBINA
        case 221: // SHIP_BOUND_FOR_MHAURA
        case 227: // SHIP_BOUND_FOR_SELBINA_PIRATES
        case 228: // SHIP_BOUND_FOR_MHAURA_PIRATES
            return true;
        default:
            return false;
    }
}

} // namespace mobbasehelpers
