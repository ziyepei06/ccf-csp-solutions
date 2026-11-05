#include <bits/stdc++.h>
using namespace std;

static bool isLeap(int y) {
    return (y % 400 == 0) || (y % 4 == 0 && y % 100 != 0);
}

int main() {
    int a, b, c, y1, y2;
    if (!(cin >> a >> b >> c >> y1 >> y2)) return 0;

    static const int md[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // base：1850/01/01 到 (y)/01/01 的天数差；1850/01/01 是星期二，记 c=1..7 表示周一..周日
    // 星期编号 w = ((base + 1) % 7) + 1，base=0 时 w=2 即星期二
    long long base = 0;
    for (int y = 1850; y < y1; y++) base += 365 + (isLeap(y) ? 1 : 0);

    for (int y = y1; y <= y2; y++) {
        long long days = base;
        for (int m = 1; m < a; m++) days += md[m] + ((m == 2 && isLeap(y)) ? 1 : 0);

        int w = (int)((days + 1) % 7) + 1;             // 该年 a 月 1 日的星期（1=周一 ... 7=周日）
        int first = 1 + ((c - w) % 7 + 7) % 7;          // 该月第一个星期 c 的日期
        int date = first + 7 * (b - 1);                 // 第 b 个星期 c
        int dim = md[a] + ((a == 2 && isLeap(y)) ? 1 : 0);

        if (date > dim) {
            cout << "none\n";
        } else {
            cout << y << '/';
            cout << setw(2) << setfill('0') << a << '/';
            cout << setw(2) << setfill('0') << date << '\n';
        }
        base += 365 + (isLeap(y) ? 1 : 0);              // 进入下一年，维护 base
    }
    return 0;
}
