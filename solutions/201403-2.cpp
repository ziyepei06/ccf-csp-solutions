#include <bits/stdc++.h>
using namespace std;

struct Win {
    int x1, y1, x2, y2;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    if (!(cin >> n >> m)) return 0;

    vector<Win> w(n);
    for (int i = 0; i < n; ++i) {
        cin >> w[i].x1 >> w[i].y1 >> w[i].x2 >> w[i].y2;
    }

    // order[0] 为最底层，order.back() 为最顶层
    vector<int> order(n);
    for (int i = 0; i < n; ++i) order[i] = i;

    for (int q = 0; q < m; ++q) {
        int x, y;
        cin >> x >> y;
        int hit = -1;
        // 从顶层向底层找第一个命中的窗口
        for (int k = n - 1; k >= 0; --k) {
            int id = order[k];
            if (x >= w[id].x1 && x <= w[id].x2 &&
                y >= w[id].y1 && y <= w[id].y2) {
                hit = k;
                break;
            }
        }
        if (hit == -1) {
            cout << "IGNORED\n";
        } else {
            cout << order[hit] + 1 << '\n';
            // 被点击的窗口移到最顶层，其余窗口相对顺序不变
            int id = order[hit];
            order.erase(order.begin() + hit);
            order.push_back(id);
        }
    }
    return 0;
}
