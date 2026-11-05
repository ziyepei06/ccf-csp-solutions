#include <bits/stdc++.h>
using namespace std;

int main() {
    long long r, y, g;
    if (!(cin >> r >> y >> g)) return 0;
    int n;
    cin >> n;
    long long total = 0;
    for (int i = 0; i < n; ++i) {
        int k;
        long long t;
        cin >> k >> t;
        if (k == 0) {
            // 经过一段道路，直接耗时 t
            total += t;
        } else if (k == 1) {
            // 红灯：等完剩余倒计时 t 秒即可通行
            total += t;
        } else if (k == 2) {
            // 黄灯：等完剩余 t 秒黄灯，之后必然进入红灯，再等 r 秒
            total += t + r;
        } else {
            // 绿灯：直接通过，不耗时
        }
    }
    cout << total << '\n';
    return 0;
}
