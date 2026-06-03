#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef unsigned char BYTE;
static void die(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}
static void read_record(FILE *f, BYTE *buf, int rec_len, long i) {
    fseek(f, i * rec_len, SEEK_SET);
    if ((int)fread(buf, 1, rec_len, f) != rec_len) die("fread failed");
}
static void write_record(FILE *f, const BYTE *buf, int rec_len, long i) {
    fseek(f, i * rec_len, SEEK_SET);
    if ((int)fwrite(buf, 1, rec_len, f) != rec_len) die("fwrite failed");
}
static void swap_records(FILE *f, int rec_len, long i, long j) {
    BYTE *bi = malloc(rec_len), *bj = malloc(rec_len);
    if (!bi || !bj) die("malloc failed");
    read_record(f, bi, rec_len, i);
    read_record(f, bj, rec_len, j);
    write_record(f, bj, rec_len, i);
    write_record(f, bi, rec_len, j);
    free(bi); free(bj);
}
static long count_records(FILE *f, int rec_len) {
    fseek(f, 0, SEEK_END);
    return ftell(f) / rec_len;
}
static void bubble_sort(FILE *f, int rec_len,
                        int key_start, int key_len,
                        int ascending, long n)
{
    BYTE *bi = malloc(rec_len), *bj = malloc(rec_len);
    if (!bi || !bj) die("malloc failed");
    for (long i = 0; i < n - 1; i++) {
        for (long j = 0; j < n - 1 - i; j++) {
            read_record(f, bi, rec_len, j);
            read_record(f, bj, rec_len, j + 1);
            int cmp = memcmp(bi + key_start, bj + key_start, key_len);
            if (ascending ? cmp > 0 : cmp < 0)
                swap_records(f, rec_len, j, j + 1);
        }
    }
    free(bi); free(bj);
}
static void copy_file(const char *src_path, const char *dst_path) {
    FILE *src = fopen(src_path, "rb");
    if (!src) die("Cannot open source file");
    FILE *dst = fopen(dst_path, "wb");
    if (!dst) { fclose(src); die("Cannot create destination file"); }
    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof buf, src)) > 0)
        fwrite(buf, 1, n, dst);
    fclose(src); fclose(dst);
}
int main(void) {
    char src_path[256];
    const char dst_path[] = "FILE.SRT";
    int rec_len, key_start, key_end, sort_order;
    printf("Путь к файлу: ");          scanf("%255s", src_path);
    printf("Длина записи (байт): ");   scanf("%d", &rec_len);
    printf("Первый байт ключа: ");     scanf("%d", &key_start);
    printf("Последний байт ключа: ");  scanf("%d", &key_end);
    printf("Порядок (1=возр, 0=убыв): "); scanf("%d", &sort_order);
    copy_file(src_path, dst_path);
    FILE *f = fopen(dst_path, "r+b");
    if (!f) die("Cannot open output file");
    long n = count_records(f, rec_len);
    printf("Записей: %ld\n", n);
    if (n >= 2)
        bubble_sort(f, rec_len, key_start, key_end - key_start + 1, sort_order, n);
    fclose(f);
    printf("Готово: %s\n", dst_path);
    return EXIT_SUCCESS;
}
