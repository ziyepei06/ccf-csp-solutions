#include <bits/stdc++.h>
using namespace std;

int main() {
    int n;
    if (!(cin >> n)) return 0;
    vector<double> a(n);
    double sum = 0.0;
    for (int i = 0; i < n; ++i) {
        cin >> a[i];
        sum += a[i];
    }
    // 平均值
    double mean = sum / n;
    // 方差 D = (1/n) * sum (a[i] - mean)^2
    double var = 0.0;
    for (int i = 0; i < n; ++i) {
        double d = a[i] - mean;
        var += d * d;
    }
    var /= n;
    // 标准差（题面保证方差 >= 1，不必防 0）
    double sd = sqrt(var);

    cout.setf(ios::fixed);
    cout << setprecision(10);
    for (int i = 0; i < n; ++i) {
        cout << (a[i] - mean) / sd << '\n';
    }
    return 0;
}
