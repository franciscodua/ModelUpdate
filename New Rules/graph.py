import rpy2.robjects as ro
def graph(input_file, clusters):
	ro.r('data <- read.csv("' + input_file + '", header=FALSE)')
	ro.r('jpeg("' + input_file.split('/')[-1].split('.')[0] + '.jpg")')
	ro.r('plot(x=data$V1, y=data$V2)')
	for c in clusters:
		function = c.centroid.coords
		ro.r('abline(a=' + str(function[1]) +
			', b=' + str(function[0]) + ', col="red")')
	ro.r('dev.off()')