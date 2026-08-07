#include "backtracking.h"
#include "cell.h"
#include "grid.h"
#include "solving.h"
#include "worklist.h"
#include <assert.h>
#include <bits/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_SOLVER_ARGS 2 // Just executable call and input file
#define MAX_SOLVER_ARGS 3 // Executable call, input file, output file
#define INPUT_FILE_ARGV_POS 1
#define OUTPUT_FILE_ARGV_POS 2
#define INPUT_FILE_OPEN_MODE "r"
#define OUTPUT_FILE_OPEN_MODE "w"
#define LINE_SIZE 11
#define LINE_SIZE_LONG (GRID_DIMENSION * GRID_DIMENSION + 3)

typedef enum {
    END,
    EASY = 1,
    MEDIUM = 2,
    HARD = 3,
    EXPERT = 4,
    EVIL = 5,
} difficulty;

difficulty read_board(worklist *wl, FILE *input_stream) {
    char line[LINE_SIZE_LONG];
    coordinate coord;
    if (fgets(line, LINE_SIZE_LONG, input_stream) != NULL) {
        for (int i = 0; i < GRID_DIMENSION * GRID_DIMENSION; i++) {
            digit_t digit = line[i] - '0';
            coord = number_to_coordinate(i);
            if (digit != 0) {
                place_digit(coord.row, coord.column, digit, wl);
            }
        }

        digit_t diff = line[GRID_DIMENSION * GRID_DIMENSION] - '0';
        return diff;
    }
    return END;
}
int init_csv(FILE *output_stream) {
    fprintf(output_stream,
            "puzzleID, algorithm, difficulty, timeElapsed, guessCount\n");
    return EXIT_SUCCESS;
}
int add_to_csv(difficulty diff, double timeElapsed,
               backtracking_type backtrack_algorithm, int guesses,
               bool solving_algorithms, FILE *output_stream, int id,
               uint64_t guess_count) {
    (void)guesses;
    fprintf(output_stream, "%d, %d - %s, %d, %f, %lld\n", id,
            backtrack_algorithm,
            solving_algorithms ? "with algorithms" : "without algorithms", diff,
            timeElapsed, (long long)guess_count);
    return EXIT_SUCCESS;
}

int run_solve_instance(FILE *input_stream, FILE *output_stream,
                       backtracking_type backtracking_type,
                       bool with_algorithm) {
    worklist wl;
    uint64_t *guess_count = malloc(sizeof(uint64_t));
    assert(guess_count != NULL);
    // float startTime, endTime, timeElapsed;
    struct timespec startTime, endTime;
    difficulty diff;
    for (int i = 0; i >= 0; i++) {
        populate_grid();
        wl = worklist_create_empty();
        diff = read_board(&wl, input_stream);
        if (diff == END) {
            return EXIT_SUCCESS;
        }

        // startTime = (float)clock() / CLOCKS_PER_SEC;
        clock_gettime(CLOCK_MONOTONIC, &startTime);

        *guess_count = 0;
        solve_backtracking(&wl, backtracking_type, with_algorithm, guess_count);

        // endTime = (float)clock() / CLOCKS_PER_SEC;
        clock_gettime(CLOCK_MONOTONIC, &endTime);

        // timeElapsed = endTime - startTime;
        // stores the timeElapsed in microseconds
        double timeElapsed = (endTime.tv_sec - startTime.tv_sec) * 1e6 +
                             (endTime.tv_nsec - startTime.tv_nsec) / 1e3;

        add_to_csv(diff, timeElapsed, backtracking_type, 0, with_algorithm,
                   output_stream, i, *guess_count);
        char buffer[MAX_PRINT_LENGTH];
        grid_to_string(buffer);
        printf("Puzzle %d: \n%s\n\n", i, buffer);
        worklist_free(wl);
    }
    free(guess_count);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    if (argc < MIN_SOLVER_ARGS || argc > MAX_SOLVER_ARGS) {
        fprintf(stderr,
                "Solver IO error: Invalid argument count %d. Usage: "
                "./sudoku-solver <input_file> <output_file>(?).\n",
                argc);
        return EXIT_FAILURE;
    }

    FILE *input_stream = fopen(argv[INPUT_FILE_ARGV_POS], INPUT_FILE_OPEN_MODE);

    if (input_stream == NULL) {
        fprintf(stderr,
                "Solver IO error: Couldn't find or open input file '%s'.\n",
                argv[INPUT_FILE_ARGV_POS]);
        return EXIT_FAILURE;
    }

    const bool output_stream_is_file = argc == MAX_SOLVER_ARGS;
    FILE *output_stream =
        output_stream_is_file
            ? fopen(argv[OUTPUT_FILE_ARGV_POS], OUTPUT_FILE_OPEN_MODE)
            : stdout;
    if (output_stream == NULL) {
        fprintf(stderr,
                "Solver IO error: Couldn't find or open output file '%s'.\n",
                argv[OUTPUT_FILE_ARGV_POS]);
        return EXIT_FAILURE;
    }

    init_csv(output_stream);

    /*run_solve_instance(input_stream, output_stream, BACTRACK, false);
    rewind(input_stream);*/

    /*run_solve_instance(input_stream, output_stream, MRV, false);
    rewind(input_stream);*/

    run_solve_instance(input_stream, output_stream, SIMPLE, true);
    rewind(input_stream);
    run_solve_instance(input_stream, output_stream, MRV_HEURISTIC, true);
    rewind(input_stream);
    run_solve_instance(input_stream, output_stream, MRV_HEURISTIC, false);
    rewind(input_stream);
    run_solve_instance(input_stream, output_stream, SIMPLE, false);

    fclose(input_stream);
    if (output_stream_is_file) {
        fclose(output_stream);
    }
    return EXIT_SUCCESS;
}
