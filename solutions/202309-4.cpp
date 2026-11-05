#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef pair<ll, ll> pll;

int main() {
    int n, m, p, q;
    if (scanf("%d %d %d %d", &n, &m, &p, &q) != 4) return 0;

    vector<ll> ex(p), ey(p);
    set<pll> A, B, C, D;  // A:(y,x) B:(x,y) C:(x-y,x) D:(x+y,x)
    unordered_map<ll, int> pos;
    pos.reserve((size_t)p * 2 + 10);
    pos.max_load_factor(0.7f);
    const ll MODK = 2000000001LL;
    auto enc = [&](ll x, ll y) -> ll { return x * MODK + y; };

    for (int i = 0; i < p; i++) {
        ll x, y;
        if (scanf("%lld %lld", &x, &y) != 2) return 0;
        ex[i] = x;
        ey[i] = y;
        A.insert(pll(y, x));
        B.insert(pll(x, y));
        C.insert(pll(x - y, x));
        D.insert(pll(x + y, x));
        pos[enc(x, y)] = i;
    }

    static const int dx[8] = {1, 1, 0, -1, -1, -1, 0, 1};
    static const int dy[8] = {0, 1, 1, 1, 0, -1, -1, -1};

    for (int qi = 0; qi < q; qi++) {
        ll u, v, t;
        if (scanf("%lld %lld %lld", &u, &v, &t) != 3) return 0;
        ll best = LLONG_MAX;

        // 方向0 (1,0)：同一 y，x 向右最近
        {
            auto it = A.lower_bound(pll(v, u + 1));
            if (it != A.end() && it->first == v) best = min(best, it->second - u);
        }
        // 方向1 (1,1)：x-y 相同，x 向右最近
        {
            ll key = u - v;
            auto it = C.lower_bound(pll(key, u + 1));
            if (it != C.end() && it->first == key) best = min(best, it->second - u);
        }
        // 方向2 (0,1)：同一 x，y 向上最近
        {
            auto it = B.lower_bound(pll(u, v + 1));
            if (it != B.end() && it->first == u) best = min(best, it->second - v);
        }
        // 方向3 (-1,1)：x+y 相同，x 向左最近
        {
            ll key = u + v;
            auto it = D.lower_bound(pll(key, u));
            if (it != D.begin()) {
                --it;
                if (it->first == key) best = min(best, u - it->second);
            }
        }
        // 方向4 (-1,0)：同一 y，x 向左最近
        {
            auto it = A.lower_bound(pll(v, u));
            if (it != A.begin()) {
                --it;
                if (it->first == v) best = min(best, u - it->second);
            }
        }
        // 方向5 (-1,-1)：x-y 相同，x 向左最近
        {
            ll key = u - v;
            auto it = C.lower_bound(pll(key, u));
            if (it != C.begin()) {
                --it;
                if (it->first == key) best = min(best, u - it->second);
            }
        }
        // 方向6 (0,-1)：同一 x，y 向下最近
        {
            auto it = B.lower_bound(pll(u, v));
            if (it != B.begin()) {
                --it;
                if (it->first == u) best = min(best, v - it->second);
            }
        }
        // 方向7 (1,-1)：x+y 相同，x 向右最近
        {
            ll key = u + v;
            auto it = D.lower_bound(pll(key, u + 1));
            if (it != D.end() && it->first == key) best = min(best, it->second - u);
        }

        // K2 = {1..k2}，k2 为八方向同时不越界的最远距离
        ll k2 = min(min(n - u, u - 1), min(m - v, v - 1));
        ll k = (best <= k2) ? best : 0;

        if (k > 0) {
            struct Mv { int id; ll nx, ny; };
            Mv mv[8];
            int cnt = 0;
            for (int i = 0; i < 8; i++) {
                ll sx = u + k * dx[i], sy = v + k * dy[i];
                auto it = pos.find(enc(sx, sy));
                if (it == pos.end()) continue;
                int id = it->second;
                int j = (i + t) % 8;
                ll tx = u + k * dx[j], ty = v + k * dy[j];
                mv[cnt++] = Mv{id, tx, ty};
            }
            // 先全部删除旧位置，再统一插入新位置（八元轮换无冲突）
            for (int i = 0; i < cnt; i++) {
                ll ox = ex[mv[i].id], oy = ey[mv[i].id];
                A.erase(pll(oy, ox));
                B.erase(pll(ox, oy));
                C.erase(pll(ox - oy, ox));
                D.erase(pll(ox + oy, ox));
                pos.erase(enc(ox, oy));
            }
            for (int i = 0; i < cnt; i++) {
                int id = mv[i].id;
                ll nx = mv[i].nx, ny = mv[i].ny;
                ex[id] = nx;
                ey[id] = ny;
                A.insert(pll(ny, nx));
                B.insert(pll(nx, ny));
                C.insert(pll(nx - ny, nx));
                D.insert(pll(nx + ny, nx));
                pos[enc(nx, ny)] = id;
            }
        }
    }

    ll ans = 0;
    for (int i = 0; i < p; i++) ans ^= (ll)(i + 1) * ex[i] + ey[i];
    printf("%lld\n", ans);
    return 0;
}
