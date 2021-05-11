'''
	Command to convert all png files to eps : mogrify -format eps *.png
'''

import pandas as pd
from numpy import double
import matplotlib.pyplot as plt
from matplotlib import cm
import numpy as np
from collections import defaultdict,OrderedDict
import random
from math import log10
import seaborn as sns
from matplotlib.patches import FancyBboxPatch
import statistics
from scipy.interpolate import interp1d



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
	plot_data = [[] for i in range(4)]

	for i in range(len(data[1])):
		plot_data[0].append(data[1][i])
		plot_data[1].append(data[0][i])
		if data[21][i] != 0:
			plot_data[2].append(data[21][i])
		plot_data[3].append(data[33][i])

	for i in range(4):
		print(statistics.median(plot_data[i]))

	colors = ['#1f77b4', '#ff7f0e','#2ca02c','#d62728']
	box = plt.boxplot(plot_data, showfliers=False, patch_artist=True,medianprops=dict(color='black'))
	plt.xticks([1, 2, 3, 4], ['Dijkstra', 'fybrrLink', 'SDRA', 'Sway'])

	for patch, color in zip(box['boxes'], colors):
		patch.set_facecolor(color)

	plt.ylabel("log10(Path generation time(ms))")
	plt.show()

	# plt.plot(plot_data[0], label="Dijkstra",ls='--')
	# plt.plot(plot_data[1], label="fybrrLink")
	# plt.plot(plot_data[2], label="SDRA", ls=':')
	# plt.plot(plot_data[3], label="Sway", ls='-.')
	# plt.xlabel("Time(s)")
	# plt.ylabel("Average Congestion Degree of network")

	# plt.legend(loc='upper left', shadow=True)
	# plt.show()

def plotAvgExecutionTime(data):
	dic_dijkstra = defaultdict(list)
	dic_fybrr = defaultdict(list)
	dic_sdra = defaultdict(list)
	dic_sway = defaultdict(list)

	for i in range(len(data[11])):
		dic_dijkstra[data[11][i]].append(log10(data[12][i]))
		dic_fybrr[data[13][i]].append(log10(data[14][i]))
		if data[20][i] != 0:
			dic_sdra[data[19][i]].append(log10(data[20][i]))
		dic_sway[data[31][i]].append(log10(data[32][i]))

	# for i in range(len(data[11])):
	# 	dic_dijkstra[data[11][i]].append(data[12][i])
	# 	dic_fybrr[data[13][i]].append(data[14][i])
	# 	if data[20][i] != 0:
	# 		dic_sdra[data[19][i]].append(data[20][i])
	# 	dic_sway[data[31][i]].append(data[32][i])


	dic_dijkstra = OrderedDict(sorted(dic_dijkstra.items()))
	dic_fybrr = OrderedDict(sorted(dic_fybrr.items()))
	dic_sdra = OrderedDict(sorted(dic_sdra.items()))
	dic_sway = OrderedDict(sorted(dic_sway.items()))


	data_to_plot = [[] for _ in range(8)]

	for i in dic_fybrr.keys():
		dic_fybrr[i] = sum(dic_fybrr[i])/len(dic_fybrr[i])
		data_to_plot[0].append(i)
		data_to_plot[1].append(dic_fybrr[i])

	for i in dic_fybrr.keys():
		dic_dijkstra[i] = sum(dic_dijkstra[i])/len(dic_dijkstra[i])
		data_to_plot[2].append(i)
		data_to_plot[3].append(dic_dijkstra[i])

	for i in dic_sdra.keys():
		dic_sdra[i] = sum(dic_sdra[i])/len(dic_sdra[i])
		data_to_plot[4].append(i)
		data_to_plot[5].append(dic_sdra[i])

	for i in range(1,10):
		dic_sway[i] = sum(dic_sway[i])/len(dic_sway[i])
		data_to_plot[6].append(i)
		data_to_plot[7].append(dic_sway[i])

	for i in range(len(data_to_plot)):
		print(data_to_plot[i])

	plt.plot(data_to_plot[0], data_to_plot[1], label="Dijkstra", ls = '--')
	plt.plot(data_to_plot[2], data_to_plot[3], label="fybrrLink")
	plt.plot(data_to_plot[4], data_to_plot[5], label="SDRA", ls = ':')
	plt.plot(data_to_plot[6], data_to_plot[7], label="Sway", ls = '-.')
	plt.xlabel("Number of hops in the generated path")
	plt.ylabel("log10(Average path generation time(ms))")

	plt.legend(loc='upper left', shadow=True)
	plt.show()

def plotAverageExecutionTime2(data):
	plot_data = [[] for i in range(4)]

	for i in range(len(data[12])):
		plot_data[0].append(data[14][i])
		plot_data[1].append(data[12][i])
		if data[20][i] != 0:
			plot_data[2].append(data[20][i])
		plot_data[3].append(data[32][i])

	for i in range(4):
		print(statistics.median(plot_data[i]))

	colors = ['#1f77b4', '#ff7f0e','#2ca02c','#d62728']
	box = plt.boxplot(plot_data, showfliers=False, patch_artist=True,medianprops=dict(color='black'))
	plt.xticks([1, 2, 3, 4], ['Dijkstra', 'fybrrLink', 'SDRA', 'Sway'])

	for patch, color in zip(box['boxes'], colors):
		patch.set_facecolor(color)

	plt.ylabel("log10(Path generation time(ms))")
	plt.show()

def printAvgLatency(data):
	dic_dijkstra = defaultdict(list)
	dic_fybrr = defaultdict(list)
	dic_sdra = defaultdict(list)
	dic_sway = defaultdict(list)

	l = [0,0]
	for i in range(len(data[3])):
		dic_dijkstra[data[13][i]].append(data[6][i])
		dic_fybrr[data[11][i]].append(data[3][i])
		if data[17][i] != 0:
			l[0] += 1
			l[1] += data[17][i]
		dic_sdra[data[19][i]].append(data[17][i])
		dic_sway[data[31][i]].append(data[29][i])

	dic_dijkstra = OrderedDict(sorted(dic_dijkstra.items()))
	dic_fybrr = OrderedDict(sorted(dic_fybrr.items()))
	dic_sdra = OrderedDict(sorted(dic_sdra.items()))
	dic_sway = OrderedDict(sorted(dic_sway.items()))

	data_to_plot = [[] for _ in range(8)]
# 
	print(l[1]/l[0])

	for i in range(1,10):
		dic_fybrr[i] = sum(dic_fybrr[i])/len(dic_fybrr[i])
		data_to_plot[0].append(i)
		data_to_plot[1].append(dic_fybrr[i])

	for i in range(1,10):
		dic_dijkstra[i] = sum(dic_dijkstra[i])/len(dic_dijkstra[i])
		data_to_plot[2].append(i)
		data_to_plot[3].append(dic_dijkstra[i])

	for i in range(1, 10):
		dic_sdra[i] = sum(dic_sdra[i])/len(dic_sdra[i])
		data_to_plot[4].append(i)
		data_to_plot[5].append(dic_sdra[i])

	for i in range(1,10):
		dic_sway[i] = sum(dic_sway[i])/len(dic_sway[i])
		data_to_plot[6].append(i)
		data_to_plot[7].append(dic_sway[i])

	# n = len(data[3])
	# lis = list(range(n))
	# random.Random(1).shuffle(lis)
	# # print(lis, n,list(range(n)))
	# index = 1
	# for i in lis:
	# 	sum_latency[0].append(data[3][i])
	# 	sum_latency[1].append(data[6][i])
	# 	sum_latency[2].append(data[9][i])


	plt.plot(data_to_plot[2], data_to_plot[3], label="Dijkstra", ls = '--')
	plt.plot(data_to_plot[0], data_to_plot[1], label="fybrrLink")
	plt.plot(data_to_plot[4], data_to_plot[5], label="SDRA", ls = ':')
	plt.plot(data_to_plot[6], data_to_plot[7], label="Sway", ls = '-.')
	plt.xlabel("Number of hops in the generated path")
	plt.ylabel("Average latency of generated paths(ms)")

	plt.legend(loc='upper left', shadow=True)
	plt.show()

def printAvgLatency2(data):
	data_to_plot = [[0] for _ in range(4)]
	sum_latency = [[0] for _ in range(4)]

	n = len(data[3])
	lis = list(range(n))
	random.Random(3).shuffle(lis)
	# print(lis, n,list(range(n)))
	index = 1
	# for i in lis:
	for i in range(n):
		sum_latency[0].append(data[3][i])
		sum_latency[1].append(data[6][i])
		sum_latency[2].append(data[17][i])
		sum_latency[3].append(data[29][i])
		# sum_latency[0].append(sum_latency[0][-1] + data[3][i])
		# sum_latency[1].append(sum_latency[1][-1] + data[6][i])
		# sum_latency[2].append(sum_latency[2][-1] + data[9][i])
		# for j in range(3):
		# 	data_to_plot[j].append(sum_latency[j][-1]/index)
		# index += 1



	plt.plot(getMovingAverageValue(sum_latency[1]), label="Dijkstra", ls = '--')
	plt.plot(getMovingAverageValue(sum_latency[0]), label="fybrrLink")
	plt.plot(getMovingAverageValue(sum_latency[2]), label="SDRA", ls = ':')
	plt.plot(getMovingAverageValue(sum_latency[3]), label="Sway", ls = '-.')
	plt.ylim(top=115)

	plt.xlabel("Time(s)")
	plt.ylabel("Average Latency of generated paths(ms)")

	plt.legend(loc='upper left', shadow=True)
	plt.show()

def printAvgLatency3(data):
	data_to_plot = [[0] for _ in range(4)]

	for i in range(len(data[6])):
		data_to_plot[0].append(data[6][i])
		data_to_plot[1].append(data[3][i])
		if data[18][i] != 0:
			data_to_plot[2].append(data[17][i])
		data_to_plot[3].append(data[29][i])

	for i in range(4):
		print(statistics.median(data_to_plot[i]))

	colors = ['#1f77b4', '#ff7f0e','#2ca02c','#d62728']
	box = plt.boxplot(data_to_plot,showfliers=False, patch_artist=True, medianprops=dict(color='black'))
	plt.xticks([1, 2, 3, 4], ['Dijkstra', 'fybrrLink', 'SDRA', 'Sway'])

	for patch, color in zip(box['boxes'], colors):
		patch.set_facecolor(color)

	plt.ylabel("Latency of generated paths(ms)")

	plt.show()

def printAvgPLR(data):
	dic_dijkstra = defaultdict(list)
	dic_fybrr = defaultdict(list)
	dic_sdra = defaultdict(list)
	dic_sway = defaultdict(list)
	l = [0,0, 0, 0, 0]
	for i in range(len(data[3])):
		dic_dijkstra[data[13][i]].append(data[7][i])
		dic_fybrr[data[11][i]].append(data[4][i])
		if data[18][i] != 0:
			l[0] += 1
			l[1] += data[7][i]
			l[2] += data[4][i]
			l[3] += data[18][i]
			l[4] += data[30][i]
		dic_sdra[data[19][i]].append(data[18][i])
		dic_sway[data[31][i]].append(data[30][i])

	dic_dijkstra = OrderedDict(sorted(dic_dijkstra.items()))
	dic_fybrr = OrderedDict(sorted(dic_fybrr.items()))
	dic_sdra = OrderedDict(sorted(dic_sdra.items()))
	dic_sway = OrderedDict(sorted(dic_sway.items()))

	data_to_plot = [[] for _ in range(8)]
	print(l[1]/l[0], l[2]/l[0], l[3]/l[0], l[4]/l[0])

	for i in range(1,10):
		dic_fybrr[i] = sum(dic_fybrr[i])/len(dic_fybrr[i])
		data_to_plot[0].append(i)
		data_to_plot[1].append(dic_fybrr[i])

	for i in range(1,10):
		dic_dijkstra[i] = sum(dic_dijkstra[i])/len(dic_dijkstra[i])
		data_to_plot[2].append(i)
		data_to_plot[3].append(dic_dijkstra[i])

	for i in range(1, 10):
		dic_sdra[i] = sum(dic_sdra[i])/len(dic_sdra[i])
		data_to_plot[4].append(i)
		data_to_plot[5].append(dic_sdra[i])

	for i in range(1,10):
		dic_sway[i] = sum(dic_sway[i])/len(dic_sway[i])
		data_to_plot[6].append(i)
		data_to_plot[7].append(dic_sway[i])


	plt.plot(data_to_plot[2], data_to_plot[3], label="Dijkstra", ls = '--')
	plt.plot(data_to_plot[0], data_to_plot[1], label="fybrrLink")
	plt.plot(data_to_plot[4], data_to_plot[5], label="SDRA", ls = ':')
	plt.plot(data_to_plot[6], data_to_plot[7], label="Sway", ls = '-.')
	plt.xlabel("Number of hops in the generated path")
	plt.ylabel("Average Packet Loss Rate of generated paths")

	plt.legend(loc='upper left', shadow=True)
	plt.show()

def printAvgPLR2(data):
	data_to_plot = [[0] for _ in range(4)]
	sum_latency = [[0] for _ in range(4)]

	n = len(data[4])
	lis = list(range(n))
	random.Random(3).shuffle(lis)
	# print(lis, n,list(range(n)))
	index = 1
	for i in range(n):
		sum_latency[0].append(data[4][i])
		sum_latency[1].append(data[7][i])
		sum_latency[2].append(data[18][i])
		sum_latency[3].append(data[30][i])
		# sum_latency[0].append(sum_latency[0][-1] + data[3][i])
		# sum_latency[1].append(sum_latency[1][-1] + data[6][i])
		# sum_latency[2].append(sum_latency[2][-1] + data[9][i])
		# for j in range(3):
		# 	data_to_plot[j].append(sum_latency[j][-1]/index)
		index += 1


	plt.plot(getMovingAverageValue(sum_latency[1]), label="Dijkstra", ls = '--')
	plt.plot(getMovingAverageValue(sum_latency[0]), label="fybrrLink")
	plt.plot(getMovingAverageValue(sum_latency[2]), label="SDRA", ls = ':')
	plt.plot(getMovingAverageValue(sum_latency[3]), label="Sway", ls = '-.')

	plt.xlabel("Time(s)")
	plt.ylabel("Average Packet Loss Rate of generated paths")

	plt.legend(loc='upper left', shadow=True)
	plt.show()


def printAvgPLR3(data):
	data_to_plot = [[0] for _ in range(4)]

	for i in range(len(data[7])):
		data_to_plot[0].append(data[7][i])
		data_to_plot[1].append(data[4][i])
		if data[18][i] != 0:
			data_to_plot[2].append(data[18][i])
		data_to_plot[3].append(data[30][i])

	for i in range(4):
		print(statistics.median(data_to_plot[i]))

	colors = ['#1f77b4', '#ff7f0e','#2ca02c','#d62728']
	box = plt.boxplot(data_to_plot,showfliers=False, patch_artist=True,medianprops=dict(color='black'))
	plt.xticks([1, 2, 3, 4], ['Dijkstra', 'fybrrLink', 'SDRA', 'Sway'])

	for patch, color in zip(box['boxes'], colors):
		patch.set_facecolor(color)

	plt.ylabel("Packet Loss Rate of generated paths")

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

	plt.legend(loc='upper left', shadow=True)
	plt.show()



def plotPies(total_flows, no_qos_aware_flows, no_flows_satisfied_fybrr, no_flows_satisfied_dijkstra, no_flows_sdra, no_flows_sway):
	values = [no_qos_aware_flows, total_flows-no_qos_aware_flows,no_flows_satisfied_fybrr, total_flows - no_flows_satisfied_fybrr, no_flows_satisfied_dijkstra, total_flows - no_flows_satisfied_dijkstra]
	colors = ['b', 'g', 'c', 'r', 'm', 'y']
	labels = ['QoS Aware Flows', 'Non-QoS Aware Flows', 'Flows satisfied by fybrrLink', 'Other flows', 'Flows satisfied by Plane Dijkstra', 'Other flows']
	title = ['Distribution of flows', 'Distribution of QoS-satisfied flows']
	explode = (0.2, 0)
	for i in range(1):
		index = 2*i
		plt.pie(values[index:index+2], colors=colors[index:index+2],explode=explode,counterclock=False, shadow=True, autopct='%1.1f%%')
		plt.title(title[bool(i)])
		plt.legend(labels[index:index+2],loc=3)
		plt.show()

	labels = ['fybrrLink','Sway','SDRA' ,'Dijkstra']
	y = np.array([no_flows_satisfied_fybrr, no_flows_sway, no_flows_sdra, no_flows_satisfied_dijkstra ])
	y = (y/total_flows)*100
	# colors = cm.hsv(y / float(max(y)))
	colors = ['#ff7f0e','#d62728', '#2ca02c','#1f77b4']
	# plot = plt.scatter(y, y, c = y, cmap = 'hsv')
	plt.clf()
	# plt.colorbar(plot)
	plt.barh(labels[:4],y, color = colors)
	plt.xlabel("Percentage of flows satisfied")

	for index, value in enumerate(y):
		plt.text(value,index, " " + str(value)[:4] + "%   ",c='black', fontsize=12, horizontalalignment='right',  verticalalignment='center')
	plt.show()


if __name__ == "__main__":
	df=pd.read_csv('analysis.csv', sep=',',header=None,dtype=object,na_filter=False)
	
	no_of_column = 34
	data = [[] for i in range(no_of_column)]

	for i in range(1, 325):
		for j in range(no_of_column):
			data[j].append(double(df[j][i]))

	# plotAvgExecutionTime(data)
	# plotAverageExecutionTime2(data)
	# printAvgLatency2(data)
	# printAvgLatency(data)
	# printAvgLatency3(data)
	# printAvgPLR2(data)
	# printAvgPLR3(data) 
	# printAvgPLR(data)
	plotAvgCD(data)
	# plotPies(324, 280, 304, 145, 177, 285)
