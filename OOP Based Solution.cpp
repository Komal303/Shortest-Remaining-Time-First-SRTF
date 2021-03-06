#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

int currentTime = 0;
//Process----------------------------------------------------------------
class Process
{
public:
	int processID;
	int arrivalTime;
	int burstTime;
	int waitTime;
	int completionTime;
	int turnaroundTime;
	int tempBurstTime;

	Process()
	{
		this->processID = -1;
	}
	Process(int pid, int at, int bt)
	{
		this->processID = pid;
		this->arrivalTime = at;
		this->burstTime = bt;
		this->waitTime = 0;
		this->completionTime = 0;
		this->turnaroundTime = 0;
		this->tempBurstTime = 0;
	}

	string getPID()
	{
		stringstream ss;
		ss << processID;
		string str = ss.str();

		string result = "pid_" + str;

		return result;
	}
};
//ReadyQueue----------------------------------------------------------------
class ReadyQueue
{
public:
	vector<Process> processQueue;

	bool isFinished()
	{
		if (processQueue.empty()) return false;
		for each (Process pr in processQueue)
		{
			if (pr.completionTime <= 0) return false;
		}
		return true;
	}

	bool addProcess(Process p)
	{
		for each (Process pr in processQueue)
		{
			if (pr.processID == p.processID) return false;
		}
		processQueue.push_back(p);
		return true;
	}

	Process removeProcess(Process p)
	{
		Process proc;

		if (processQueue.size() > 1)
		{
			int idx = 0;
			for each (Process pr in processQueue)
			{
				if (pr.processID == p.processID)
				{
					proc = pr;
					processQueue.erase(processQueue.begin() + idx);
					break;
				}
				idx++;
			}
		}
		else if ((processQueue.size() > 0) && (processQueue[0].processID == p.processID))
		{
			proc = processQueue[0];
			processQueue.erase(processQueue.begin());
		}
		return proc;

	}

	Process removeProcess(int index)
	{
		Process proc;
		if (index > (processQueue.size() - 1)) return proc;
		if (processQueue.size() > 1)
		{
			int idx = 0;
			for each (Process pr in processQueue)
			{
				if (idx == index)
				{
					proc = pr;
					processQueue.erase(processQueue.begin() + idx);
					break;
				}
				idx++;
			}
		}
		else if ((processQueue.size() > 0) && index == 0)
		{
			proc = processQueue[0];
			processQueue.erase(processQueue.begin());
		}
		return proc;
	}

	vector<Process> getAccessibleProcesses()
	{
		vector<Process> result;
		for each (Process pr in processQueue)
		{
			if (pr.arrivalTime <= currentTime)
			{
				result.push_back(pr);
			}
		}
		return result;
	}

	int getLowestArrivalTime()
	{
		vector<Process> processes = getAccessibleProcesses();
		if (processes.size() == 0) return -1;
		int result = processes[0].arrivalTime;
		//int len = 0;
		for each (Process pr in processes)
		{
			if (result > pr.arrivalTime) result = pr.arrivalTime;
			//else if (result == pr.arrivalTime) len++;
		}
		//if (len == processQueue.size()) return -1;
		return result;
	}

	int getLowestBurstTime()
	{
		vector<Process> processes = getAccessibleProcesses();
		if (processes.size() == 0) return -1;
		int result = processes[0].burstTime;
		//int len = 0;
		for each (Process pr in processes)
		{
			if (result > pr.burstTime) result = pr.burstTime;
			//else if (result == pr.arrivalTime) len++;
		}
		//if (len == processQueue.size()) return -1;
		return result;
	}

	Process extractProcess()
	{
		Process p;
		vector<Process> processes = getAccessibleProcesses();
		vector<Process> equalProcesses;
		vector<Process> subEqualProcesses;
		if (processes.size() == 0) return p;
		for each (Process pr in processes)
		{
			if (pr.burstTime == getLowestBurstTime())
			{
				equalProcesses.push_back(pr);
			}
		}
		int shortestAT = equalProcesses[0].arrivalTime;
		for each (Process pr in equalProcesses)
		{
			if (shortestAT > pr.arrivalTime)
			{
				shortestAT = pr.arrivalTime;
			}
		}
		for each (Process pr in equalProcesses)
		{
			if (pr.arrivalTime == shortestAT) subEqualProcesses.push_back(pr);
		}

		int oldestId = subEqualProcesses[0].processID;
		for each (Process pr in subEqualProcesses)
		{
			if (oldestId > pr.processID) oldestId = pr.processID;
		}
		for each (Process pr in subEqualProcesses)
		{
			if (oldestId == pr.processID) return removeProcess(pr);
		}
	}

	void updateWaits(int seconds)
	{
		for (int i = 0; i < seconds; i++)
		{
			for (int j = 0; j < processQueue.size(); j++)
			{
				if (processQueue[j].arrivalTime <= currentTime) processQueue[j].waitTime++;
			}
		}
	}

	void sort()
	{
		int i, j;
		int n = this->processQueue.size();
		for (i = 0; i < n - 1; i++)
		{
			for (j = 0; j < n - i - 1; j++)
			{
				if (this->processQueue[j].processID > this->processQueue[j + 1].processID)
				{
					Process temp = this->processQueue[j];
					this->processQueue[j] = this->processQueue[j + 1];
					this->processQueue[j + 1] = temp;
				}
			}
		}
	}

	void takeInput()
	{
		int numberOfProcesses = 0;
		cout << "Enter number of processes: ";
		cin >> numberOfProcesses;
		int i;
		for (i = 0; i < numberOfProcesses; i++)
		{
			int arrivalTime = 0;
			int burstTime = 0;
			cout << "Enter arrival time of process " << i + 1 << " : ";
			cin >> arrivalTime;
			cout << "Enter burst time of process " << i + 1 << " : ";
			cin >> burstTime;
			Process pr(i, arrivalTime, burstTime);
			this->addProcess(pr);
		}
	}
};
//Calculate----------------------------------------------------------------
class Calculate
{
public:
	ReadyQueue perform(ReadyQueue rq)
	{
		ReadyQueue result;
		currentTime = 0;

		while (!rq.processQueue.empty())
		{

			Process pr;
			while (true)
			{
				pr = rq.extractProcess();
				if (pr.processID == -1) currentTime++;
				else break;
			}
			if (pr.burstTime > 0)
			{
				rq.updateWaits(1);
				pr.burstTime--;
				pr.tempBurstTime++;
				currentTime++;
				if (pr.burstTime == 0)
				{
					pr.completionTime = currentTime;
					pr.turnaroundTime = pr.completionTime - pr.arrivalTime;
					pr.burstTime = pr.tempBurstTime;
					result.addProcess(pr);
				}
				else
				{
					rq.processQueue.push_back(pr);
				}
			}
		}
		return result;
	}
	/*ReadyQueue perform(ReadyQueue rq)
	{
		ReadyQueue result;
		currentTime = 0;
		int iter = rq.processQueue.size();

		for (int i = 0; i < iter; i++)
		{
			Process pr;
			while (true)
			{
				pr = rq.extractProcess();
				if (pr.processID == -1) currentTime++;
				else break;
			}

			//currentTime = pr.arrivalTime;
			for (int j = 0; j < pr.burstTime; j++)
			{
				rq.updateWaits(1);
				currentTime++;
			}
			pr.completionTime = currentTime;
			pr.turnaroundTime = pr.completionTime - pr.arrivalTime;
			result.addProcess(pr);
		}
		return result;
	}*/

	void display(ReadyQueue rqs)
	{
		ReadyQueue rq;
		rq = this->perform(rqs);
		rq.sort();
		cout << "==============================================================================" << endl;
		cout << "ProcessID" << " | " << "ArrivalTime" << " | " << "BurstTime" << " | " << "WaitingTime" << " | " << "CompletionTime" << " | " << "TAT" << "     | " << endl;

		for each (Process pr in rq.processQueue)
		{
			stringstream ss;
			string result;
			unsigned int iter = 0;

			result = pr.getPID();
			iter = result.length();
			for (int i = 0; i < (10 - iter); i++)
			{
				result += " ";
			}
			cout << result << "| ";
			result = "";

			ss << pr.arrivalTime;
			result = ss.str();
			iter = result.length();
			for (int i = 0; i < (12 - iter); i++)
			{
				result += " ";
			}
			cout << result << "| ";
			result = "";

			ss.str("");
			ss << pr.burstTime;
			result = ss.str();
			iter = result.length();
			for (int i = 0; i < (10 - iter); i++)
			{
				result += " ";
			}
			cout << result << "| ";
			result = "";

			ss.str("");
			ss << pr.waitTime;
			result = ss.str();
			iter = result.length();
			for (int i = 0; i < (12 - iter); i++)
			{
				result += " ";
			}
			cout << result << "| ";
			result = "";

			ss.str("");
			ss << pr.completionTime;
			result = ss.str();
			iter = result.length();
			for (int i = 0; i < (15 - iter); i++)
			{
				result += " ";
			}
			cout << result << "| ";
			result = "";

			ss.str("");
			ss << pr.turnaroundTime;
			result = ss.str();
			iter = result.length();
			for (int i = 0; i < (8 - iter); i++)
			{
				result += " ";
			}
			cout << result << "| " << endl;
			result = "";
		}
		cout << "==============================================================================" << endl;
		//cout << rq.isFinished() << endl;
		float total = 0;

		for each (Process pr in rq.processQueue)
		{
			total += pr.waitTime;
		}
		cout << "Average waiting time = " << total / (rq.processQueue.size()) << endl;

		total = 0;
		for each (Process pr in rq.processQueue)
		{
			total += pr.turnaroundTime;
		}
		cout << "Average turn around time = " << total / (rq.processQueue.size()) << endl;
	}
};

class Gantt
{
public:
	int getMaxLength(ReadyQueue rq)
	{
		int result = rq.processQueue[0].completionTime;
		for each (Process pr in rq.processQueue)
		{
			if (result < pr.completionTime) result = pr.completionTime;
		}
		return result;
	}

	void drawLine(ReadyQueue rq)
	{
		stringstream ss;
		ss << getMaxLength(rq);
		string str = ss.str();

		for (int i = 0; i < (78 - str.length()); i++)
		{
			if (i == 0) cout << "0";
			else cout << "-";
		}
		cout << str << endl;
	}

	int getProcessPosition(int at, ReadyQueue rq)
	{
		if (at == 0) return 0;
		float percent = (float)at / (float)getMaxLength(rq);
		int result = (float)78 * (float)percent;
		return result;
	}

	int getLowestAt(ReadyQueue rq)
	{
		int result = rq.processQueue[0].arrivalTime;
		for each (Process pr in rq.processQueue)
		{
			if (result > pr.arrivalTime) result = pr.arrivalTime;
		}
		return result;
	}

	void drawGantt(ReadyQueue rq)
	{
		cout << endl;
		drawLine(rq);
		cout << endl;

		int pos = 0;
		for each (Process pr in rq.processQueue)
		{
			if (getLowestAt(rq) == pr.arrivalTime)
			{
				pos = getProcessPosition(rq.removeProcess(pr).arrivalTime, rq);
				break;
			}
		}

		for (int i = 0; i < 78; i++)
		{
			if (pos <= i && rq.processQueue.size() == 0)
			{
				cout << "p";
				pos = 10000000;
			}
			if (pos <= i)
			{
				cout << "p";
				for each (Process pr in rq.processQueue)
				{
					int lowestAt = getLowestAt(rq);
					if (lowestAt == pr.arrivalTime)
					{
						pos = getProcessPosition(rq.removeProcess(pr).arrivalTime, rq);
						break;
					}
				}
			}
			else
			{
				cout << "-";
			}

		}
		cout << endl;
	}
};

int main()
{

	Process p1(0, 2, 6);
	Process p2(1, 5, 2);
	Process p3(2, 1, 8);
	Process p4(3, 0, 3);
	Process p5(4, 4, 4);
	ReadyQueue rq;

	rq.addProcess(p1);
	rq.addProcess(p2);
	rq.addProcess(p3);
	rq.addProcess(p4);
	rq.addProcess(p5);

	Calculate c;
	c.display(rq);

	system("pause");
	return 0;
}