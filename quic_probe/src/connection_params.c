//
// Created by sergei on 07.05.18.
//

#include "connection_params.h"

#include <string.h>
#include <stdlib.h>

#include "engine_structs.h"

connection_parameters* find_connection(struct simple_list * connection_list, const char* hostname)
{
    struct enumerator* enumerator_ref = get_enumerator(connection_list);
    connection_parameters* connection = (connection_parameters*)get_next_element(enumerator_ref);
    while(connection != NULL)
    {
        if (strcmp(hostname, connection->hostname->text) == 0)
            break;
        connection = (connection_parameters*)get_next_element(enumerator_ref);
    }
    free(enumerator_ref);
    return connection;
}