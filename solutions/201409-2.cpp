#include <bits/stdc++.h>
using namespace std;

int main() {
    int n;
    if (scanf("%d", &n) != 1) return 0;

    // 坐标系中一个单位正方形由整数点 (x,y) 与 (x+1,y+1) 围成。
    // 题目坐标范围 0..100，网格取 0..100 的单元即可（左下角坐标最大到 100 时该单元不存在，
    // 因为右上角 <= 100，所以实际被涂的单元左下角坐标 <= 99，开 101 足够安全）。
    const int C = 101;
    vector<vector<char>> painted(C, vector<char>(C, 0));

    for (int i = 0; i < n; ++i) {
        int x1, y1, x2, y2;
        scanf("%d %d %d %d", &x1, &y1, &x2, &y2);
        for (int x = x1; x < x2; ++x) {
            for (int y = y1; y < y2; ++y) {
                painted[x][y] = 1;
            }
        }
    }

    long long area = 0;
    for (int x = 0; x < C; ++x)
        for (int y = 0; y < C; ++y)
            if (painted[x][y]) ++area;

    printf("%lld\n", area);
    return 0;
}
