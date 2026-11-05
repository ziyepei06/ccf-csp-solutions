#include <bits/stdc++.h>
using namespace std;
struct Visit {int day,user,region;};
int main() {
    int n;cin >> n;
    unordered_map<int,pair<int,int>> risk;
    deque<vector<Visit>> history;
    for (int day=0;day<n;++day) {
        int nr,nm;cin >> nr >> nm;
        while (nr--) {
            int x;cin >> x;
            auto it=risk.find(x);
            if (it==risk.end() || it->second.second<day-1) risk[x]={day,day+6};
            else it->second.second=day+6;
        }
        vector<Visit> today(nm);
        for (auto &v:today) cin >> v.day >> v.user >> v.region;
        history.push_back(move(today));if(history.size()>7) history.pop_front();
        set<int> users;
        for (auto &batch:history) for (auto &v:batch) {
            auto it=risk.find(v.region);
            if (v.day>=day-6 && it!=risk.end() && it->second.first<=v.day && it->second.second>=day)
                users.insert(v.user);
        }
        cout << day;for(int x:users) cout << ' ' << x;cout << '\n';
    }
}
