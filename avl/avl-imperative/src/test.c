int main() {
    char* x = malloc(2);
    long y = ((long) x) | 1;
    char* z = (char*) y;
    printf("%lu %lu %lu\n", x, y, z);
}