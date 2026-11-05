#include <bits/stdc++.h>
using namespace std;

// 判断闰年：4 的倍数且非 100 的倍数，或 400 的倍数
static bool isLeap(int y) {
    return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
}

int main() {
    int y, d;
    if (scanf("%d", &y) != 1) return 0;
    if (scanf("%d", &d) != 1) return 0;

    int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (isLeap(y)) days[1] = 29;

    // 从 1 月开始逐月扣除，找到第 d 天所在月份
    int month = 1;
    for (int i = 0; i < 12; ++i) {
        if (d <= days[i]) {
            month = i + 1;
            break;
        }
        d -= days[i];
    }

    printf("%d\n%d\n", month, d);
    return 0;
}
