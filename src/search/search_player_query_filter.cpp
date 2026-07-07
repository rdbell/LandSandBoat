#include "search_player_query_filter.h"

#include "common/logging.h"
#include "common/types/maybe.h"
#include "search.h"

auto BuildSearchPlayerCountQuery(const search_req& request) -> SearchPlayerCountQuery
{
    if (request.jobid > 0 && request.jobid < 21)
    {
        return SearchPlayerCountQuery{
            "SELECT COUNT(*) FROM accounts_sessions LEFT JOIN char_stats USING (charid) WHERE mjob = ?",
            true,
            request.jobid,
        };
    }

    return SearchPlayerCountQuery{
        "SELECT COUNT(*) FROM accounts_sessions",
        false,
        0,
    };
}

auto BuildSearchPlayerListQuery(const std::string& filterQuery) -> std::string
{
    std::string query =
        "SELECT charid, partyid, charname, pos_zone, pos_prevzone, nation, rank_sandoria, rank_bastok, unity_leader, "
        "rank_windurst, race, mjob, sjob, mlvl, slvl, languages, settings, seacom_type, disconnecting, gmHiddenEnabled, muted, "
        "linkshellid1, linkshellid2 "
        "FROM accounts_sessions "
        "LEFT JOIN accounts_parties USING (charid) "
        "LEFT JOIN chars USING (charid) "
        "LEFT JOIN char_look USING (charid) "
        "LEFT JOIN char_stats USING (charid) "
        "LEFT JOIN char_profile USING(charid) "
        "LEFT JOIN char_flags USING(charid) "
        "WHERE charname IS NOT NULL ";

    query.append(filterQuery);
    query.append(" ORDER BY charname ASC");
    return query;
}

auto BuildSearchPlayerQueryFilter(const search_req& request) -> std::string
{
    std::string filterQuery;

    if (request.jobid > 0 && request.jobid < 21)
    {
        filterQuery.append(" AND ");
        filterQuery.append(" mjob = ");
        filterQuery.append(std::to_string(static_cast<unsigned long long>(request.jobid)));
    }

    if (request.zoneid[0] > 0)
    {
        std::string zoneList;
        int         i = 1;
        zoneList.append(std::to_string(static_cast<unsigned long long>(request.zoneid[0])));
        while (i < 10 && request.zoneid[i] != 0)
        {
            zoneList.append(", ");
            zoneList.append(std::to_string(static_cast<unsigned long long>(request.zoneid[i])));
            i++;
        }
        filterQuery.append(" AND ");
        filterQuery.append("(pos_zone IN (");
        filterQuery.append(zoneList);
        filterQuery.append(") OR (pos_zone = 0 AND pos_prevzone IN (");
        filterQuery.append(zoneList);
        filterQuery.append("))) ");
    }

    if (request.commentType != 0)
    {
        filterQuery.append(" AND (seacom_type & 0xF0) = ");
        filterQuery.append(std::to_string(static_cast<unsigned long long>(request.commentType)));
    }

    return filterQuery;
}
