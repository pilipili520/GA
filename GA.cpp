#include<bits/stdc++.h>
#include <sys/timeb.h>
#include <time.h>
#include<windows.h>

using namespace std;
struct unit {//一条染色体，一个个体
	int ship_seq[4],diao_seq[19],uans;
	double pro;
	unit(){
		memset(ship_seq, 0, sizeof(ship_seq));
		memset(diao_seq, 0, sizeof(diao_seq));//map<货物编号,货物的入场顺序>
		uans = 0; pro = 0.0;
	}
};
struct ite {
	int ID;
	int seq;
	bool operator < (const ite &I) const{
		//按照seq由小到大排列
		return I.seq < seq;
	}
};
struct huo {
	int ID;
	int inTim, outTim, nowAt,nitem; //存放入泊,驶离时间
	priority_queue<ite> q;  // 小顶堆，按照seq的顺序排好序的
	huo() {
		inTim = 0, outTim = 0, nowAt = 0,nitem = 0;
	}
};

char*   log_Time(void)//获得系统的时间精确到毫秒级
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


vector<huo> ship(4);//表示有多少船还在等待入泊
int timetable[19];//存放的是每个货物对应的时间

void input() {//初始化的输入
	int cot = -1;
	for (int i = 0; i < 4; i++) {
		cin >> ship[i].inTim >> ship[i].outTim;
		ship[i].ID = i;
		int totItem;//每个船带的货物的个数
		cin >> totItem;
		ship[i].nitem = totItem;
		for (int j = 0; j < totItem; j++) {
			cin >> timetable[++cot];
		}
	}
}
bool cmp(unit a, unit b) {
	return (a.pro + a.pro) < (b.pro + b.pro);//从小到大的排，从尾部弹出
}
int ran=88;
void cal(unit& s) {
	int de = 0;
	//下面这些变量都是一次性的
	/************************************************/
	int clk = 0;//模拟真实的时间，也是最终的结果
	int cnt = 0;
	queue<huo> boWei;//每次的吊车都去吊最早到的那搜船的，然后吊完了货物到了缓冲区，就弹出队首的，
	vector<huo> readyOut;
	int diao[3] = { 0,0,0 }, car[3] = { 0,0,0 }, wait = 0;//diao是吊车记录时间，car是小车也是记录的时间,wait记录的是缓冲区的等待的数量
	int shiCha = 0;
	bool b[3] = { false,false,false };//标记是否有船正在泊入
	huo tb[3];//当前船的入泊时间
	vector<huo> tship = ship;//现在还是原汁原味的ship
	/************************************************/

	//处理一下当前的那个ship,把s中的顺序放到这个新的tship中
	int markIte = -1;//给ite赋值的临时变量
	for (int i = 0; i < tship.size(); i++) {
		for (int j = 0; j < tship[i].nitem; j++) {
			ite t;
			t.ID = ++markIte;
			t.seq = s.diao_seq[markIte];
			tship[i].q.push(t);
		}
	}
	int cnt_ship = -1;
	
	while (++clk) {//这个是表示1s之后的结果，模拟真的时钟
		//停船部分
		for (int i = 1; i <= 2; i++) {//开始遍历每一个泊位，在哪个泊位没有关系主要是数目
			huo tmp;
			if (!b[i]) {//如果泊位为空，且没有船去停泊
				b[i] = true;
				if (cnt_ship < 3) {
					tmp = tship[s.ship_seq[++cnt_ship]];//按照s顺序来取
					tmp.nowAt = i;//该船所在的泊位，出泊用的
					tmp.inTim--;
					tb[i] = tmp;
				}
			}
			else if (tb[i].inTim) {
				tb[i].inTim--;
				if (!tb[i].inTim) {
					boWei.push(tb[i]);//进入了泊位
				}
			}
		}
		//起吊部分
		bool isAllOut = true;
		for (int i = 1; i <= 2; i++) {//遍历吊车
			if (!boWei.empty()) {//
				isAllOut = false;
				if (!diao[i]) {//当前吊车空闲
					//局部搜索:吊车去看每个泊位的情况，总是从第一个泊位看起，那个是最早到的船，泊位是一个queue
					huo tmp = boWei.front();
					if (!tmp.q.empty()) {
						diao[i] = timetable[tmp.q.top().ID];
						boWei.front().q.pop();//这个货物从船上下来了
						
					}
				}
				else {
					diao[i]--;
					if (!diao[i]) {//当前的货物已经到缓冲区了
						wait++;
						if (boWei.front().q.empty()) {//当前的船的所有的货物都到了缓冲区了，
							for (int j = 1; j <= 2; j++) {//可能有当前的那个的diao是零了，而且货都从船上下来了，但是有一个货还在吊车上没有进入缓冲区
								if (diao[j])boWei.front().outTim += (diao[j]);//把这个时间变成，吊车也在吊，但是船已经出港，把船出港的时间变长，后面会多减1
							}
							readyOut.insert(readyOut.begin(), boWei.front());//新的都放在最开始
							boWei.pop();//这里先弹出去但是，还没有驶离，方便吊车去遍历，但是不把这个船放入驶离队列里
						}
					}
				}
			}
			if (isAllOut&&diao[i]) {
				diao[i]--;//两艘船同时不能出新货的情况，但是吊车上面吊着东西呢
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
				readyOut.pop_back();
			}
		}
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
		if (cnt == 4 && !car[1] && !car[2] && !wait)break;
	}
	s.uans = clk;
}
void fill(unit& s) {
	
	bool mark[20];//没有被选过的是true
	memset(mark, true, sizeof(mark));
	for (int i = 0; i < 4; i++) {
		int t ;
		while (true) {
			srand(ran++);
			s.ship_seq[i] = t = rand() % 4;
			if (mark[t]) { mark[t] = false; break; }
		}
	}
	memset(mark, true, sizeof(mark));
	for (int i = 0; i < 19; i++) {
		int t;
		while (true) {
			srand(ran++);
			s.diao_seq[i] = t = rand() % 19;
			if (mark[t]) { mark[t] = false; break; }
		}
	}
}
//void print(unit tmp) {//打印当前的顺序
//	for (int j = 0; j < 4; j++)log << tmp.ship_seq[j] << ' ';
//	log << endl;
//	for (int j = 0; j < 19; j++)log << tmp.diao_seq[j] << ' ';
//	log << endl;
//
//}

void Xover(unit &a,unit &b) {//交叉完了后顺便修复

	int fixa[20], fixb[20];
	//先搞船
	memset(fixa, -1, sizeof(fixa));
	memset(fixb, -1, sizeof(fixb));
	int pos;
	srand(ran++);
	pos = rand() % 4 + 1;//前pos分量交换
	for (int i = 0; i < pos; i++) {
		fixb[a.ship_seq[i]] = b.ship_seq[i];
		fixa[b.ship_seq[i]] = a.ship_seq[i];
		swap(a.ship_seq[i], b.ship_seq[i]);
	}
	for (int i = pos; i < 4; i++) {
		if (fixa[a.ship_seq[i]] != -1) {
			a.ship_seq[i] = fixa[a.ship_seq[i]];
		}
		if (fixa[b.ship_seq[i]] != -1) {
			b.ship_seq[i] = fixa[b.ship_seq[i]];
		}
	}
	//再搞货
	memset(fixa, -1, sizeof(fixa));
	memset(fixb, -1, sizeof(fixb));
	srand(ran++);
	pos = rand() % 19 + 1;//前pos分量交换
	for (int i = 0; i < pos; i++) {
		fixb[a.diao_seq[i]] = b.diao_seq[i];
		fixa[b.diao_seq[i]] = a.diao_seq[i];
		swap(a.diao_seq[i], b.diao_seq[i]);
	}
	for (int i = pos; i < 19; i++) {
		if (fixa[a.diao_seq[i]] != -1) {
			a.diao_seq[i] = fixa[a.diao_seq[i]];
		}
		if (fixa[b.diao_seq[i]] != -1) {
			b.diao_seq[i] = fixa[b.diao_seq[i]];
		}
	}
}

int main() {
	ofstream log("log.txt", ios::app);
	input();
	cout << "Algorithm start time:" << log_Time() << endl;
	log << "Algorithm start time:" << log_Time() << endl;
	const int pop_size = 25; //定义种群大小 
	
	double sum; //适值总和 
	double avl_sum; //适度平均值 
	double p_probability[pop_size]; //适值概率 
	double pp[pop_size];
	double pro; //定义随机生成的概率 
	double pc = 0.90; //定义交叉的概率 
	double pm = 0.05; //定义变异的概率 
	int NG =5500; //指定种群最大的繁殖的代数 
	int t = 0; //当前繁殖的代数 
	unit p[pop_size]; //定义种群 
	unit q[pop_size]; //定义繁殖种群 即种群的下一代 

	log << "初始的种群 "<<endl;
	for (int i = 0; i < pop_size; i++) { //生成初始的第0代种群 
		fill(p[i]);
		//print(p[i]);
	}
	while (t <= NG) {
		log << "繁殖的代数：t=" << t << endl;
		sum = 0.0;
		//for (int i = 0; i < pop_size; i++){
		//	q[i] = p[i];
		//	//print(q[i]);
		//	for (int j = 0; j < 4; j++)log << q[i].ship_seq[j] << ' ';
		//	log << endl;
		//	for (int j = 0; j < 19; j++)log << q[i].diao_seq[j] << ' ';
		//	log << endl;
		//}
		log << endl;
		for (int i = 0; i < pop_size; i++) { //计算sum
			cal(p[i]);
			sum += p[i].uans;
		}
		avl_sum = sum / pop_size;
		log << "sum=" << sum << endl;
		log << "适度平均值=" << avl_sum << endl;
		for (int i = 0; i < pop_size; i++) {
			p[i].pro = p[i].uans / sum;
		}
		sort(p, p + pop_size, cmp);//大小的概率互换
		int le = 0, ri = pop_size - 1;
		while (le <= ri) {
			swap(p[le].pro, p[ri].pro);
			le++; ri--;
		}
		for (int i = 0; i < pop_size; i++) {
			log <<"ans:"<<p[i].uans<<"pro:" <<p[i].pro << " "<<endl;
		}log << endl;
		for (int i = 0; i < pop_size; i++) {
			q[i] = p[i];
			//print(q[i]);
			for (int j = 0; j < 4; j++)log << q[i].ship_seq[j] << ' ';
			log << endl;
			for (int j = 0; j < 19; j++)log << q[i].diao_seq[j] << ' ';
			log << endl;
		}
		for (int i = 0; i < pop_size; i++) //计算适值概率 
		{
			p_probability[i] =p[i].pro;//要保留小的
			if (i == 0) {
				pp[i] = p_probability[i];
				log << "pp" << i << "=" << pp[i] << endl;
			}
			else {
				pp[i] = p_probability[i] + pp[i - 1];
				log << "pp" << i << "=" << pp[i] << endl;
			}
			//log<<"p_probability"<<i<<"="<<p_probability[i]<<endl; 
		}
		//选择双亲 
		for (int i = 0; i < pop_size; i++) {
			srand(ran++);
			pro = rand() % 1000 / 1000.0;
			if (pro >= pp[0] && pro < pp[1])
				p[i] = q[0];
			else if (pro >= pp[1] && pro < pp[2])
				p[i] = q[1];
			else if (pro >= pp[2] && pro < pp[3])
				p[i] = q[2];
			else if (pro >= pp[3] && pro < pp[4])
				p[i] = q[3];
			else if (pro >= pp[4] && pro < pp[5])
				p[i] = q[4];
			else
				p[i] = q[5];
		}

		//杂交算子 
		int r = 0;
		int z = 0;
		for (int j = 0; j < pop_size; j++) {
			srand(ran++);
			pro = rand() % 1000 / 1000.0;
			if (pro < pc) {
				++z;
				if (z % 2 == 0)
					Xover(p[r], p[j]);
				else
					r = j;
			}
		}
		//变异算子 
		bool check = false;
		for (int i = 0; i < pop_size; i++) {
			for (int j = 0; j < 4; j++) {//船的顺序先变异
				srand(ran++);
				pro = rand() % 1000 / 1000.0; //在【0,1】区间产生随机数 
				if (pro < pm) {
					int pos;
					log << check << endl;
					srand(ran++);
					pos = rand() % 4;//pos分量交换
					swap(p[i].ship_seq[j], p[i].ship_seq[pos]);
				}
			}
			for (int j = 0; j < 19; j++) {//货物顺的变异
				srand(ran++);
				pro = rand() % 1000 / 1000.0; //在【0,1】区间产生随机数 
				if (pro < pm) {
					int pos;
					srand(ran++);
					pos = rand() % 19;//pos分量交换
					swap(p[i].diao_seq[j], p[i].diao_seq[pos]);
				}
			}
		}
		t++;
		log << endl;
	}
	log << "最终结果："<< avl_sum<<endl;
	for (int i(0); i < pop_size; i++) //算法结束，输出结果 
	{
		//print(p[i]);
	}
	log << endl;
	cout << "Algorithm end time:" << log_Time() << endl;
	log << "Algorithm end time:" << log_Time() << endl;
	return 0;
	
}