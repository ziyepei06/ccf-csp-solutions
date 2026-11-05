#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, k;
    if (!(cin >> n >> k)) return 0;

    while (k--) {
        int x, y;
        string s;
        cin >> x >> y >> s;

        for (char c : s) {
            int nx = x, ny = y;
            switch (c) {
                case 'f': ny = y + 1; break;  // 向前：(x,y) -> (x,y+1)
                case 'b': ny = y - 1; break;  // 向后：(x,y) -> (x,y-1)
                case 'l': nx = x - 1; break;  // 向左：(x,y) -> (x-1,y)
                case 'r': nx = x + 1; break;  // 向右：(x,y) -> (x+1,y)
            }
            // 目标位置在场地外时位置保持不变，即等价于把坐标夹到 [1,n]
            if (nx >= 1 && nx <= n && ny >= 1 && ny <= n) {
                x = nx;
                y = ny;
            }
        }

        cout << x << ' ' << y << '\n';
    }

    return 0;
}
