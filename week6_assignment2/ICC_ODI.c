#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "Players_data.h"

#define TEAM_COUNT 10
#define ROLE_COUNT 3
#define NAME_LEN 51

enum { BATSMAN = 0, BOWLER = 1, ALL_ROUNDER = 2 };

typedef struct PlayerRecord {
    int playerId;
    char name[NAME_LEN];
    char teamName[NAME_LEN];
    int role;
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;

    struct PlayerRecord *nextTeam;
    struct PlayerRecord *nextRole;
} PlayerRecord;

typedef struct {
    int teamId;
    char name[NAME_LEN];
    int totalPlayers;
    float averageBattingStrikeRate;

    PlayerRecord *playerHead;
    PlayerRecord *playerTail;
    PlayerRecord *roleHead[ROLE_COUNT];
} Team;

typedef struct {
    PlayerRecord *items[TEAM_COUNT + 1];
    int size;
} MaxHeap;

static Team teamList[TEAM_COUNT];

static const char *teamNamesById[TEAM_COUNT] = {
    "India", "Australia", "England", "Pakistan", "New Zealand",
    "South Africa", "Bangladesh", "Sri Lanka", "Afghanistan", "West Indies"
};

static const char *roleNames[ROLE_COUNT] = {
    "Batsman", "Bowler", "All-rounder"
};

static void discardRestOfLine(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) { }
}

static int readInt(const char *prompt)
{
    char line[100];
    char *end;
    long value;

    for (;;) {
        printf("%s", prompt);
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\nInput ended. Exiting.\n");
            exit(EXIT_SUCCESS);
        }

        value = strtol(line, &end, 10);
        while (isspace((unsigned char)*end)) end++;

        if (end != line && *end == '\0' && value >= INT_MIN &&
            value <= INT_MAX) {
            return (int)value;
        }
        printf("Invalid integer. Please try again.\n");
    }
}

static float readFloat(const char *prompt)
{
    char line[100];
    char *end;
    float value;

    for (;;) {
        printf("%s", prompt);
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\nInput ended. Exiting.\n");
            exit(EXIT_SUCCESS);
        }

        value = strtof(line, &end);
        while (isspace((unsigned char)*end)) end++;

        if (end != line && *end == '\0') return value;
        printf("Invalid number. Please try again.\n");
    }
}

static void readString(const char *prompt, char *destination, size_t capacity)
{
    size_t length;

    for (;;) {
        printf("%s", prompt);
        if (!fgets(destination, (int)capacity, stdin)) {
            printf("\nInput ended. Exiting.\n");
            exit(EXIT_SUCCESS);
        }

        length = strlen(destination);
        if (length > 0 && destination[length - 1] == '\n') {
            destination[length - 1] = '\0';
        } else {
            discardRestOfLine();
        }

        if (destination[0] != '\0') return;
        printf("Input cannot be empty.\n");
    }
}

static int getRoleFromNumber(int choice)
{
    if (choice >= 1 && choice <= 3) return choice - 1;
    return -1;
}

static int roleFromHeader(const char *role)
{
    if (strcmp(role, "Batsman") == 0) return BATSMAN;
    if (strcmp(role, "Bowler") == 0) return BOWLER;
    if (strcmp(role, "All-rounder") == 0) return ALL_ROUNDER;
    return -1;
}

static float calculatePerformanceIndex(const PlayerRecord *player)
{
    float battingPart =
        (player->battingAverage * player->strikeRate) / 100.0f;

    if (player->role == BATSMAN) return battingPart;
    if (player->role == BOWLER)
        return (player->wickets * 2.0f) + (100.0f - player->economyRate);

    return battingPart + (player->wickets * 2.0f);
}

static int comparePlayerOrder(const PlayerRecord *a, const PlayerRecord *b)
{
    if (a->performanceIndex > b->performanceIndex) return 1;
    if (a->performanceIndex < b->performanceIndex) return -1;
    return 0;
}

static void insertIntoRoleList(Team *team, PlayerRecord *node)
{
    PlayerRecord **link = &team->roleHead[node->role];

    while (*link && comparePlayerOrder(*link, node) >= 0) {
        link = &(*link)->nextRole;
    }

    node->nextRole = *link;
    *link = node;
}

static void appendPlayerToTeam(Team *team, PlayerRecord *node)
{
    node->nextTeam = NULL;

    if (team->playerTail == NULL) {
        team->playerHead = node;
    } else {
        team->playerTail->nextTeam = node;
    }

    team->playerTail = node;
    team->totalPlayers++;
    insertIntoRoleList(team, node);
}

static void updateTeamAverage(Team *team)
{
    PlayerRecord *current;
    float sum = 0.0f;
    int count = 0;

    for (current = team->playerHead; current; current = current->nextTeam) {
        if (current->role == BATSMAN || current->role == ALL_ROUNDER) {
            sum += current->strikeRate;
            count++;
        }
    }

    team->averageBattingStrikeRate = (count > 0) ? sum / count : 0.0f;
}

static int compareTeamId(const void *a, const void *b)
{
    const Team *ta = (const Team *)a;
    const Team *tb = (const Team *)b;
    return (ta->teamId > tb->teamId) - (ta->teamId < tb->teamId);
}

static void initializeTeams(void)
{
    int i;

    for (i = 0; i < TEAM_COUNT; i++) {
        teamList[i].teamId = i + 1;
        snprintf(teamList[i].name, sizeof(teamList[i].name), "%s",
                 teamNamesById[i]);
        teamList[i].totalPlayers = 0;
        teamList[i].averageBattingStrikeRate = 0.0f;
        teamList[i].playerHead = NULL;
        teamList[i].playerTail = NULL;

        for (int r = 0; r < ROLE_COUNT; r++) {
            teamList[i].roleHead[r] = NULL;
        }
    }

    qsort(teamList, TEAM_COUNT, sizeof(Team), compareTeamId);
}

static Team *searchTeamById(int teamId)
{
    int low = 0, high = TEAM_COUNT - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        if (teamList[mid].teamId == teamId) return &teamList[mid];
        if (teamList[mid].teamId < teamId) low = mid + 1;
        else high = mid - 1;
    }

    return NULL;
}

static int playerIdExists(int playerId)
{
    int i;
    PlayerRecord *current;

    for (i = 0; i < TEAM_COUNT; i++) {
        for (current = teamList[i].playerHead; current; current = current->nextTeam) {
            if (current->playerId == playerId) return 1;
        }
    }
    return 0;
}

static void initializePlayers(void)
{
    size_t i;
    int loaded = 0;

    for (i = 0; i < sizeof(players) / sizeof(players[0]); i++) {
        int role = roleFromHeader(players[i].role);
        Team *team = NULL;
        PlayerRecord *node;
        int j;

        if (role < 0) continue;

        for (j = 0; j < TEAM_COUNT; j++) {
            if (strcmp(teamList[j].name, players[i].team) == 0) {
                team = &teamList[j];
                break;
            }
        }
        if (!team) continue;

        node = (PlayerRecord *)calloc(1, sizeof(PlayerRecord));
        if (!node) {
            fprintf(stderr, "Memory allocation failed during initialization.\n");
            exit(EXIT_FAILURE);
        }

        node->playerId = players[i].id;
        snprintf(node->name, sizeof(node->name), "%s", players[i].name);
        snprintf(node->teamName, sizeof(node->teamName), "%s", players[i].team);
        node->role = role;
        node->totalRuns = players[i].totalRuns;
        node->battingAverage = players[i].battingAverage;
        node->strikeRate = players[i].strikeRate;
        node->wickets = players[i].wickets;
        node->economyRate = players[i].economyRate;
        node->performanceIndex = calculatePerformanceIndex(node);

        appendPlayerToTeam(team, node);
        loaded++;
    }

    for (i = 0; i < TEAM_COUNT; i++) updateTeamAverage(&teamList[i]);

    printf("Initialized %d players across %d teams.\n", loaded, TEAM_COUNT);
}

static void printPlayerHeader(void)
{
    printf("---------------------------------------------------------------------------------------------------------------\n");
    printf("%-6s %-25s %-13s %-10s %-8s %-8s %-8s %-8s %-10s\n",
           "ID", "Name", "Role", "Runs", "Avg", "SR", "Wickets", "Econ", "Perf.Index");
    printf("---------------------------------------------------------------------------------------------------------------\n");
}

static void printPlayer(const PlayerRecord *p)
{
    printf("%-6d %-25.25s %-13s %-10d %-8.2f %-8.2f %-8d %-8.2f %-10.2f\n",
           p->playerId, p->name, roleNames[p->role], p->totalRuns,
           p->battingAverage, p->strikeRate, p->wickets,
           p->economyRate, p->performanceIndex);
}

static void addPlayer(void)
{
    int teamId, roleChoice, role;
    Team *team;
    PlayerRecord *node;

    teamId = readInt("Enter Team ID: ");
    team = searchTeamById(teamId);

    if (!team) {
        printf("Team ID not found.\n");
        return;
    }

    node = (PlayerRecord *)calloc(1, sizeof(PlayerRecord));
    if (!node) {
        printf("Memory allocation failed.\n");
        return;
    }

    node->playerId = readInt("Player ID: ");
    if (node->playerId < 1 || node->playerId > 1000 ||
        playerIdExists(node->playerId)) {
        printf("Invalid or duplicate Player ID. Player was not added.\n");
        free(node);
        return;
    }

    readString("Name: ", node->name, sizeof(node->name));

    printf("Role (1-Batsman, 2-Bowler, 3-All-rounder):\n");
    roleChoice = readInt("Enter role: ");
    role = getRoleFromNumber(roleChoice);
    if (role < 0) {
        printf("Invalid role.\n");
        free(node);
        return;
    }
    node->role = role;

    node->totalRuns = readInt("Total Runs: ");
    node->battingAverage = readFloat("Batting Average: ");
    node->strikeRate = readFloat("Strike Rate: ");
    node->wickets = readInt("Wickets: ");
    node->economyRate = readFloat("Economy Rate: ");

    if (node->totalRuns < 0 || node->battingAverage < 0 ||
        node->strikeRate < 0 || node->wickets < 0 ||
        node->economyRate < 0) {
        printf("Statistics cannot be negative. Player was not added.\n");
        free(node);
        return;
    }

    snprintf(node->teamName, sizeof(node->teamName), "%s", team->name);
    node->performanceIndex = calculatePerformanceIndex(node);

    appendPlayerToTeam(team, node);
    updateTeamAverage(team);

    printf("Player added successfully to Team %s.\n", team->name);
    printf("Calculated Performance Index: %.2f\n", node->performanceIndex);
}

static void displayTeamPlayers(void)
{
    int teamId;
    Team *team;
    PlayerRecord *current;

    teamId = readInt("Enter Team ID: ");
    team = searchTeamById(teamId);

    if (!team) {
        printf("Team ID not found.\n");
        return;
    }

    printf("\nPlayers of Team %s:\n", team->name);
    printPlayerHeader();

    for (current = team->playerHead; current; current = current->nextTeam) {
        printPlayer(current);
    }

    printf("---------------------------------------------------------------------------------------------------------------\n");
    printf("Total Players: %d\n", team->totalPlayers);
    printf("Average Batting Strike Rate: %.2f\n",
           team->averageBattingStrikeRate);
}

static void displayTeamsByAverage(void)
{
    Team *sorted[TEAM_COUNT];
    int pass, current;

    for (pass = 0; pass < TEAM_COUNT; pass++) sorted[pass] = &teamList[pass];

    for (pass = 0; pass < TEAM_COUNT - 1; pass++) {
        for (current = 0; current < TEAM_COUNT - pass - 1; current++) {
            if (sorted[current]->averageBattingStrikeRate <
                sorted[current + 1]->averageBattingStrikeRate) {
                Team *temp = sorted[current];
                sorted[current] = sorted[current + 1];
                sorted[current + 1] = temp;
            }
        }
    }

    printf("\nTeams Sorted by Average Batting Strike Rate\n");
    printf("---------------------------------------------------------------\n");
    printf("%-6s %-20s %-15s %-15s\n","ID", "Team Name", "Average SR", "Total Players");
    printf("---------------------------------------------------------------\n");

    for (pass = 0; pass < TEAM_COUNT; pass++) {
        printf("%-6d %-20s %-15.2f %-15d\n", sorted[pass]->teamId, sorted[pass]->name,
               sorted[pass]->averageBattingStrikeRate,
               sorted[pass]->totalPlayers);
    }
}

static int readRoleChoice(void)
{
    int choice, role;

    printf("1. Batsman\n2. Bowler\n3. All-rounder\n");
    choice = readInt("Enter role: ");
    role = getRoleFromNumber(choice);

    if (role < 0) printf("Invalid role.\n");
    return role;
}

static void displayTopKPlayers(void)
{
    int teamId, role, k, displayed = 0;
    Team *team;
    PlayerRecord *current;

    teamId = readInt("Enter Team ID: ");
    team = searchTeamById(teamId);

    if (!team) {
        printf("Team ID not found.\n");
        return;
    }

    role = readRoleChoice();
    if (role < 0) return;

    k = readInt("Enter number of top players (K): ");
    if (k <= 0) {
        printf("K must be greater than zero.\n");
        return;
    }

    printf("\nTop %d %s(s) of Team %s:\n", k, roleNames[role], team->name);
    printPlayerHeader();

    current = team->roleHead[role];
    while (current && displayed < k) {
        printPlayer(current);
        current = current->nextRole;
        displayed++;
    }

    if (displayed == 0) printf("No players found for this role.\n");
    printf("Displayed %d player(s).\n", displayed);
}

static int higherPriority(const PlayerRecord *a, const PlayerRecord *b)
{
    return a->performanceIndex > b->performanceIndex;
}

static void heapPush(MaxHeap *heap, PlayerRecord *node)
{
    int index;

    if (!node || heap->size >= TEAM_COUNT) return;

    index = ++heap->size;
    while (index > 1 &&
           higherPriority(node, heap->items[index / 2])) {
        heap->items[index] = heap->items[index / 2];
        index /= 2;
    }
    heap->items[index] = node;
}

static PlayerRecord *heapPop(MaxHeap *heap)
{
    PlayerRecord *top, *last;
    int parent, child;

    if (heap->size == 0) return NULL;

    top = heap->items[1];
    last = heap->items[heap->size--];

    if (heap->size == 0) return top;

    parent = 1;
    while (parent * 2 <= heap->size) {
        child = parent * 2;

        if (child + 1 <= heap->size &&
            higherPriority(heap->items[child + 1], heap->items[child])) {
            child++;
        }

        if (higherPriority(last, heap->items[child])) break;

        heap->items[parent] = heap->items[child];
        parent = child;
    }

    heap->items[parent] = last;
    return top;
}

static void displayAllPlayersByRole(void)
{
    int role, currentTeam, total = 0;
    MaxHeap heap = { .size = 0 };
    PlayerRecord *current;

    role = readRoleChoice();
    if (role < 0) return;

    for (currentTeam = 0; currentTeam < TEAM_COUNT; currentTeam++) {
        if (teamList[currentTeam].roleHead[role]) {
            heapPush(&heap, teamList[currentTeam].roleHead[role]);
        }
    }

    printf("\nAll %s players across all teams (descending Performance Index):\n",
           roleNames[role]);
    printf("-------------------------------------------------------------------------------------------------------------------------\n");
    printf("%-6s %-25s %-18s %-13s %-10s %-8s %-8s %-8s %-8s %-10s\n",
           "ID", "Name", "Team", "Role", "Runs", "Avg", "SR",
           "Wickets", "Econ", "Perf.Index");
    printf("-------------------------------------------------------------------------------------------------------------------------\n");

    while (heap.size > 0) {
        current = heapPop(&heap);

        printf("%-6d %-25.25s %-18.18s %-13s %-10d %-8.2f %-8.2f %-8d %-8.2f %-10.2f\n",
               current->playerId, current->name, current->teamName,
               roleNames[current->role], current->totalRuns,
               current->battingAverage, current->strikeRate,
               current->wickets, current->economyRate,
               current->performanceIndex);
        total++;

        if (current->nextRole) heapPush(&heap, current->nextRole);
    }

    printf("-------------------------------------------------------------------------------------------------------------------------\n");
    printf("Total %s players: %d\n", roleNames[role], total);
}

static void freeAllPlayers(void)
{
    int currentTeam;
    for (currentTeam = 0; currentTeam < TEAM_COUNT; currentTeam++) {
        PlayerRecord *current = teamList[currentTeam].playerHead;

        while (current) {
            PlayerRecord *next = current->nextTeam;
            free(current);
            current = next;
        }

        teamList[currentTeam].playerHead = NULL;
        teamList[currentTeam].playerTail = NULL;
    }
}

int main(void)
{
    int choice = 0;

    initializeTeams();
    initializePlayers();

    do {
        printf("\n============================================================\n");
        printf("             ICC ODI Player Performance Analyzer\n");
        printf("============================================================\n");
        printf("1. Add Player to Team\n");
        printf("2. Display Players of a Specific Team\n");
        printf("3. Display Teams by Average Batting Strike Rate\n");
        printf("4. Display Top K Players of a Specific Team by Role\n");
        printf("5. Display All Players of a Specific Role Across All Teams\n");
        printf("6. Exit\n");
        printf("============================================================\n");

        choice = readInt("Enter your choice: ");

        switch (choice) {
            case 1: addPlayer(); break;
            case 2: displayTeamPlayers(); break;
            case 3: displayTeamsByAverage(); break;
            case 4: displayTopKPlayers(); break;
            case 5: displayAllPlayersByRole(); break;
            case 6:
                freeAllPlayers();
                printf("Program exited successfully.\n");
                break;
            default:
                printf("Invalid choice. Please select 1 to 6.\n");
        }
    } while (choice != 6);

    return 0;
}
