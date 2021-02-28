#include<bits/stdc++.h>
#include <sys/timeb.h>
#include <time.h>
#include<windows.h>
using namespace std;
struct huo {
	int ID;
	int inTim, outTim, nowAt; //存放入泊,驶离时间
	priority_queue<int, vector<int>, greater<int> > q;  // 小顶堆
	huo() {
		inTim = 0, outTim = 0, nowAt = 0;
	}
};

int clk = 0;//模拟真实的时间，也是最终的结果
int cnt = 0;
vector<huo> boWei;//每次的吊车都去吊最早到的那搜船的，然后吊完了货物到了缓冲区，就弹出队首的，
vector<huo> readyOut;
vector<huo> ship(4);//表示有多少船还在等待入泊
int diao[3] = { 0 }, car[3] = { 0 }, wait = 0;//diao是吊车记录时间，car是小车也是记录的时间,wait记录的是缓冲区的等待的数量
int shiCha = 0;
bool b[3] = { false };//标记是否有船正在泊入
huo tb[3];//当前船的入泊时间
priority_queue<int, vector<int>, greater<int> > qiDiao;//贪心策略:把两艘船的货物排序，每次不是吊最早到的那个，可以吊后来到的那个的
void input() {//初始化的输入
	for (int i = 0; i < 4; i++) {
		cin >> ship[i].inTim >> ship[i].outTim;
		ship[i].ID = i;
		int totItem;//每个船带的货物的个数
		cin >> totItem;
		for (int j = 0; j < totItem; j++) {
			int t;
			cin >> t;
			ship[i].q.push(t);
		}
	}
}
char*   log_Time(void)
{
	struct  tm      *ptm;
	struct  timeb   stTimeb;
	static  char    szTime[19];

	ftime(&stTimeb);
	ptm = localtime(&stTimeb.time);
	sprintf(szTime, "%02d-%02d %02d:%02d:%02d.%03d",
		ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, stTimeb.millitm);
	szTime[18] = 0;
	return szTime;
}
bool cmp(huo a, huo b) {
	return (a.inTim + a.outTim) > (b.inTim + b.outTim);//从大到小的排，从尾部弹出
}
int main()
{
	input();
	cout << "Algorithm start time:" << log_Time() << endl;
	sort(ship.begin(), ship.end(), cmp);

	while (++clk) {//这个是表示1s之后的结果，模拟真的时钟
		//停船部分
		for (int i = 1; i <= 2; i++) {//开始遍历每一个泊位，在哪个泊位没有关系主要是数目
			huo tmp;
			if (!b[i]) {//如果泊位为空，且没有船去停泊
				b[i] = true;
				if (!ship.empty()) {
					tmp = *(ship.end() - 1);//贪心策略：取入泊时间加驶离时间最短的，先处理
					tmp.nowAt = i;//该船所在的泊位，出泊用的
					tmp.inTim--;
					tb[i] = tmp;
					ship.pop_back();//该船从等待的队伍弹出
				}
			}
			else if (tb[i].inTim) {
				tb[i].inTim--;
				if (!tb[i].inTim) {
					boWei.push_back(tb[i]);//进入了泊位
				}
			}
		}
		for (int i = 0; i < boWei.size(); i++) {//把来的货物都用这个qiDiao的小根堆来维护
			while (!boWei[i].q.empty()) {
				qiDiao.push(boWei[i].q.top());
				boWei[i].q.pop();
			}
		}
		//起吊部分
		for (int i = 1; i <= 2; i++) {//遍历吊车
			if (!diao[i] && !qiDiao.empty()) {
				diao[i] = qiDiao.top();
				qiDiao.pop();
				continue;
			}
			if(diao[i]) {
				diao[i]--;
				if (!diao[1] && !diao[2] && qiDiao.empty()) {
					for (int i = 0; i < 2; i++) {
						readyOut.push_back(boWei[i]);//就两个一起走
					}
					boWei.clear();
				}
				if (!diao[i])wait++;
			}
		}
		
		//驶离部分
		for (int i = 0; i < readyOut.size(); i++) {
			if (readyOut[i].outTim) {
				readyOut[i].outTim--;
			}//上面减到零之后立即执行，下面的
			if (!readyOut[i].outTim) {
				cnt++;
				b[readyOut[i].nowAt] = false;//把该泊位设置成空闲状态，可以驶入
				readyOut.erase(readyOut.begin()+i);
			}
		}
		if (cnt == 4 && !car[1] && !car[2] && !wait)break;
		//小车部分
		for (int i = 1; i <= 2; i++) {
			if (car[i]) {
				car[i]--;
			}
			if (!car[i] && wait) {//小车在缓冲区且有货物在等待运输
				car[i] = 40;
				wait--;//货物上车了
			}
		}
	}
	cout << clk << endl;
	cout << "Algorithm end time:" << log_Time() << endl;
}