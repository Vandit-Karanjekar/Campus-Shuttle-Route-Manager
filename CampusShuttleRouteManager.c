#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct StopNode {
    int stopID;
    char name[64];
    int routeNumber;
    float distance;
    char status[32];
    char timing[32];
    struct StopNode *next;
    struct StopNode *prev;
} StopNode;

typedef struct {
    StopNode *head;
    int totalStops;
} ShuttleRoute;

static void read_line(char *buf, size_t n) {
    if (fgets(buf, (int)n, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    buf[strcspn(buf, "\n")] = '\0';
}

static int read_int(void) {
    char buf[64];
    read_line(buf, sizeof buf);
    return atoi(buf);
}

static float read_float(void) {
    char buf[64];
    read_line(buf, sizeof buf);
    return (float)atof(buf);
}

static StopNode *find(ShuttleRoute *r, int id) {
    if (r->head == NULL) {
        return NULL;
    }
    StopNode *curr = r->head;
    do {
        if (curr->stopID == id) {
            return curr;
        }
        curr = curr->next;
    } while (curr != r->head);
    return NULL;
}

static StopNode *make_stop(int id, const char *name, int route, float dist,
                           const char *status, const char *timing) {
    StopNode *n = (StopNode *)malloc(sizeof(StopNode));
    if (n == NULL) {
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }
    n->stopID = id;
    n->routeNumber = route;
    n->distance = dist;
    strncpy(n->name, name, sizeof n->name - 1);
    n->name[sizeof n->name - 1] = '\0';
    strncpy(n->status, status, sizeof n->status - 1);
    n->status[sizeof n->status - 1] = '\0';
    strncpy(n->timing, timing, sizeof n->timing - 1);
    n->timing[sizeof n->timing - 1] = '\0';
    n->next = n->prev = NULL;
    return n;
}

static void display_brief(const StopNode *s) {
    printf("[ID:%d] %s | Route:%d | Dist:%.1fkm | %s | %s\n",
           s->stopID, s->name, s->routeNumber, s->distance, s->status, s->timing);
}

static void display_stop(const StopNode *s) {
    printf("Stop ID      : %d\n", s->stopID);
    printf("Name         : %s\n", s->name);
    printf("Route Number : %d\n", s->routeNumber);
    printf("Distance     : %.2f km\n", s->distance);
    printf("Status       : %s\n", s->status);
    printf("Timing       : %s\n", s->timing);
    printf("-----------------------------------------------\n");
}

void insert_stop(ShuttleRoute *r, int id, const char *name, int route, float dist,
                 const char *status, const char *timing) {
    if (find(r, id) != NULL) {
        printf("Stop ID %d already exists.\n", id);
        return;
    }
    StopNode *node = make_stop(id, name, route, dist, status, timing);
    if (r->head == NULL) {
        r->head = node;
        node->next = node;
        node->prev = node;
    } else {
        StopNode *tail = r->head->prev;
        tail->next = node;
        node->prev = tail;
        node->next = r->head;
        r->head->prev = node;
    }
    r->totalStops++;
    printf("Stop '%s' (ID: %d) inserted.\n", name, id);
}

void insert_after(ShuttleRoute *r, int afterID, int id, const char *name, int route,
                  float dist, const char *status, const char *timing) {
    if (r->head == NULL) {
        insert_stop(r, id, name, route, dist, status, timing);
        return;
    }
    if (find(r, id) != NULL) {
        printf("Stop ID %d already exists.\n", id);
        return;
    }
    StopNode *curr = find(r, afterID);
    if (curr == NULL) {
        printf("Stop ID %d not found.\n", afterID);
        return;
    }
    StopNode *node = make_stop(id, name, route, dist, status, timing);
    node->next = curr->next;
    node->prev = curr;
    curr->next->prev = node;
    curr->next = node;
    r->totalStops++;
    printf("Stop '%s' inserted after ID %d.\n", name, afterID);
}

void delete_stop(ShuttleRoute *r, int id) {
    if (r->head == NULL) {
        printf("Route is empty.\n");
        return;
    }
    StopNode *curr = find(r, id);
    if (curr == NULL) {
        printf("Stop ID %d not found.\n", id);
        return;
    }
    if (r->totalStops == 1) {
        free(curr);
        r->head = NULL;
    } else {
        curr->prev->next = curr->next;
        curr->next->prev = curr->prev;
        if (curr == r->head) {
            r->head = curr->next;
        }
        free(curr);
    }
    r->totalStops--;
    printf("Stop ID %d deleted.\n", id);
}

void search_and_display(ShuttleRoute *r, int id) {
    StopNode *found = find(r, id);
    if (found == NULL) {
        printf("Stop ID %d not found.\n", id);
        return;
    }
    printf("\n========== Stop Found ==========\n");
    display_stop(found);
}

void update_stop(ShuttleRoute *r, int id, const char *name, float dist,
                 const char *status, const char *timing) {
    StopNode *found = find(r, id);
    if (found == NULL) {
        printf("Stop ID %d not found.\n", id);
        return;
    }
    strncpy(found->name, name, sizeof found->name - 1);
    found->name[sizeof found->name - 1] = '\0';
    found->distance = dist;
    strncpy(found->status, status, sizeof found->status - 1);
    found->status[sizeof found->status - 1] = '\0';
    strncpy(found->timing, timing, sizeof found->timing - 1);
    found->timing[sizeof found->timing - 1] = '\0';
    printf("Stop ID %d updated.\n", id);
}

void forward_traversal(ShuttleRoute *r) {
    if (r->head == NULL) {
        printf("Route is empty.\n");
        return;
    }
    printf("\n========== Forward Traversal (Current Route) ==========\n");
    StopNode *curr = r->head;
    int n = 1;
    do {
        printf("%d. ", n++);
        display_brief(curr);
        curr = curr->next;
    } while (curr != r->head);
    printf("Total stops: %d\n", r->totalStops);
}

void backward_traversal(ShuttleRoute *r) {
    if (r->head == NULL) {
        printf("Route is empty.\n");
        return;
    }
    printf("\n========== Backward Traversal ==========\n");
    StopNode *curr = r->head->prev;
    int n = 1;
    do {
        printf("%d. ", n++);
        display_brief(curr);
        curr = curr->prev;
    } while (curr != r->head->prev);
}

void circular_traversal(ShuttleRoute *r, int startID, int hops) {
    StopNode *start = find(r, startID);
    if (start == NULL) {
        printf("Starting stop ID %d not found.\n", startID);
        return;
    }
    printf("\n========== Circular Traversal from ID %d ==========\n", startID);
    StopNode *curr = start;
    for (int i = 0; i < hops; i++) {
        printf("%d. ", i + 1);
        display_brief(curr);
        curr = curr->next;
    }
}

void show_neighbors(ShuttleRoute *r, int id) {
    StopNode *found = find(r, id);
    if (found == NULL) {
        printf("Stop ID %d not found.\n", id);
        return;
    }
    printf("\n========== Neighbors of Stop ID %d ==========\n", id);
    printf("Current : ");
    display_brief(found);
    printf("Previous: ");
    display_brief(found->prev);
    printf("Next    : ");
    display_brief(found->next);
}

void display_active_route(ShuttleRoute *r) {
    if (r->head == NULL) {
        printf("Route is empty.\n");
        return;
    }
    printf("\n========== Active Stops Only ==========\n");
    StopNode *curr = r->head;
    int n = 0;
    do {
        if (strcmp(curr->status, "Active") == 0) {
            n++;
            printf("%d. ", n);
            display_brief(curr);
        }
        curr = curr->next;
    } while (curr != r->head);
    if (n == 0) {
        printf("No active stops currently.\n");
    }
}

static void free_route(ShuttleRoute *r) {
    if (r->head == NULL) {
        return;
    }
    StopNode *curr = r->head->next;
    while (curr != r->head) {
        StopNode *tmp = curr;
        curr = curr->next;
        free(tmp);
    }
    free(r->head);
    r->head = NULL;
    r->totalStops = 0;
}

static void show_menu(void) {
    printf("\n======================================================\n");
    printf("       SMART CAMPUS SHUTTLE ROUTE MANAGER\n");
    printf("======================================================\n");
    printf("1.  Insert Stop (at end)\n");
    printf("2.  Insert Stop after a given Stop ID\n");
    printf("3.  Delete Stop\n");
    printf("4.  Search Stop\n");
    printf("5.  Update Stop\n");
    printf("6.  Forward Traversal (Full Route)\n");
    printf("7.  Backward Traversal\n");
    printf("8.  Circular Traversal\n");
    printf("9.  Show Next & Previous Stops\n");
    printf("10. Display Active Stops Only\n");
    printf("0.  Exit\n");
    printf("Enter your choice: ");
}

int main(void) {
    ShuttleRoute route = { NULL, 0 };

    insert_stop(&route, 101, "Main Gate", 1, 0.0f, "Active", "07:30 - 21:00");
    insert_stop(&route, 102, "Library", 1, 0.8f, "Active", "07:45 - 21:00");
    insert_stop(&route, 103, "Academic Block A", 1, 0.6f, "Active", "08:00 - 20:30");
    insert_stop(&route, 104, "Hostel Circle", 1, 1.2f, "Active", "08:15 - 20:45");
    insert_stop(&route, 105, "Sports Complex", 1, 0.9f, "Under Construction", "08:30 - 19:00");
    insert_stop(&route, 106, "Cafeteria", 1, 0.5f, "Active", "08:45 - 20:00");

    int choice;
    do {
        show_menu();
        choice = read_int();

        switch (choice) {
            case 1: {
                printf("Stop ID: ");
                int id = read_int();
                printf("Stop Name: ");
                char name[64];
                read_line(name, sizeof name);
                printf("Route Number: ");
                int routeNo = read_int();
                printf("Distance from previous (km): ");
                float dist = read_float();
                printf("Status (Active/Closed/Under Construction): ");
                char status[32];
                read_line(status, sizeof status);
                printf("Timing (e.g. 08:00 - 20:00): ");
                char timing[32];
                read_line(timing, sizeof timing);
                insert_stop(&route, id, name, routeNo, dist, status, timing);
                break;
            }
            case 2: {
                printf("Insert after Stop ID: ");
                int afterID = read_int();
                printf("New Stop ID: ");
                int id = read_int();
                printf("Stop Name: ");
                char name[64];
                read_line(name, sizeof name);
                printf("Route Number: ");
                int routeNo = read_int();
                printf("Distance: ");
                float dist = read_float();
                printf("Status: ");
                char status[32];
                read_line(status, sizeof status);
                printf("Timing: ");
                char timing[32];
                read_line(timing, sizeof timing);
                insert_after(&route, afterID, id, name, routeNo, dist, status, timing);
                break;
            }
            case 3: {
                printf("Stop ID to delete: ");
                int id = read_int();
                delete_stop(&route, id);
                break;
            }
            case 4: {
                printf("Stop ID to search: ");
                int id = read_int();
                search_and_display(&route, id);
                break;
            }
            case 5: {
                printf("Stop ID to update: ");
                int id = read_int();
                printf("New Name: ");
                char name[64];
                read_line(name, sizeof name);
                printf("New Distance: ");
                float dist = read_float();
                printf("New Status: ");
                char status[32];
                read_line(status, sizeof status);
                printf("New Timing: ");
                char timing[32];
                read_line(timing, sizeof timing);
                update_stop(&route, id, name, dist, status, timing);
                break;
            }
            case 6:
                forward_traversal(&route);
                break;
            case 7:
                backward_traversal(&route);
                break;
            case 8: {
                printf("Starting Stop ID: ");
                int startID = read_int();
                printf("Number of hops: ");
                int hops = read_int();
                circular_traversal(&route, startID, hops);
                break;
            }
            case 9: {
                printf("Stop ID: ");
                int id = read_int();
                show_neighbors(&route, id);
                break;
            }
            case 10:
                display_active_route(&route);
                break;
            case 0:
                printf("Exiting. Goodbye.\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    } while (choice != 0);

    free_route(&route);
    return 0;
}