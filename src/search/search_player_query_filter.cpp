#include "search_player_query_filter.h"

#include "common/logging.h"
#include "common/types/maybe.h"
#include "search.h"

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
