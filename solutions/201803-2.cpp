#include <bits/stdc++.h>
using namespace std;

int main() {
    int n, L, t;
    if (!(cin >> n >> L >> t)) return 0;
    vector<int> pos(n), dir(n, 1);
    for (int i = 0; i < n; ++i) cin >> pos[i];

    for (int sec = 0; sec < t; ++sec) {
        // 先整体移动一步
        for (int i = 0; i < n; ++i) pos[i] += dir[i];
        // 到达端点则立即反向
        for (int i = 0; i < n; ++i)
            if (pos[i] == 0 || pos[i] == L) dir[i] = -dir[i];
        // 两球相撞则各自反向（题目保证不会有三球同时相撞）
        for (int i = 0; i < n; ++i)
            for (int j = i + 1; j < n; ++j)
                if (pos[i] == pos[j]) {
                    dir[i] = -dir[i];
                    dir[j] = -dir[j];
                }
    }

    for (int i = 0; i < n; ++i) {
        if (i) cout << ' ';
        cout << pos[i];
    }
    cout << '\n';
    return 0;
}
