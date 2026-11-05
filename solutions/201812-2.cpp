#include <cstdio>

int main() {
    long long r, y, g;
    if (scanf("%lld %lld %lld", &r, &y, &g) != 3) return 0;
    long long cycle = r + y + g;

    int n;
    if (scanf("%d", &n) != 1) return 0;

    long long ans = 0;  // 已经过去的总时间（秒）

    for (int i = 0; i < n; ++i) {
        long long k, t;
        if (scanf("%lld %lld", &k, &t) != 2) return 0;

        if (k == 0) {
            // 经过一段道路
            ans += t;
            continue;
        }

        // 把“出发时刻的灯状态”换算成循环内的相位位置
        // 循环顺序：红 [0, r) -> 绿 [r, r+g) -> 黄 [r+g, r+g+y)
        long long pos;
        if (k == 1) {
            // 红灯，剩余 t 秒 -> 相位 r - t
            pos = r - t;
        } else if (k == 3) {
            // 绿灯，剩余 t 秒 -> 相位 r + g - t
            pos = r + g - t;
        } else {
            // 黄灯，剩余 t 秒 -> 相位 r + g + y - t
            pos = cycle - t;
        }

        // 到达该路口时的相位
        pos = (pos + ans) % cycle;

        if (pos < r) {
            // 红灯，需要等灯变绿
            ans += r - pos;
        } else if (pos < r + g) {
            // 绿灯，直接通过
        } else {
            // 黄灯，等黄灯结束（剩 cycle - pos）再等一个红灯 r
            ans += cycle - pos + r;
        }
    }

    printf("%lld\n", ans);
    return 0;
}
