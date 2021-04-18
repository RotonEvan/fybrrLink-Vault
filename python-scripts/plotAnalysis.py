import pandas as pd
from numpy import double
import matplotlib.pyplot as plt
import numpy as np
from collections import defaultdict,OrderedDict
import random

def getMovingAverageValue(numbers):
	window_size = 50

	i = 0
	moving_averages = []
	while i < len(numbers) - window_size + 1:
	    this_window = numbers[i : i + window_size]
	    window_average = sum(this_window) / window_size
	    moving_averages.append(window_average)
	    i += 1

	return moving_averages

def plotAvgCD(data):
	plt.plot(data[1], label="Dijkstra")
	plt.plot(data[0], label="fybrrLink")
	plt.xlabel("Time(s)")
	plt.ylabel("Average Congestion Degree of network")

	plt.legend()
	plt.show()

def plotAvgExecutionTime(data):
	dic_dijkstra = defaultdict(list)
	dic_fybrr = defaultdict(list)

	for i in range(len(data[11])):
		dic_dijkstra[data[11][i]].append(data[12][i])
		dic_fybrr[data[13][i]].append(data[14][i])

	dic_dijkstra = OrderedDict(sorted(dic_dijkstra.items()))
	dic_fybrr = OrderedDict(sorted(dic_fybrr.items()))

	data_to_plot = [[] for _ in range(4)]

	for i in dic_fybrr.keys():
		dic_fybrr[i] = sum(dic_fybrr[i])/len(dic_fybrr[i])
		data_to_plot[0].append(i)
		data_to_plot[1].append(dic_fybrr[i])

	for i in dic_fybrr.keys():
		dic_dijkstra[i] = sum(dic_dijkstra[i])/len(dic_dijkstra[i])
		data_to_plot[2].append(i)
		data_to_plot[3].append(dic_dijkstra[i])

	for i in range(len(data_to_plot)):
		print(data_to_plot[i])

	plt.plot(data_to_plot[0], data_to_plot[1], label="Dijkstra")
	plt.plot(data_to_plot[2], data_to_plot[3], label="fybrrLink")
	plt.xlabel("Number of hops in the generated path")
	plt.ylabel("Average execution time(ms)")

	plt.legend()
	plt.show()

def printAvgLatency(data):
	dic_dijkstra = defaultdict(list)
	dic_fybrr = defaultdict(list)
	dic_threshold = defaultdict(list)

	for i in range(len(data[3])):
		dic_dijkstra[data[11][i]].append(data[3][i])
		dic_fybrr[data[13][i]].append(data[6][i])
		dic_threshold[min(data[11][i], data[13][i])].append(data[9][i])

	dic_dijkstra = OrderedDict(sorted(dic_dijkstra.items()))
	dic_fybrr = OrderedDict(sorted(dic_fybrr.items()))
	dic_threshold = OrderedDict(sorted(dic_threshold.items()))

	data_to_plot = [[] for _ in range(6)]

	for i in dic_fybrr.keys():
		dic_fybrr[i] = sum(dic_fybrr[i])/len(dic_fybrr[i])
		data_to_plot[0].append(i)
		data_to_plot[1].append(dic_fybrr[i])

	for i in dic_dijkstra.keys():
		dic_dijkstra[i] = sum(dic_dijkstra[i])/len(dic_dijkstra[i])
		data_to_plot[2].append(i)
		data_to_plot[3].append(dic_dijkstra[i])

	for i in dic_threshold.keys():
		dic_threshold[i] = sum(dic_threshold[i])/len(dic_threshold[i])
		data_to_plot[4].append(i)
		data_to_plot[5].append(dic_threshold[i])

	# n = len(data[3])
	# lis = list(range(n))
	# random.Random(1).shuffle(lis)
	# # print(lis, n,list(range(n)))
	# index = 1
	# for i in lis:
	# 	sum_latency[0].append(data[3][i])
	# 	sum_latency[1].append(data[6][i])
	# 	sum_latency[2].append(data[9][i])

	plt.plot(data_to_plot[0], data_to_plot[1], label="Dijkstra")
	plt.plot(data_to_plot[2], data_to_plot[3], label="fybrrLink")
	# plt.plot(data_to_plot[4], data_to_plot[5], label="Flow constraints")
	plt.xlabel("Number of hops in the generated path")
	plt.ylabel("Average latency of generated paths(ms)")

	plt.legend()
	plt.show()

def printAvgLatency2(data):
	data_to_plot = [[0] for _ in range(3)]
	sum_latency = [[0] for _ in range(3)]

	n = len(data[3])
	lis = list(range(n))
	random.Random(1).shuffle(lis)
	# print(lis, n,list(range(n)))
	index = 1
	for i in lis:
		sum_latency[0].append(data[3][i])
		sum_latency[1].append(data[6][i])
		sum_latency[2].append(data[9][i])
		# sum_latency[0].append(sum_latency[0][-1] + data[3][i])
		# sum_latency[1].append(sum_latency[1][-1] + data[6][i])
		# sum_latency[2].append(sum_latency[2][-1] + data[9][i])
		# for j in range(3):
		# 	data_to_plot[j].append(sum_latency[j][-1]/index)
		# index += 1

	plt.plot(getMovingAverageValue(sum_latency[1]), label="Dijkstra")
	plt.plot(getMovingAverageValue(sum_latency[0]), label="fybrrLink")
	# plt.plot(getMovingAverageValue(sum_latency[2]), label="Flow constraints")

	plt.xlabel("Time(s)")
	plt.ylabel("Average latency of generated paths(ms)")

	plt.legend()
	plt.show()

def printAvgPLR(data):
	dic_dijkstra = defaultdict(list)
	dic_fybrr = defaultdict(list)
	# dic_threshold = defaultdict(list)

	for i in range(len(data[3])):
		dic_dijkstra[data[11][i]].append(data[4][i])
		dic_fybrr[data[13][i]].append(data[7][i])
		# dic_threshold[min(data[11][i], data[13][i])].append(data[10][i])

	dic_dijkstra = OrderedDict(sorted(dic_dijkstra.items()))
	dic_fybrr = OrderedDict(sorted(dic_fybrr.items()))
	# dic_threshold = OrderedDict(sorted(dic_threshold.items()))

	data_to_plot = [[] for _ in range(6)]

	for i in dic_fybrr.keys():
		dic_fybrr[i] = sum(dic_fybrr[i])/len(dic_fybrr[i])
		data_to_plot[0].append(i)
		data_to_plot[1].append(dic_fybrr[i])

	for i in dic_dijkstra.keys():
		dic_dijkstra[i] = sum(dic_dijkstra[i])/len(dic_dijkstra[i])
		data_to_plot[2].append(i)
		data_to_plot[3].append(dic_dijkstra[i])

	# for i in dic_threshold.keys():
	# 	dic_threshold[i] = sum(dic_threshold[i])/len(dic_threshold[i])
	# 	data_to_plot[4].append(i)
	# 	data_to_plot[5].append(dic_threshold[i])

	plt.plot(data_to_plot[0], data_to_plot[1], label="Dijkstra")
	plt.plot(data_to_plot[2][:9], data_to_plot[3][:9], label="fybrrLink")
	# plt.plot(data_to_plot[4], data_to_plot[5], label="Flow constraints")
	plt.xlabel("Number of hops in the generated path")
	plt.ylabel("Average Packet Loss Rate of generated paths")

	plt.legend()
	plt.show()

def printAvgPLR2(data):
	data_to_plot = [[0] for _ in range(3)]
	sum_latency = [[0] for _ in range(3)]

	n = len(data[4])
	lis = list(range(n))
	random.Random(1).shuffle(lis)
	# print(lis, n,list(range(n)))
	index = 1
	for i in lis:
		sum_latency[0].append(data[4][i])
		sum_latency[1].append(data[7][i])
		sum_latency[2].append(data[10][i])
		# sum_latency[0].append(sum_latency[0][-1] + data[3][i])
		# sum_latency[1].append(sum_latency[1][-1] + data[6][i])
		# sum_latency[2].append(sum_latency[2][-1] + data[9][i])
		# for j in range(3):
		# 	data_to_plot[j].append(sum_latency[j][-1]/index)
		index += 1

	plt.plot(getMovingAverageValue(sum_latency[1]), label="Dijkstra")
	plt.plot(getMovingAverageValue(sum_latency[0]), label="fybrrLink")
	# plt.plot(getMovingAverageValue(sum_latency[2]), label="Flow constraints")

	plt.xlabel("Time(s)")
	plt.ylabel("Average Packet Loss Rate of generated paths")

	plt.legend()
	plt.show()

def printAvgPLR2(data):
	data_to_plot = [[0] for _ in range(3)]
	sum_latency = [[0] for _ in range(3)]

	n = len(data[4])
	lis = list(range(n))
	random.Random(1).shuffle(lis)
	# print(lis, n,list(range(n)))
	index = 1
	for i in lis:
		sum_latency[0].append(data[4][i])
		sum_latency[1].append(data[7][i])
		sum_latency[2].append(data[10][i])
		# sum_latency[0].append(sum_latency[0][-1] + data[3][i])
		# sum_latency[1].append(sum_latency[1][-1] + data[6][i])
		# sum_latency[2].append(sum_latency[2][-1] + data[9][i])
		# for j in range(3):
		# 	data_to_plot[j].append(sum_latency[j][-1]/index)
		index += 1

	plt.plot(getMovingAverageValue(sum_latency[1]), label="Dijkstra")
	plt.plot(getMovingAverageValue(sum_latency[0]), label="fybrrLink")
	# plt.plot(getMovingAverageValue(sum_latency[2]), label="Flow constraints")

	plt.xlabel("Time(s)")
	plt.ylabel("Average Packet Loss Rate of generated paths")

	plt.legend()
	plt.show()

def plotHopCount(data):
	# bins = np.linspace(0, len(data[13]), 1)
	bins = list(range(0, len(data[13])))
	dic_fybrr = defaultdict(int)
	dic_dijkstra = defaultdict(int)
	for i in range(len(data[13])):
		dic_fybrr[data[11][i]] += 1
		dic_dijkstra[data[13][i]] += 1
	dic_dijkstra = OrderedDict(sorted(dic_dijkstra.items()))
	dic_fybrr = OrderedDict(sorted(dic_fybrr.items()))

	values, labels = [],[]
	for i in dic_fybrr.keys():
		values.append(dic_fybrr[i])
		labels.append(i)

	plt.plot(labels,values, label="Dijkstra")
	
	values, labels = [],[]
	for i in dic_dijkstra.keys():
		values.append(dic_dijkstra[i])
		labels.append(i)
	plt.plot(labels,values,label="fybrrLink")

	

	plt.legend()
	plt.show()

	

	# plt.pie(values,counterclock=False, shadow=True, autopct='%1.1f%%')
	# plt.legend(labels,loc=3)
	# plt.show()



def plotPies(total_flows, no_qos_aware_flows, no_flows_satisfied_fybrr, no_flows_satisfied_dijkstra):
	values = [no_qos_aware_flows, total_flows-no_qos_aware_flows,no_flows_satisfied_fybrr, total_flows - no_flows_satisfied_fybrr, no_flows_satisfied_dijkstra, total_flows - no_flows_satisfied_dijkstra]
	colors = ['b', 'g', 'c', 'r', 'm', 'y']
	labels = ['QoS Aware Flows', 'Non-QoS Aware Flows', 'Flows satisfied by fybrrLink', 'Other flows', 'Flows satisfied by Plane Dijkstra', 'Other flows']
	title = ['Distribution of flows', 'Distribution of QoS-satisfied flows']
	explode = (0.2, 0)
	for i in range(3):
		index = 2*i
		plt.pie(values[index:index+2], colors=colors[index:index+2],explode=explode,counterclock=False, shadow=True, autopct='%1.1f%%')
		plt.title(title[bool(i)])
		plt.legend(labels[index:index+2],loc=3)
		plt.show()

if __name__ == "__main__":
	df=pd.read_csv('analysis.csv', sep=',',header=None,dtype=object,na_filter=False)
	
	no_of_column = 15
	data = [[] for i in range(no_of_column)]

	for i in range(1, len(df)):
		for j in range(no_of_column):
			data[j].append(double(df[j][i]))

	plotAvgExecutionTime(data)
	printAvgLatency2(data)
	printAvgLatency(data)
	printAvgPLR2(data)
	printAvgPLR(data)
	# plotAvgCD(data)
	# plotPies(324, 280, 304, 145)