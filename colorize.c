#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

typedef enum { SINGLE, BOUNDARY } MatchType;

typedef struct StringColorPair {
    char *string;
    char *string2;
    char *color;
    MatchType type;
    struct StringColorPair *next;
} StringColorPair;

const char *get_ansi_color(const char *name) {
    if (strcmp(name, "black") == 0) return "30";
    if (strcmp(name, "red") == 0) return "31";
    if (strcmp(name, "green") == 0) return "32";
    if (strcmp(name, "yellow") == 0) return "33";
    if (strcmp(name, "blue") == 0) return "34";
    if (strcmp(name, "magenta") == 0) return "35";
    if (strcmp(name, "cyan") == 0) return "36";
    if (strcmp(name, "white") == 0) return "37";
    if (strcmp(name, "bright_black") == 0) return "90";
    if (strcmp(name, "bright_red") == 0) return "91";
    if (strcmp(name, "bright_green") == 0) return "92";
    if (strcmp(name, "bright_yellow") == 0) return "93";
    if (strcmp(name, "bright_blue") == 0) return "94";
    if (strcmp(name, "bright_magenta") == 0) return "95";
    if (strcmp(name, "bright_cyan") == 0) return "96";
    if (strcmp(name, "bright_white") == 0) return "97";
    if (strcmp(name, "dark_gray") == 0) return "100";
    if (strcmp(name, "bright_gray") == 0) return "47";
    return NULL;
}

void print_colored(const char *text, const char *color) {
    printf("\033[%sm%s\033[0m", color, text);
}

void process_stream(StringColorPair *head, FILE *input) {
    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, input)) {
        char *current = buffer;
        while (*current) {
            int matched = 0;
            StringColorPair *node = head;
            while (node) {
                if (node->type == SINGLE) {
                    size_t len = strlen(node->string);
                    if (strncmp(current, node->string, len) == 0) {
                        print_colored(node->string, node->color);
                        current += len;
                        matched = 1;
                        break;
                    }
                } else if (node->type == BOUNDARY) {
                    size_t start_len = strlen(node->string);
                    if (strncmp(current, node->string, start_len) == 0) {
                        char *start = current + start_len;
                        char *end = strstr(start, node->string2);
                        if (end) {
                            printf("%s", node->string);
                            current += start_len;
                            printf("\033[%sm", node->color);
                            while (current < end) {
                                putchar(*current);
                                current++;
                            }
                            printf("\033[0m");
                            printf("%s", node->string2);
                            current += strlen(node->string2);
                            matched = 1;
                        }
                        break;
                    }
                }
                node = node->next;
            }
            if (!matched) {
                putchar(*current);
                current++;
            }
        }
        fflush(stdout);
    }
}

void free_pairs(StringColorPair *head) {
    while (head) {
        StringColorPair *temp = head;
        head = head->next;
        free(temp->color);
        free(temp);
    }
}

void print_help() {
    printf("Supported colors:\n");
    printf("  black, red, green, yellow, blue, magenta, cyan, white\n");
    printf("  bright_black, bright_red, bright_green, bright_yellow, bright_blue, bright_magenta, bright_cyan, bright_white\n");
    printf("  dark_gray, bright_gray\n");
    printf("Colors can be combined with backgrounds using a colon, e.g., bright_red:dark_gray\n");
    printf("\nUsage:\n");
    printf("  colorize -m <string1> <color1> [-m <string2> <color2> ...] [-b <start> <end> <color> ...]\n");
    printf("\nExample:\n");
    printf("  echo 'Hello World!' | colorize -m Hello bright_red -m World bright_green -b Hello World cyan\n");
}

char *build_ansi_sequence(const char *color) {
    char *result = malloc(32);
    if (!result) return NULL;

    char color_part[16] = "";
    char background_part[16] = "";

    const char *colon_pos = strchr(color, ':');
    if (colon_pos) {
        size_t color_len = colon_pos - color;
        if (color_len >= sizeof(color_part)) {
            free(result);
            fprintf(stderr, "Foreground color name is too long: %.*s\n", (int)color_len, color);
            return NULL;
        }
        strncpy(color_part, color, color_len);
        color_part[color_len] = '\0';
        strncpy(background_part, colon_pos + 1, sizeof(background_part) - 1);
        background_part[sizeof(background_part) - 1] = '\0';
    } else {
        strncpy(color_part, color, sizeof(color_part) - 1);
        color_part[sizeof(color_part) - 1] = '\0';
    }

    const char *fg_code = get_ansi_color(color_part);
    const char *bg_code = background_part[0] ? get_ansi_color(background_part) : NULL;

    if (!fg_code) {
        fprintf(stderr, "Unknown foreground color: %s\n", color_part);
        free(result);
        return NULL;
    }

    if (background_part[0] && !bg_code) {
        fprintf(stderr, "Unknown background color: %s\n", background_part);
        free(result);
        return NULL;
    }

    if (fg_code && bg_code) {
        snprintf(result, 32, "%s;%d", fg_code, atoi(bg_code) + 10);
    } else if (fg_code) {
        snprintf(result, 32, "%s", fg_code);
    }

    return result;
}

StringColorPair *parse_arguments(int argc, char *argv[]) {
    StringColorPair *head = NULL;
    StringColorPair *tail = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-m") == 0) {
            if (i + 2 >= argc) {
                fprintf(stderr, "Invalid argument format. Expected -m <string> <color>\n");
                free_pairs(head);
                return NULL;
            }

            char *ansi_sequence = build_ansi_sequence(argv[i + 2]);
            if (!ansi_sequence) {
                free_pairs(head);
                return NULL;
            }

            StringColorPair *new_pair = malloc(sizeof(StringColorPair));
            if (!new_pair) {
                fprintf(stderr, "Memory allocation failed\n");
                free_pairs(head);
                free(ansi_sequence);
                return NULL;
            }

            new_pair->string = argv[i + 1];
            new_pair->string2 = NULL;
            new_pair->color = ansi_sequence;
            new_pair->type = SINGLE;
            new_pair->next = NULL;

            if (!head) {
                head = new_pair;
                tail = new_pair;
            } else {
                tail->next = new_pair;
                tail = new_pair;
            }

            i += 2;
        } else if (strcmp(argv[i], "-b") == 0) {
            if (i + 3 >= argc) {
                fprintf(stderr, "Invalid argument format. Expected -b <start> <end> <color>\n");
                free_pairs(head);
                return NULL;
            }

            char *ansi_sequence = build_ansi_sequence(argv[i + 3]);
            if (!ansi_sequence) {
                free_pairs(head);
                return NULL;
            }

            StringColorPair *new_boundary = malloc(sizeof(StringColorPair));
            if (!new_boundary) {
                fprintf(stderr, "Memory allocation failed\n");
                free_pairs(head);
                free(ansi_sequence);
                return NULL;
            }

            new_boundary->string = argv[i + 1];
            new_boundary->string2 = argv[i + 2];
            new_boundary->color = ansi_sequence;
            new_boundary->type = BOUNDARY;
            new_boundary->next = NULL;

            if (!head) {
                head = new_boundary;
                tail = new_boundary;
            } else {
                tail->next = new_boundary;
                tail = new_boundary;
            }

            i += 3;
        } else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            free_pairs(head);
            return NULL;
        }
    }

    return head;
}

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        print_help();
        return 0;
    }

    StringColorPair *head = parse_arguments(argc, argv);
    if (!head) {
        return 1;
    }

    process_stream(head, stdin);

    free_pairs(head);

    return 0;
}
