#include <stdio.h>
#include <string.h>

#define BUF_SIZE 1024
#define BODY_SIZE 2000

int main(void) {
    char buffer[BUF_SIZE];
    char body[BODY_SIZE];
    int body_index = 0;

    FILE *fp = fopen("index.html", "r");
    if (fp == NULL) {
        printf("Error: Could not open file index.html\n");
        return 1;
    }

    // Read the file into buffer
    while (fgets(buffer, BUF_SIZE, fp) != NULL) {
        int len = strlen(buffer);

        if (body_index + len >= BODY_SIZE) {
            printf("Error: Body is too large!\n");
            return 1;
        }

        strcpy(body + body_index, buffer);
        body_index += len;
    }

    printf("Body:\n%s\n", body);

    fclose(fp);

    return 0;
}
