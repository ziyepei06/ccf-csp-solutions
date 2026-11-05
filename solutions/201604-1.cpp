// Source: https://github.com/qzylalala/CSP/blob/master/201604/1.cpp
// License: Apache-2.0 (see licenses/qzylalala-CSP-Apache-2.0.txt)
// Changes: attribution header; local sample verification.
#include <iostream>
#include <cstring>
#include <algorithm>

using namespace std;

const int N = 1010;
int a[N];
int n;

int main() {
    cin >> n;
    for (int i = 0; i < n; i++ ) cin >> a[i];

    int ans = 0;
    for (int i = 1; i < n - 1; i++) {
        if (a[i] - a[i - 1] < 0 && a[i] - a[i + 1] < 0) ans ++;
        else if (a[i] - a[i - 1] > 0 && a[i] - a[i + 1] > 0) ans ++;
    }

    cout << ans << endl;

    return 0;
}