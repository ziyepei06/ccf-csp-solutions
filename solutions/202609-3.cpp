#include <bits/stdc++.h>
using namespace std;
int main(){
    int n,m;long long T;cin>>n>>m>>T;
    vector<int> slice(m),work(n);for(int &x:slice)cin>>x;
    vector<vector<bool>> prefer(n,vector<bool>(m));
    vector<deque<int>> queue(m);
    for(int i=0;i<n;i++){
        int k;cin>>work[i]>>k;
        while(k--){int c;cin>>c;prefer[i][c-1]=true;}
        queue[i%m].push_back(i);
    }
    vector<int> running(m,-1),speed(m),left(m);
    vector<long long> first(n,-1),answer(n);int finished=0,shift=0;
    // 总工作量至多 10000，逐毫秒模拟即可。
    for(long long now=0;finished<n;now++){
        // 完成 / 放回先于对应关系变换。
        for(int c=0;c<m;c++)if(running[c]!=-1&&left[c]==0){
            int i=running[c];running[c]=-1;
            if(work[i]==0){answer[i]=now-first[i];++finished;}
            else queue[(c+shift)%m].push_back(i);
        }
        if(now&&now%T==0)shift=(shift+1)%m;
        for(int c=0;c<m;c++)if(running[c]==-1){
            auto &q=queue[(c+shift)%m];if(q.empty())continue;
            int i=q.front();q.pop_front();running[c]=i;
            if(first[i]==-1)first[i]=now;
            speed[c]=prefer[i][c]?2:1;
            left[c]=min(slice[c],(work[i]+speed[c]-1)/speed[c]);
        }
        for(int c=0;c<m;c++)if(running[c]!=-1){
            work[running[c]]=max(0,work[running[c]]-speed[c]);--left[c];
        }
    }
    for(int i=0;i<n;i++)cout<<answer[i]<<(i+1==n?'\n':' ');
}
