import networkx as nx
from matplotlib import pyplot as plt
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
from matplotlib.figure import Figure
import numpy as np
import random
import base64
import io
import time
import pandas as pd

class ISL:
    def __init__(self, link_type):
        self.link_type = link_type
        if self.link_type == 0:
            self.throughput = 100 - np.random.poisson(5)
            self.delay = 17 + np.random.poisson(5)
        elif self.link_type == 1:
            self.throughput = 100 - np.random.poisson(5)
            self.delay = 17 + np.random.poisson(7)
    def getScore(self):
        self.score = self.throughput/100 + 17/self.delay
        return self.score
    
    def getCost(self):
        self.cost = 2/self.getScore()
        return self.cost

# def graph_init():
# 	size = 20
# 	sz = int(size / 2)
# 	G = nx.grid_2d_graph(sz, sz)
# 	pos = list(G.nodes())
# 	for i in range(sz * sz):
# 		if pos[i][0] % 2 != 0:
# 			pos[i] = (pos[i][0], pos[i][1] + 0.5)
# 		pos[i] = (pos[i][0] * 2, int(pos[i][1] * 2))

# 	G_new = nx.Graph()
# 	plt.figure(figsize=(sz, sz))
# 	pos1 = {(x, y): (x, y) for x, y in G_new.nodes()}

# 	link = ISL(1)

# 	G_new.add_weighted_edges_from([
# 		((x, y), (x + 2, y + 1), link.getCost())
# 		for x in range(0, size - 2, 2)
# 		for y in range(1, size - 1, 2)
# 		if (x / 2) % 2 != 0] + [
# 		((x, y), (x + 2, y + 1), link.getCost())
# 		for x in range(0, size - 2, 2)
# 		for y in range(0, size - 1, 2)
# 		if (x / 2) % 2 == 0] + [
# 		((x, y), (x + 2, y - 1), link.getCost())
# 		for x in range(0, size - 4, 2)
# 		for y in range(1, size, 2)
# 		if (x / 2) % 2 != 0] + [
# 		((x, y), (x + 2, y - 1), link.getCost())
# 		for x in range(0, size - 2, 2)
# 		for y in range(2, size, 2)
# 		if (x / 2) % 2 == 0] + [
# 		((x, y), (x, y + 2), 1)
# 		for x in range(0, size - 2, 2)
# 		for y in range(0, size - 2, 2)
# 		if (x / 2) % 2 == 0] + [
# 		((x, y), (x, y + 2), 1)
# 		for x in range(0, size - 1, 2)
# 		for y in range(1, size - 2, 2)
# 		if (x / 2) % 2 != 0])
	
# 	nx.draw(G_new, pos=pos1,
# 			node_color='lightblue',
# 			with_labels=True,
# 			node_size=600)
# 	nx.draw(G_new, pos=pos1, node_color="lightblue", with_labels=True, node_size=600)
# 	for i in node_rem:
# 		color = "%06x" % random.randint(0, 0xFFFFFF)
# 		color = "#"+color
# 		nx.draw(G_new, pos=pos1, nodelist=i, node_color=color, with_labels=True, node_size=600)
# 		labels = nx.get_edge_attributes(G_new,'weight')
# 		nx.draw_networkx_edge_labels(G_new,pos1,edge_labels=labels)
	
# 	plt.show()

def graph_init():
	size = 20
	sz = int(size / 2)
	G = nx.grid_2d_graph(sz, sz)

	pos = list(G.nodes())

	for i in range(sz * sz):
		if pos[i][0] % 2 != 0:
			pos[i] = (pos[i][0], pos[i][1] + 0.5)
		pos[i] = (pos[i][0] * 2, int(pos[i][1] * 2))

	G_new = nx.Graph()
	G_new2 = nx.Graph()

	G_new.add_nodes_from(pos)
	G_new2.add_nodes_from(pos)


	# plt.figure(figsize=(sz, sz))

	# pos1 = {(x, y): (x, y) for x, y in G_new.nodes()}

	link = ISL(1)

	G_new.add_weighted_edges_from([
		((x, y), (x + 2, y + 1), link.getCost())
		for x in range(0, size - 2, 2)
		for y in range(1, size - 1, 2)
		if (x / 2) % 2 != 0] + [
		((x, y), (x + 2, y + 1), link.getCost())
		for x in range(0, size - 2, 2)
		for y in range(0, size - 1, 2)
		if (x / 2) % 2 == 0] + [
		((x, y), (x + 2, y - 1), link.getCost())
		for x in range(0, size - 4, 2)
		for y in range(1, size, 2)
		if (x / 2) % 2 != 0] + [
		((x, y), (x + 2, y - 1), link.getCost())
		for x in range(0, size - 2, 2)
		for y in range(2, size, 2)
		if (x / 2) % 2 == 0] + [
		((x, y), (x, y + 2), 1)
		for x in range(0, size - 2, 2)
		for y in range(0, size - 2, 2)
		if (x / 2) % 2 == 0] + [
		((x, y), (x, y + 2), 1)
		for x in range(0, size - 1, 2)
		for y in range(1, size - 2, 2)
		if (x / 2) % 2 != 0])
	
	G_new2.add_edges_from([
		((x, y), (x + 2, y + 1))
		for x in range(0, size - 2, 2)
		for y in range(1, size - 1, 2)
		if (x / 2) % 2 != 0] + [
		((x, y), (x + 2, y + 1))
		for x in range(0, size - 2, 2)
		for y in range(0, size - 1, 2)
		if (x / 2) % 2 == 0] + [
		((x, y), (x + 2, y - 1))
		for x in range(0, size - 4, 2)
		for y in range(1, size, 2)
		if (x / 2) % 2 != 0] + [
		((x, y), (x + 2, y - 1))
		for x in range(0, size - 2, 2)
		for y in range(2, size, 2)
		if (x / 2) % 2 == 0] + [
		((x, y), (x, y + 2))
		for x in range(0, size - 2, 2)
		for y in range(0, size - 2, 2)
		if (x / 2) % 2 == 0] + [
		((x, y), (x, y + 2))
		for x in range(0, size - 1, 2)
		for y in range(1, size - 2, 2)
		if (x / 2) % 2 != 0])

	return G_new, G_new2

def d_graph(node_rem, G_new):
	
	line = node_rem

	# print(line)
	b = time.time()
	d_path = nx.dijkstra_path(G_new, line[0], line[-1])
	e = time.time()

	print('dijkstra', e-b, len(G_new.nodes()))

	print(d_path)

	return d_path

def get_short_graph(node_rem, G):
	b = time.time()
	G_new = nx.Graph(G)
	# e = time.time()
	# nx.draw(G_new, pos=pos1,
	# 		node_color='lightblue',
	# 		with_labels=True,
	# 		node_size=600)
	# # G_new.add_edge(a,b, edge_color='b',width = 6)
	# # nx.draw(G_new, pos=pos1, node_color="lightblue", with_labels=True, node_size=600)
	# for i in node_rem:
	# 	color = "%06x" % random.randint(0, 0xFFFFFF)
	# 	color = "#"+color
	# 	nx.draw(G_new, pos=pos1, nodelist=i, node_color=color, with_labels=True, node_size=600)
	# 	labels = nx.get_edge_attributes(G_new,'weight')
	# 	nx.draw_networkx_edge_labels(G_new,pos1,edge_labels=labels)
	
	# plt.show()
	
	# for p in range(sats[0]-1):
	# 	for q in range(sats[1]-1):
	# 		if p % 2 != 0:
	# 			link = ISL(1)
	# 			G_new[(p,q)][(p+1,q+1)]['weight'] = link.getCost()
	# 			print(G.get_edge_data((p,q),(p+1,q+1)))

	# for p in range(sats[0]-1):
	# 	for q in range(sats[1]-1):
	# 		if p % 2 == 0:
	# 			link = ISL(1)
	# 			G_new[(p+1,q)][(p,q+1)]['weight'] = link.getCost()
	# 			print(G.get_edge_data((p+1,q),(p,q+1)))

	# print(node_rem)
	line = node_rem[0]

	dr1, dr2, dl1, dl2, vu, vd = 0, 0, 0, 0, 0, 0

	a1, b1 = line[0][0], line[0][1]

	for l in range(len(line)):
		if l == 0:
			continue
		a2, b2 = line[l][0], line[l][1]
		if a2 - a1 > 0:
			if b2 - b1 > 0:
				dr1 += 1
			elif b2 - b1 < 0:
				dr2 += 1
		elif a2 - a1 < 0:
			if b2 - b1 > 0:
				dl1 += 1
			elif b2 - b1 < 0:
				dl2 += 1
		else:
			if b2 - b1 > 0:
				vu += 1
			elif b2 - b1 < 0:
				vd += 1
		a1, b1 = a2, b2
	res_nodes = []
	if vu == 0: 
		if dr2 > 0:
			for i in range(vd+1):
				x, y = line[0][0], line[0][1]
				y -= 2*i
				for j in range(dr2+1):
					res_nodes.append((x, y))
					# print((x,y), end=" ")
					x += 2
					y -= 1
				# print()
		elif dl2 > 0:
			for i in range(vd+1):
				x, y = line[0][0], line[0][1]
				y -= 2*i
				for j in range(dl2+1):
					res_nodes.append((x, y))
					# print((x,y), end=" ")
					x -= 2
					y -= 1
				# print()
	elif vd == 0:
		if dr1 > 0:
			for i in range(vu+1):
				x, y = line[0][0], line[0][1]
				y += 2*i
				for j in range(dr1+1):
					res_nodes.append((x, y))
					# print((x,y), end=" ")
					x += 2
					y += 1
				# print()
		elif dl1 > 0:
			for i in range(vu+1):
				x, y = line[0][0], line[0][1]
				y += 2*i
				for j in range(dl1+1):
					res_nodes.append((x, y))
					# print((x,y), end=" ")
					x -= 2
					y += 1
				# print()
	# print(res_nodes)

	rem_nodes = set(G_new.nodes() - res_nodes)
	G_new.remove_nodes_from(rem_nodes)
	e = time.time()

	b2 = time.time()
	d_path = nx.dijkstra_path(G_new, line[0], line[-1])
	e2 = time.time()

	# b3 = time.time()

	# if (4, 4) in rem_nodes:
	# 	print(True)
	# 	G_new.remove_node((16, 2))
	# d_path2 = nx.dijkstra_path(G_new, line[0], line[-1])

	# e3 = time.time()

	print('fybrrLink', e-b, e2-b2, len(G_new.nodes()))

	print(d_path)

	return d_path, (e-b)


def get_line(start, end):
	x1, y1 = start
	x2, y2 = end

	dx = abs(x2 - x1)
	dy = abs(y2 - y1)

	is_steep = dy > dx

	# Swap start and end points if necessary and store swap state
	swapped = False
	if x1 > x2:
		x1, x2 = x2, x1
		y1, y2 = y2, y1
		swapped = True

	# Recalculate differentials
	dx = abs(x2 - x1)
	dy = abs(y2 - y1)

	decision_var = 2*dy - dx

	is_source_up = False

	if y1 < y2:
		ystep = 2
	else:
		is_source_up = True
		ystep = -2

	# Iterate over bounding box generating points between start and end
	x, y = x1, y1
	points = []
	if is_steep:
		while x <= x2+1:
			if not is_source_up and y > y2:
				break
			elif is_source_up and y < y2:
				break
			points.append((x, y))

			if decision_var < 0:
				decision_var += 4*dy
				x += 1
			else:
				decision_var += 2*(dy-2*dx)
				y += ystep
			x += 1
	else:		
		while x <= x2:
			points.append((x, y))

			if decision_var < 0:
				decision_var += 4*dy
				x += 1
			else:
				decision_var += 2*(dy-2*dx)
				y += ystep
			x += 1
	if points[-1] != (x2, y2):
		extra_point = get_line(points[-1], (x2, y2))
		points.extend(extra_point[1:])

	if swapped:
		points.reverse()
	return points

if __name__ == "__main__":
	# F = [(0, 0), (16, 16)]
	df = pd.read_csv("TestCases.csv")
	l1 = list(zip(df['Source X'], df['Source Y']))
	l2 = list(zip(df['Destination X'], df['Destination Y']))
	result = [None]*(len(l1)+len(l2))
	result[::2] = l1
	result[1::2] = l2
	F = result
	# F = [(0, 0), (2, 3), (0, 0), (4, 4), (0, 0), (6, 5), (0, 0), (8, 8), (0, 0), (10, 11), (0, 0), (14, 13), (0, 0), (16, 16), (0, 0), (18, 19)]
	# F.reverse()
	F_new = []
	lines = []
	t = []
	G, G2 = graph_init()
	for j in F:
		p = (j[1], j[0])
		F_new.append(p)
	for i in range(0, len(F_new)-1, 2):
		# print("Source Coordinate \t: ", F[i])
		# print("Destination Coordinate \t: ", F[i+1])
		
		begin1 = time.time()
		line = get_line(F_new[i], F_new[i + 1])		#bresenham
		end1 = time.time()

		line_new = []
		for j in line:
			p = (j[1], j[0])
			line_new.append(p)
		
		# print("Path	\t \t: ", line_new)
		# begin2 = time.time()
		# path, t2 = get_short_graph([line_new], G)
		# end2 = time.time()

		beginD = time.time()
		lineD = d_graph([F[i], F[i + 1]], G2)		#dijkstra
		endD = time.time()

		# print(F[i], F[i + 1])
		# print('fybrrLink : ' + str(line_new))
		# print('dijkstra : ' + str(lineD))

		# t.append({"ISLs":len(path)-1, "fybrrLink": (end1-begin1), "bres+dijk": (end2-begin2)+(end1-begin1), "dijkstra": endD-beginD})
		# t.append({"ISLs":len(path)-1, "fybrrLink": ((end1-begin1) + (end2-begin2) - t2)*1000, "dijkstra": (endD-beginD)*1000})
		t.append({"ISLs":len(line)-1, "bresenham": ((end1-begin1))*1000, "dijkstra": (endD-beginD)*1000})
	
	# print(t)

	df = pd.DataFrame(t)


	df.to_csv('time-file.csv')

	df = df.groupby(['ISLs']).mean().reset_index()

	df.plot("ISLs", ['dijkstra', 'bresenham'])
	plt.ylabel("Average time taken to compute route (ms)")
	plt.xlabel("Number of ISLs in the route")
	plt.title("Weighted QoS-Aware Best Path Algorithm Time Comparison")

	plt.show()
