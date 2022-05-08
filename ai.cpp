#include <iostream>
#include <cstring>
#include <queue>
#include <vector>
#include <algorithm>
#include <Windows.h>

using namespace std;

struct Node
{
	int m;
	int c;
	int b;		 // A船狀態，1:左，0:右
	int B;		 // B船狀態，1:左，0:右
	int b_count; // A船次數
	int B_count; // B船次數
	int step;
	double f_loss;
	Node *parent;
	Node(int m_num, int c_num, int A_boat_state, int B_boat_state, int bcount, int Bcount, int step_count, Node *ptr) : m{m_num}, c{c_num}, b{A_boat_state}, B{B_boat_state}, b_count{bcount}, B_count{Bcount}, step{step_count}, parent{ptr}
	{
		f_loss = (m + c) / 2;
	}
	Node(): m{0}, c{0}, b{0}, B{0}, b_count{0}, B_count{0}, step{0}, f_loss{0}, parent{nullptr} {};
};

deque<Node> opened_list;
vector<Node> closed_list;
int m_num, c_num;

bool is_safe(Node *n)
{ // 判斷兩岸的傳教士是否安全
	int m_left = n->m;
	int c_left = n->c;
	int m_right = m_num - m_left;
	int c_right = c_num - c_left;

	if (m_left < c_left && m_left != 0) // 左岸食人魔數量不可超過傳教士數量，除非傳教士數量為0
		return false;
	if (m_right < c_right && m_right != 0) // 右岸食人魔數量不可超過傳教士數量，除非傳教士數量為0
		return false;
	if (m_left < 0 || c_left < 0)
		return false;
	if (m_left >= c_left || m_right >= c_right)
		return true;
}

int cal_value(Node *node)
{
	return node->m * 100 + node->c * 10 + node->b;
}

bool in_closed_list(Node *node)
{ // 確認是否已經在closed_list內
	for (auto x : closed_list)
	{
		if (x.m == node->m && x.c == node->c && x.b == node->b)
		{
			return true;
		}
	}
	return false;
}

void sort_by_floss()
{ // 將opened_list內的點按照分數大小排序
	for(auto i=opened_list.begin(); i!=opened_list.end(); i++){
		for(auto j=i+1; j!=opened_list.end(); j++){
			if(j->step>=i->f_loss){
				continue;
			}
			else
				swap(i, j);
		}
	}
}

void swap(Node &a, Node &b)
{
	Node tmp;
	tmp = a;
	a = b;
	b = tmp;
}

void refresh_opened(Node *n)
{ // 如果節點有較低的分數或步數，更新opened_list
	for (auto x : opened_list)
	{
		if (x.m == n->m && x.c == n->c && x.b == n->b)
		{
			if (n->f_loss < x.f_loss || n->step < x.step)
			{
				memcpy(&x, n, sizeof(Node));
			}
			return;
		}
	}
	opened_list.push_back(Node(n->m, n->c, n->b,n->B,n->b_count,n->B_count, n->step, n->parent));
}
void a_star_algorithm()
{
	while (opened_list.size() != 0)
	{
		// 從opened_list中取出分數最小的
		Node node;
		node = opened_list.front();
		opened_list.pop_front();
		// cout<<node.step<<" ";
		closed_list.push_back(Node(node.m, node.c, node.b, node.B,node.b_count,node.B_count, node.step, node.parent)); // 將取出的點加入closed_list中
		// 判斷取出的點是否為目標點
		if (node.m == 0 && node.c == 0 && node.b == 0)
			break;

		for (int i = 0; i <= m_num; i++)
		{ // i代表上船的傳教士數量
			for (int j = 0; j <= c_num; j++)
			{ // j代表上船的食人魔數量
				// 判斷船上的合理情況
				if (i + j == 0 || i + j > 2 || (i != 0 && i < j))
				{
					continue;
				}
				if (node.b == 1) // A船在左側，下一狀態船在右側
				{
					if (i > node.m || j > node.c)
					{
						continue;
					}
					Node *child_node = new Node(node.m - i, node.c - j, 0,1,node.b_count+1,node.B_count, node.step + 1, &closed_list.back());
					if (!in_closed_list(child_node))
					{
						if (is_safe(child_node))
						{
							refresh_opened(child_node);
						}
					}
				}
				else
				{ // 船在右側，下一狀態船在左側
					if (i > m_num - node.m || j > c_num - node.c)
					{
						continue;
					}
					Node *child_node = new Node(node.m + i, node.c + j, 1,1,node.b_count+1,node.B_count, node.step + 1, &closed_list.back());
					if (!in_closed_list(child_node))
					{
						if (is_safe(child_node))
						{
							refresh_opened(child_node);
						}
					}
				}
			}
		}
		sort_by_floss();
	}
}
void output()
{
	Node *current = &closed_list.back();
	while (current != NULL)
	{
		cout << "步數:" << current->step << "左岸傳教士:" << current->m << "左岸食人族:" << current->c << "右岸傳教士:" << m_num - current->m << "右岸食人族:" << c_num - current->c << "A船位:";
		if (current->b == 1)
			cout << "左" ;
		else
			cout << "右" ;
		cout<<"B船位:";
		if (current->B == 1)
			cout << "左";
		else
			cout << "右";
		cout<<"A船次數:"<<current->b_count<<"B船次數:"<<current->B_count<<"共花費:"<<3*current->b_count+25*current->B_count<<"元"<<endl;
		current = current->parent;
	}
}
int main(int argc, char const *argv[])
{
	SetConsoleOutputCP(CP_UTF8);

	cout << "請輸入初始傳教士人數：";
	cin >> m_num;
	cout << "請輸入初始野人人數：";
	cin >> c_num;

	closed_list.reserve((m_num + 1) * (c_num + 1) * 2 + 1); //解決記憶體位址跳掉的問題
	Node start_node(m_num, c_num, 1, 1, 0, 0, 0, nullptr);	//初始化
	opened_list.push_back(start_node);

	a_star_algorithm();
	output();

	return 0;
}