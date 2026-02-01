#include <stdio.h>
#include <stdbool.h>
#include <string.h>

int main() {
    char buffer[256];
    char *prompt = "mysh> ";

    while (true) {
        printf("%s", prompt);
        fgets(buffer, 256, stdin);

        if (strcmp(buffer, "exit\n") == 0){
            break;
        }


        printf("%s", buffer);
    }

    return 0;
}

