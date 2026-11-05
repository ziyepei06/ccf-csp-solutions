#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    while (n--) {
        string s;
        cin >> s;

        int cnt[256] = {0};
        bool hasLetter = false, hasDigit = false, hasSpecial = false;

        for (char ch : s) {
            cnt[(unsigned char)ch]++;
            if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
                hasLetter = true;
            else if (ch >= '0' && ch <= '9')
                hasDigit = true;
            else // '*' 或 '#'
                hasSpecial = true;
        }

        bool longEnough = (s.size() >= 6);
        bool allKinds = hasLetter && hasDigit && hasSpecial;

        if (!longEnough || !allKinds) {
            cout << 0 << '\n';
            continue;
        }

        bool distinctEnough = true;
        for (int i = 0; i < 256; ++i)
            if (cnt[i] > 2) { distinctEnough = false; break; }

        // 长度、字符种类均达标：任一字符超过 2 次则只能评为中，否则为高
        cout << (distinctEnough ? 2 : 1) << '\n';
    }

    return 0;
}
