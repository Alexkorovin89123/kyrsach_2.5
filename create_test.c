#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    srand(time(NULL));
    FILE *f = fopen("data.bin", "wb");
    if (!f) { printf("Ошибка создания файла\n"); return 1; }

    int rec_len = 10;
    int num_records = 20;

    for (int i = 0; i < num_records; i++) {
        for (int j = 0; j < rec_len; j++) {
            unsigned char byte = rand() % 256;
            fwrite(&byte, 1, 1, f);
        }
    }

    fclose(f);
    printf("Файл data.bin создан\n");
    return 0;
}
