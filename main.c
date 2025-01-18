#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE 1

typedef struct {
    size_t len;
    size_t capacity;
    char** data;
} DynString;

DynString* init_dynstr() {
    DynString* ds = malloc(sizeof(DynString));
    ds->len = 0;
    ds->capacity = 20;
    ds->data = malloc(sizeof(char*) * ds->capacity);

    return ds;
}

void ds_append(DynString* ds, char* tok) {
    if (ds->len >= ds->capacity) {
        ds->capacity *= 2;
        char** tmp = malloc(sizeof(char*)*ds->capacity);

        memmove(tmp, ds->data, ds->len);
        free((void*)ds->data);

        ds->data = tmp;
    }

    ds->data[ds->len++] = tok;
}

char* resolve_tok(char* tok, int resolution);

DynString* parse_command(char* command, int resolution) {
    DynString* ds = init_dynstr();

    char* tok = strtok(command, " ");
    ds_append(ds, resolve_tok(tok, resolution));

    while((tok = strtok(NULL, " ")) != NULL) {
        ds_append(ds, resolve_tok(tok, resolution));
    }

    return ds;
}

char* resolve_tok(char* tok, int resolution) {
    if (tok == NULL) {
        printf("%s\n", "Error parsing the command");
    }

    if (tok[0] == '$' && resolution) {
        tok++;
        char* env_val = getenv(tok);
        if (env_val == NULL) {
            printf("Could not resolve env var: %s\n", tok);
            return tok;
        }

        return env_val;
    }

    return tok;
}

int main() {
    /*
    * What we expect is: [ls, -alh, /dev]
    * Other examples incldues:
    * ls -alh $PWD -> running with resolve_evars on we should get -> [ls, -alh, /home/{...user}]
    * ls -alh $VAR -> we can provide a var to the resolved such as set_evar("VAR", "some_value") -> [ls, -alh, some_value]
    */
    char command[] = "ls -alh $PWD";
    int resolution = TRUE;

    DynString* ds = parse_command(command, resolution);

    printf("[ ");
    for (size_t i = 0; i < ds->len; i++) {
        printf("%s", ds->data[i]);
        if (i + 1 == ds->len) { // if its the last element don't print a ,
            printf(" ]\n");
            break;
        }
        printf(", ");
    }
}
