#import rpy2.robjects as ro
import matplotlib.pyplot as plt
import numpy as np

from cluster import Cluster


#def graph(input_file, clusters):
	#ro.r('data <- read.csv("' + input_file + '", header=FALSE)')
	#ro.r('jpeg("' + input_file.split('/')[-1].split('.')[0] + '.jpg")')
	#ro.r('plot(x=data$V1, y=data$V2)')
	#for c in clusters:
	#	function = c.centroid.coords
	#	ro.r('abline(a=' + str(function[1]) +
	#		', b=' + str(function[0]) + ', col="red")')
	#ro.r('dev.off()')


def new_graph(clusters, output_graph):
	colors = ['b', 'g', 'r', 'c', 'm', 'y', 'k']
	colors_len = len(colors)
	color_index = 0

	if clusters[0].n != 2:
		raise Exception("ILLEGAL: Plot only works with 2 Dimensions")

	for c in clusters:
		color = colors[color_index]
		points_x = [p.coords[0] for p in c.points]
		points_y = [p.coords[1] for p in c.points]

		plt.plot(points_x, points_y, color + 'o')

		m = c.centroid.coords[0]
		b = c.centroid.coords[1]

		plt.plot(points_x, m * np.asarray(points_x) + b, color + '-')

		color_index = (color_index + 1) % colors_len
	plt.savefig(output_graph)
	plt.show()