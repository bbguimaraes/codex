static void print_board(const struct board *b) {
    puts("     A   B   C   D   E   F   G   H");
    puts("   +-------------------------------+");
    for(int y = 7; y != -1; --y) {
        printf(" %d |", y + 1);
        for(int x = 0; x != 8; x += 2)
            print_pos(b, x, y), print_pos(b, x + 1, y);
        putchar('\n');
        if(y)
            puts("   |---+---+---+---+---+---+---+---|");
    }
    puts("   +-------------------------------+");
}
