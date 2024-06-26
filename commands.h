#pragma once

#if __cplusplus
extern "C" {
#endif

char commands_list[] = "All supported commands:\n"
        "1.  get\n"
        "2.  set\n"
        "3.  del\n"
        "4.  keys\n"
        "5.  zadd\n"
        "6.  zrem\n"
        "7.  zscore\n"
        "8.  zquery\n"
        "9.  exists\n"
        "10. expire\n"
        "11. ttl\n"
        "12. command\n"
        "13. shutdown\n";

char commands_description[] = "All supported commands:\n\n"
        "1.  get: This command retrieves the value associated with\n"
        "     the provided key from the database.\n"
        "     If the key is found, it returns the associated value.\n"
        "     Otherwise, it returns nil.\n\n"
        "2.  set: This command sets the value associated with the\n"
        "     provided key in the database.\n"
        "     If the key already exists, it updates the associated value.\n"
        "     Otherwise, it creates a new key-value pair.\n\n"
        "3.  del: This command removes the key-value pair associated with\n"
        "     the provided key from the database.\n"
        "     If the key is found and successfully removed, it returns 1.\n"
        "     Otherwise, it returns 0.\n\n"
        "4.  keys: This command sends back an array of all keys in the database.\n\n"
        "5.  zadd: This command adds a member to a sorted set,\n"
        "     or updates its score if it already exists.\n"
        "     The command takes three arguments: the name of the sorted set,\n"
        "     the score of the member, and the member itself.\n\n"
        "6.  zrem: This command removes a member from a sorted set.\n"
        "     The command takes two arguments:\n"
        "     the name of the sorted set and the member to remove.\n\n"
        "7.  zscore: This command retrieves the score of a member in a sorted set.\n"
        "     The command takes two arguments:\n"
        "     the name of the sorted set and the member to retrieve the score for.\n\n"
        "8.  zquery: This command retrieves members of a sorted set by score.\n"
        "     The command takes five arguments:\n"
        "     the name of the sorted set, the score to query,\n"
        "     the member to start from, the offset, and the limit.\n\n"
        "9.  exists: This command checks if a key exists in the database.\n"
        "     If the key exists, it returns 1. Otherwise, it returns 0.\n\n"
        "10. expire: This command sets a key's time to live in milliseconds.\n"
        "     The command takes two arguments:\n"
        "     the key and the time to live in milliseconds.\n\n"
        "11. ttl: This command gets the time to live for a key.\n"
        "     The command takes one argument: the key.\n\n"
        "12. command: This command retrieves the description of all commands.\n"
        "     If the argument is 'list', all supported commands are listed instead.\n\n"
        "13. shutdown: This command shuts down the server.\n\n"
        "All commands are processed case-insensitively.\n";

#if __cplusplus
}
#endif
