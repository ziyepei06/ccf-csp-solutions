// 202012-3 带配额的文件系统
// 目录树用「孩子名字 -> 节点」的 map 存储；每个目录维护两个和：
//   dirsum[d] = d 的直接孩子中普通文件的大小之和（用于目录配额 LD）
//   sub[d]    = d 的所有后代中普通文件的大小之和（用于后代配额 LR）
// C 指令先沿路径定位：遇到第一个不存在的分量，就说明其后的分量都要新建。
// 新建前先按 delta 检验：所有祖先的 sub+delta <= LR，父亲目录的 dirsum+delta <= LD；
// 只要有一处超限就整条指令失败且不改动；否则建目录、建/替换文件并沿父链把 delta 累加。
// R 指令删普通文件时从父目录的 dirsum 与其所有祖先的 sub 里扣掉它的大小；
// 删目录时后代总量恰好就是 sub[该目录]，从它所有祖先的 sub 里扣掉即可（目录不计入 dirsum）。
// Q 指令只改该目录自身的配额，只需检验该目录的 dirsum <= LD 且 sub <= LR。
// 总和最大约 1e5 * 1e18 = 1e23，用 __int128 存和与配额，避免溢出。
#include <bits/stdc++.h>
using namespace std;

typedef __int128 i128;

struct Node {
    map<string, Node *> ch;
    Node *parent;
    i128 sub;     // 后代普通文件大小之和
    i128 dirsum;  // 直接孩子里普通文件大小之和
    i128 lq;      // 目录配额 LD，0 表示不限
    i128 rq;      // 后代配额 LR，0 表示不限
    i128 size;    // 普通文件自身大小
    bool isfile;
    Node()
        : parent(nullptr), sub(0), dirsum(0), lq(0), rq(0), size(0), isfile(false) {}
};

static void destroyNode(Node *u) {
    for (auto &kv : u->ch) destroyNode(kv.second);
    u->ch.clear();
    delete u;
}

static void splitPath(const string &path, vector<string> &comps) {
    comps.clear();
    size_t i = 0, n = path.size();
    while (i < n) {
        if (path[i] == '/') {
            ++i;
            continue;
        }
        size_t j = i;
        while (j < n && path[j] != '/') ++j;
        comps.push_back(path.substr(i, j - i));
        i = j;
    }
}

static void addSubUp(Node *p, i128 d) {
    for (Node *a = p; a; a = a->parent) a->sub += d;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;
    string line;
    getline(cin, line);

    Node *root = new Node();
    vector<string> comps;

    for (int qi = 0; qi < n; ++qi) {
        if (!getline(cin, line)) {
            cout << "N\n";
            continue;
        }
        if (!line.empty() && line.back() == '\r') line.pop_back();
        istringstream iss(line);
        string op, path;
        iss >> op >> path;
        splitPath(path, comps);
        int k = (int)comps.size();

        if (op == "C") {
            long long sz;
            iss >> sz;
            if (k == 0) {  // 题面保证不会出现，兜底
                cout << "N\n";
                continue;
            }
            Node *cur = root;
            Node *existingFile = nullptr;
            bool bad = false;
            int createdFrom = k;  // 从这里开始的分量都不存在
            for (int j = 0; j < k; ++j) {
                auto it = cur->ch.find(comps[j]);
                if (it == cur->ch.end()) {
                    createdFrom = j;
                    break;
                }
                Node *c = it->second;
                if (j == k - 1) {
                    if (c->isfile)
                        existingFile = c;
                    else
                        bad = true;  // 目标是目录，不能覆盖
                } else {
                    if (c->isfile) {  // 中间分量是普通文件，无法继续
                        bad = true;
                        break;
                    }
                    cur = c;
                }
            }
            if (bad) {
                cout << "N\n";
                continue;
            }

            i128 delta = existingFile ? ((i128)sz - existingFile->size) : (i128)sz;

            bool ok = true;
            for (Node *a = cur; a; a = a->parent) {
                if (a->rq != 0 && a->sub + delta > a->rq) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                // 需要新建的中间目录没有配额，可忽略；真正的父亲目录才要查 LD
                Node *fp;
                if (existingFile)
                    fp = existingFile->parent;
                else if (createdFrom == k - 1)
                    fp = cur;
                else
                    fp = nullptr;  // 父亲是新目录，lq == 0
                if (fp && fp->lq != 0 && fp->dirsum + delta > fp->lq) ok = false;
            }
            if (!ok) {
                cout << "N\n";
                continue;
            }

            if (existingFile) {
                existingFile->size = (i128)sz;
                Node *fp = existingFile->parent;
                fp->dirsum += delta;
                addSubUp(fp, delta);
            } else {
                Node *p = cur;
                for (int j = createdFrom; j + 1 < k; ++j) {
                    Node *nd = new Node();
                    nd->parent = p;
                    p->ch[comps[j]] = nd;
                    p = nd;
                }
                Node *f = new Node();
                f->parent = p;
                f->isfile = true;
                f->size = (i128)sz;
                p->ch[comps[k - 1]] = f;
                p->dirsum += (i128)sz;
                addSubUp(p, (i128)sz);
            }
            cout << "Y\n";
        } else if (op == "R") {
            if (k == 0) {
                cout << "Y\n";
                continue;
            }
            Node *cur = root;
            Node *target = nullptr;
            for (int j = 0; j < k; ++j) {
                auto it = cur->ch.find(comps[j]);
                if (it == cur->ch.end()) {
                    target = nullptr;
                    break;
                }
                Node *c = it->second;
                if (j == k - 1) {
                    target = c;
                } else {
                    if (c->isfile) {
                        target = nullptr;
                        break;
                    }
                    cur = c;
                }
            }
            if (!target) {
                cout << "Y\n";  // 不存在也不做任何操作，仍算成功
                continue;
            }
            Node *p = target->parent;
            p->ch.erase(comps[k - 1]);
            if (target->isfile) {
                i128 T = target->size;
                p->dirsum -= T;
                addSubUp(p, -T);
            } else {
                i128 T = target->sub;  // 整个子树里普通文件的总量
                addSubUp(p, -T);
            }
            destroyNode(target);
            cout << "Y\n";
        } else {  // Q
            long long LD, LR;
            iss >> LD >> LR;
            Node *cur = root;
            bool found = true;
            for (int j = 0; j < k; ++j) {
                auto it = cur->ch.find(comps[j]);
                if (it == cur->ch.end()) {
                    found = false;
                    break;
                }
                Node *c = it->second;
                if (j == k - 1) {
                    cur = c;
                } else {
                    if (c->isfile) {
                        found = false;
                        break;
                    }
                    cur = c;
                }
            }
            if (!found || cur->isfile) {
                cout << "N\n";
                continue;
            }
            if ((LD == 0 || cur->dirsum <= (i128)LD) &&
                (LR == 0 || cur->sub <= (i128)LR)) {
                cur->lq = (i128)LD;
                cur->rq = (i128)LR;
                cout << "Y\n";
            } else {
                cout << "N\n";
            }
        }
    }

    destroyNode(root);
    return 0;
}
