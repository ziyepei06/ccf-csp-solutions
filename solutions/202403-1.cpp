#include <bits/stdc++.h>
using namespace std;

int main() {
    int n, m;
    if (scanf("%d %d", &n, &m) != 2) return 0;

    vector<int> docCnt(m + 1, 0);   // 出现在多少篇文章中
    vector<int> totalCnt(m + 1, 0); // 总共出现次数
    vector<int> stamp(m + 1, 0);    // 标记：第 i 篇文章是否已统计过该单词
    vector<int> words;
    words.reserve(128);

    for (int i = 1; i <= n; ++i) {
        int l;
        if (scanf("%d", &l) != 1) break;
        words.clear();
        for (int j = 0; j < l; ++j) {
            int w;
            scanf("%d", &w);
            words.push_back(w);
            ++totalCnt[w];
            if (stamp[w] != i) {   // 本篇文章中第一次出现
                stamp[w] = i;
                ++docCnt[w];
            }
        }
    }

    string out;
    out.reserve(m * 12);
    for (int i = 1; i <= m; ++i) {
        out += to_string(docCnt[i]);
        out += ' ';
        out += to_string(totalCnt[i]);
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
