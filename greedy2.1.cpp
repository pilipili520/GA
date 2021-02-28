#include<bits/stdc++.h>
#include <sys/timeb.h>
#include <time.h>
#include<windows.h>
using namespace std;
struct ite {
	int ID;
	int seq;
	int des;
	int timecost;
	bool operator < (const ite &I) const {
		//按照seq由小到大排列
		return I.timecost < timecost;
	}
};
struct huo {
	int ID;
	int inTim,outTim,nowAt; //存放入泊,驶离时间
	priority_queue<ite> q;  // 小顶堆
	huo(){
		inTim = 0,outTim = 0,nowAt = 0;
	}
};

struct diaoCar {
	int tim;
	int des;
	int fromWhere;
	diaoCar() {
		tim = 0; des = 0;
	}
};
int clk = 0;//模拟真实的时间，也是最终的结果
int cnt = 0;
queue<huo> boWei;//每次的吊车都去吊最早到的那搜船的，然后吊完了货物到了缓冲区，就弹出队首的，
vector<huo> readyOut;
vector<huo> ship(4);//表示有多少船还在等待入泊
int  car[3] = { 0 }, wait=0;//diao是吊车记录时间，car是小车也是记录的时间,wait记录的是缓冲区的等待的数量
diaoCar diao[3];
int shiCha = 0;
bool b[3] = { false };//标记是否有船正在泊入
huo tb[3];//当前船的入泊时间
void input(){//初始化的输入
	for (int i = 0; i < 4; i++) {
		cin >> ship[i].inTim >> ship[i].outTim;
		ship[i].ID = i;
		int totItem;//每个船带的货物的个数
		cin >> totItem;
		for (int j = 0; j < totItem; j++) {
			int t,des;
			cin >> t>>des;
			ite tmp;
			tmp.des = des;
			tmp.timecost = t;
			ship[i].q.push(tmp);
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
int de = 0;
int main()
{
	input();
	cout << "Algorithm start time:" << log_Time() << endl;
	sort(ship.begin(), ship.end(), cmp);
	queue<int> waitForCar;//等待小车运送的货物，就按先来后到
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
			else if(tb[i].inTim){
				tb[i].inTim--;
				if (!tb[i].inTim) {
					boWei.push(tb[i]);//进入了泊位
				}
			}
		}
		//起吊部分
		
		bool isAllOut = true;
		for (int i = 1; i <= 2; i++) {//遍历吊车
			if (!boWei.empty()) {//泊位空了的时候可能还有货物
				isAllOut = false;
				if (!diao[i].tim) {//当前吊车空闲
					//贪心策略：吊车去看每个泊位的情况，总是从第一个泊位看起，那个是最早到的船，泊位是一个queue
					huo tmp = boWei.front();
					if (!tmp.q.empty()) {
						diao[i].tim = tmp.q.top().timecost;
						diao[i].des = tmp.q.top().des;
						diao[i].fromWhere = tmp.nowAt;
						boWei.front().q.pop();//这个货物从船上下来了
					}
				}
				else {
					diao[i].tim--;
					if (!diao[i].tim) {//当前的货物已经到缓冲区了
						//wait++;
						waitForCar.push(diao[i].des);
						if (boWei.front().q.empty()) {//当前的船的所有的货物都到了缓冲区了，
							for (int j = 1; j <= 2; j++) {//可能有当前的那个的diao是零了，而且货都从船上下来了，但是有一个货还在吊车上没有进入缓冲区
								if (diao[j].tim&&diao[j].fromWhere == boWei.front().nowAt)boWei.front().outTim += (diao[j].tim);//把这个时间变成，吊车也在吊，但是船已经出港，把船出港的时间变长，后面会多减1
							}
							readyOut.insert(readyOut.begin(),boWei.front());//新的都放在最开始
							boWei.pop();//这里先弹出去但是，还没有驶离，方便吊车去遍历，但是不把这个船放入驶离队列里
						}
					}
				}
			}
			if (isAllOut&&diao[i].tim) {
				diao[i].tim--;//两艘船同时不能出新货的情况，但是吊车上面吊着东西呢
				if (!diao[i].tim) {
					waitForCar.push(diao[i].des);//debug++; 
				}
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
				readyOut.pop_back();
			}
		}
		
		//小车部分
		for (int i = 1; i <= 2; i++) {
			if (car[i]) {
				car[i]--;
			}
			if (!car[i] && !waitForCar.empty()) {//小车在缓冲区且有货物在等待运输
				car[i] = waitForCar.front();
				de++;
				waitForCar.pop();//货物上车了
				//wait--;//货物上车了
			}
		}
		if (cnt == 4 && !car[1] && !car[2] && waitForCar.empty())break;
	}
	cout << clk << endl;
	cout << de << endl;
	cout << "Algorithm end time:" << log_Time() << endl;
}