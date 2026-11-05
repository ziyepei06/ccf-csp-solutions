// Source: https://github.com/qzylalala/CSP/blob/master/201909/2.cpp
// License: Apache-2.0 (see licenses/qzylalala-CSP-Apache-2.0.txt)
// Changes: attribution header; local sample verification.
#include <iostream>
#include <cstring>
#include <algorithm>

using namespace std;

const int N = 1010;
int n;
int a[N];
bool st[N];

int main() {
    cin >> n;

    for (int i = 1; i <= n; i++) {
        int m;
        cin >> m;
        for (int j = 1; j <= m; j++) {
            int val;
            cin >> val;

            if (j == 1) {
                a[i] = val;
                continue;
            }

            if (val > 0) {
                if (a[i] != val) st[i] = true;
                a[i] = val;
            }
            else {
                a[i] += val;
            }
        }
    }

    int t = 0, d = 0, e = 0;
    for (int i = 1; i <= n; i ++) {
        t += a[i];
        if (st[i]) d ++;

        if (i <= n - 2 && st[i] && st[i + 1] && st[i + 2]) e ++;
        if (i == n - 1 && st[i] && st[n] && st[1]) e ++;
        if (i == n && st[n] && st[1] && st[2]) e ++;
    }

    printf("%d %d %d\n", t, d, e);

    return 0;
}