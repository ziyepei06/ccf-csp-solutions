#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long total = 0;
    long long last = 0;      // 上一次跳跃的得分；0 表示本局尚未跳跃
    int x;
    while (cin >> x) {
        if (x == 0) break;
        if (x == 1) {
            last = 1;
            total += 1;
        } else {             // x == 2，跳到中心
            if (last == 0 || last == 1) last = 2;   // 第一次跳跃或上次得 1 分
            else last = last + 2;                    // 连续跳中心
            total += last;
        }
    }
    cout << total << "\n";
    return 0;
}
