#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

const char* DATABASE_FILE = "database.csv";

typedef enum {
    Barbell,
    Dumbell,
    Machine,
    Bodyweight,
    EzBar,
    DipBelt,
} Equipment;

typedef enum {
    Lower,
    Higher,
    None,
} RepRange;

typedef struct {
    const char* name;
    Equipment equipment;
    RepRange reprange;
} Exercise;

int setup_csv() {
    FILE* fptr = fopen(DATABASE_FILE, "w");
    if (!fptr) {
        perror("Failed to open file");
        return 1;
    }

    int err = fprintf(fptr, "Name, Equipment, Rep Range\n");
    if (err < 0) {
        perror("Failed to write to file");
        return 1;
    }

    fclose(fptr);
    return 0;
}

int write_exercise(char* exercise) {
    FILE* fptr = fopen(DATABASE_FILE, "a"); 
    if (!fptr) {
        perror("Failed to open file");
        return 1;
    }

    int err = fprintf(fptr, "%s", exercise);
    if (err < 0) {
        perror("Failed to write exercise to file");
        return 1;
    }
    
    fclose(fptr);
    return 0;
}

void exercise_str(char* buff, Exercise ex, size_t size) {
    const char* equipment;
    const char* reprange;

    switch (ex.equipment) {
        case Barbell:    equipment = "Barbell";    break;
        case Dumbell:    equipment = "Dumbell";    break;
        case Machine:    equipment = "Machine";    break;
        case Bodyweight: equipment = "Bodyweight"; break;
        case EzBar:      equipment = "EzBar";      break;
        case DipBelt:    equipment = "DipBelt";    break;
        default:         equipment = "Not Found";  break;
    }
    switch (ex.reprange) {
        case Lower:  reprange = "Lower";  break;
        case Higher: reprange = "Higher"; break;
        default:     reprange = "None";   break;
    }

    snprintf(buff, size, "%s, %s, %s\n", ex.name, equipment, reprange);
}

bool dbfile_exists() {
    FILE* fptr = fopen(DATABASE_FILE, "r");
    if (fptr) {
        fclose(fptr);
        return true; // file exists
    }
    return false;
}

Equipment parse_equipment(const char* s) {
    if (strcmp(s, "Barbell") == 0)    return Barbell;
    if (strcmp(s, "Dumbell") == 0)    return Dumbell;
    if (strcmp(s, "Machine") == 0)    return Machine;
    if (strcmp(s, "Bodyweight") == 0) return Bodyweight;
    if (strcmp(s, "EzBar") == 0)      return EzBar;
    if (strcmp(s, "DipBelt") == 0)    return DipBelt;
    return -1; // invalid
}

RepRange parse_reprange(const char* s) {
    if (strcmp(s, "Lower") == 0) return Lower;
    if (strcmp(s, "Higher") == 0) return Higher;
    if (strcmp(s, "None") == 0)  return None;
    return -1; // invalid
}

int delete_exercise(const char* name) {
    FILE* src = fopen(DATABASE_FILE, "r");
    if (!src) {
        perror("Failed to open source CSV");
        return 1;
    }

    FILE* tmp = fopen("database.tmp", "w");
    if (!tmp) {
        perror("Failed to open temporary file");
        fclose(src);
        return 1;
    }

    char line[256];
    bool deleted = false;

    if (fgets(line, sizeof(line), src)) {
        fputs(line, tmp);
    }

    while (fgets(line, sizeof(line), src)) {
        if (strncmp(line, name, strlen(name)) == 0 && line[strlen(name)] == ',') {
            deleted = true;
            continue;
        }

        fputs(line, tmp);
    }

    fclose(src);
    fclose(tmp);

    // Replace original file
    remove(DATABASE_FILE);
    rename("database.tmp", DATABASE_FILE);

    return deleted ? 0 : 2; // return 2 = not found
}



int main(int argc, char *argv[]) {
    if (argc <= 1) {
        perror("No argument was included");
        return 1;
    }

    if (!dbfile_exists()) {
        printf("database does not exist, file created...\n");
        int ret = setup_csv();
        if (ret != 0) return ret;
    }

    // write logic to write to file
    if (strcmp(argv[1], "write") == 0) {
        if (argc < 5) {
            fprintf(stderr, "Usage: %s write <name> <equipment> <reprange>\n", argv[0]);
            return 1;
        }

        Exercise new_ex;
        new_ex.name      = argv[2];
        new_ex.equipment = parse_equipment(argv[3]);
        new_ex.reprange  = parse_reprange(argv[4]);

        if (new_ex.equipment == -1 || new_ex.reprange == -1) {
            return -1;
        }
        char buff[100];
        exercise_str(buff, new_ex, sizeof(buff));
        if (write_exercise(buff) != 0) {
            return 1;
        }
        return 0;
    }
    // write logic to delete from file
    if (strcmp(argv[1], "delete") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: %s delete <name>\n", argv[0]);
            return 1;
        }
        int result = delete_exercise(argv[2]);
        if (result == 2) {
            printf("Exercise not found.\n");
        }
        return result;
    }
    // write logic to seach and return exercise from file - Arthur

    return 0;
}
