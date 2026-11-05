// 202012-2 期末预测之最佳阈值
// 排序 + 前缀和：候选阈值只取出现过的 y，按 y 升序扫描即可 O(m log m)。
#include <bits/stdc++.h>
using namespace std;

int main() {
    int m;
    if (scanf("%d", &m) != 1) return 0;
    vector<pair<int, int>> a(m);  // (y, result)
    for (int i = 0; i < m; ++i) scanf("%d %d", &a[i].first, &a[i].second);

    sort(a.begin(), a.end());  // 按 y 升序（y 相同则按 result 升序，不影响正确数统计）

    long long cnt1 = 0;  // 全体 result==1 的人数
    for (int i = 0; i < m; ++i)
        if (a[i].second == 1) ++cnt1;

    long long pref0 = 0, pref1 = 0;  // y < theta 中 result 为 0 / 1 的个数
    long long bestAcc = -1;
    int bestTheta = -1;

    for (int i = 0; i < m;) {
        int j = i;
        while (j < m && a[j].first == a[i].first) ++j;

        // 以 theta = a[i].first 为阈值：y<theta 预测 0，y>=theta 预测 1
        long long acc = pref0 + (cnt1 - pref1);
        if (acc >= bestAcc) {  // 升序扫描，相等时取更大的 theta
            bestAcc = acc;
            bestTheta = a[i].first;
        }

        // 把当前这一组（y == a[i].first）并入前缀，供后续更大的阈值使用
        for (int k = i; k < j; ++k) {
            if (a[k].second == 0) ++pref0;
            else ++pref1;
        }
        i = j;
    }

    printf("%d\n", bestTheta);
    return 0;
}
