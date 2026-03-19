#include <stdio.h>
#include "types.h"
#include "init.h"
#include "display.h"
#include "banker.h"
#include "rag.h"
#include "deadlock.h"
#include "request.h"
#include "simulation.h"

//declared as extern in types.h (every other module accesses this same variable via extern)
SystemState sys = {0};


static int check_init(void)
{
    if (!sys.initialised) {
        printf("\n  [ERROR] System not initialised.\n");
        printf("  Please run option 1 first.\n");
        return 1;
    }
    return 0;
}


static void print_menu(void)
{
    printf("\n-----------------------------------\n");
    printf("   ADAPTIVE DEADLOCK MANAGEMENT SYSTEM\n");
    printf("\n-----------------------------------\n");

    if (sys.initialised)
        printf("   Mode: %s\n",
               sys.mode == SINGLE_INSTANCE
               ? "Single-Instance (RAG + DFS)"
               : "Multi-Instance  (Banker's Algorithm)");

    printf("----------------------------------------\n");
    printf("  1. Initialise System\n");
    printf("  2. Display System State\n");
    printf("  3. Run Banker's Algorithm (Safe Check)\n");
    printf("  4. Show Resource Allocation Graph\n");
    printf("  5. Detect Deadlock (DFS Cycle Check)\n");
    printf("  6. Request Resource\n");
    printf("  7. Run Random Simulation\n");
    printf("  8. Exit\n");
    printf("----------------------------------------\n");
    printf("  Enter choice: ");
}

int main(void)
{
    int choice;

    printf("\n  Welcome to the Adaptive Deadlock Management System.\n");
    printf("  Start by selecting option 1 to initialise.\n");

    do {
        print_menu();
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                init_system();
                break;

            case 2:
                if (check_init()) break;
                display_system_state();
                break;

            case 3:
                if (check_init()) break;
                run_banker();
                break;

            case 4:
                if (check_init()) break;
                build_rag();
                print_rag();
                print_rag_edges();
                break;

            case 5:
                if (check_init()) break;
                detect_deadlock();
                break;

            case 6:
                if (check_init()) break;
                request_resource();
                /* after every confirmed allocation check if
                   periodic sweep is due                          */
                if (sys.alloc_counter > 0 &&
                    sys.alloc_counter % SWEEP_INTERVAL == 0)
                    run_periodic_sweep();
                break;

            case 7:
                if (check_init()) break;
                run_random_simulation();
                break;

            case 8:
                printf("\n  Exiting. Goodbye.\n");
                break;

            default:
                printf("\n  Invalid choice. Enter 1 to 8.\n");
                break;
        }

    } while (choice != 8);

    return 0;
}