// 202309-2 坐标变换（其二）
// 拉伸与旋转（统一缩放 + 旋转）都是复平面上的乘法，二者可交换，
// 因此区间 [i,j] 的复合变换 = (角度和, 拉伸系数积)。
// 用前缀和预处理角度和与 ln(缩放) 之和，每次查询 O(1)。
#include <bits/stdc++.h>
using namespace std;

int main() {
    int n, m;
    if (scanf("%d %d", &n, &m) != 2) return 0;
    vector<double> preA(n + 1, 0.0), preS(n + 1, 0.0);
    for (int t = 1; t <= n; ++t) {
        int op;
        double v;
        scanf("%d %lf", &op, &v);
        preA[t] = preA[t - 1];
        preS[t] = preS[t - 1];
        if (op == 1) preS[t] += log(v);
        else preA[t] += v;
    }
    for (int q = 0; q < m; ++q) {
        int i, j;
        double x, y;
        scanf("%d %d %lf %lf", &i, &j, &x, &y);
        double ang = preA[j] - preA[i - 1];
        double scl = exp(preS[j] - preS[i - 1]);
        double c = cos(ang), s = sin(ang);
        double nx = x * c - y * s;
        double ny = x * s + y * c;
        printf("%.6f %.6f\n", nx * scl, ny * scl);
    }
    return 0;
}
