#include <bits/stdc++.h>
using namespace std;

int main() {
    string fmt;
    if (!getline(cin, fmt)) return 0;

    bool hasArg[26] = {false};
    bool valid[26] = {false};
    for (size_t i = 0; i < fmt.size(); ++i) {
        char ch = fmt[i];
        if (ch >= 'a' && ch <= 'z') {
            int c = ch - 'a';
            valid[c] = true;
            if (i + 1 < fmt.size() && fmt[i + 1] == ':') hasArg[c] = true;
        }
    }

    int N;
    if (!(cin >> N)) return 0;
    string dummy;
    getline(cin, dummy);  // consume rest of the line

    for (int cs = 1; cs <= N; ++cs) {
        string line;
        getline(cin, line);

        vector<string> tok;
        {
            stringstream ss(line);
            string t;
            while (ss >> t) tok.push_back(t);
        }

        bool seen[26] = {false};
        bool tookArg[26] = {false};
        string val[26];

        size_t i = 1;  // tok[0] is the tool name, ignored
        while (i < tok.size()) {
            const string& t = tok[i];
            // A legal option is exactly a dash followed by one lowercase letter.
            if (t.size() != 2 || t[0] != '-' || t[1] < 'a' || t[1] > 'z') break;
            int c = t[1] - 'a';
            if (!valid[c]) break;  // not an accepted option -> stop analysis
            if (hasArg[c]) {
                if (i + 1 >= tok.size()) break;  // missing parameter -> stop
                seen[c] = true;
                tookArg[c] = true;
                val[c] = tok[i + 1];  // later occurrences overwrite earlier ones
                i += 2;
            } else {
                seen[c] = true;
                ++i;
            }
        }

        cout << "Case " << cs << ":";
        for (int c = 0; c < 26; ++c) {
            if (!seen[c]) continue;
            cout << " -" << char('a' + c);
            if (tookArg[c]) cout << " " << val[c];
        }
        cout << "\n";
    }
    return 0;
}
