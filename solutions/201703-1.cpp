// Source: https://github.com/qzylalala/CSP/blob/master/201703/1.cpp
// License: Apache-2.0 (see licenses/qzylalala-CSP-Apache-2.0.txt)
// Changes: attribution header; local sample verification.
#include <iostream>
#include <cstring>
#include <algorithm>

using namespace std;

int a[1010];
int n, k;

int main() {
    cin >> n >> k;
    for (int i = 1; i <= n; i++) cin >> a[i];

    int ans = 0;

    int sum = 0;
    for (int i = 1; i <= n; i++) {
        sum += a[i];
        if (sum >= k) {
            ans ++;
            sum = 0;
        }
        else {
            if (i == n) ans ++;
        }
    }

    cout << ans << endl;
    return 0;
}