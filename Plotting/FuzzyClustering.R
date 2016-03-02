library(methods)
library(DBI)
library("RSQLite")
library(frbs)
library(MASS)

distance <- function(vec1, vec2) {
	if (length(vec1) != length(vec2)) {
		stop("Vectors must have same length")
	}
	vector <- vec2 - vec1
	# eucliedean norm
	result <- sum(abs(vector)^2)^(1/2)
	result
}

normalizeFunction <- function(x) {
	(x - min(x)) / (max(x) - min(x))
}

normalizeData <- function(data) {
	data.norm <- lapply(data, normalizeFunction)
	data.frame(data.norm)
}

membershipFunction <- function(alpha, input, input.cluster) {
	distance <- distance(input, input.cluster)
	# formula from Chiu's paper
	result <- exp(alpha * distance^2)
	result
}

getClusters <- function(data) {
	range.data <- matrix(c(0, 1), ncol = dim(data)[2], nrow = 2)
	method.type <- "SBC"
	control.SBC <- list(r.a = 1, eps.high = 0.5, eps.low = 0.15, name = "Sim-0")
	samples <- data.frame(data)
	object.SBC <- frbs.learn(samples, range.data, method.type, control.SBC)
	object.SBC
}

# getClusters

# miu(matrix): samples.length X clusters.length
# and fill miu
computeMiu <- function(clusters, samples, alpha) {
	input.length <- dim(samples)[2] - 1
	clusters.length <- dim(clusters)[1]
	samples.length <- dim(samples)[1]
	# table miu
	# miu[i, j] corresponds to membership function from samlpe i to cluster j
	miu <- matrix(, nrow = samples.length, ncol = clusters.length)

	for(i in 1:samples.length) {
		for(j in 1:clusters.length) {
			input <- samples[i, 1:input.length]
			cluster <- clusters[j, 1:input.length]
			miu[i, j] <- membershipFunction(alpha, input, cluster)
		}
	}
	miu
}

# output = samples$output

computeRaw <- function(miu, sample.id, cluster.id) {
	raw <- miu[sample.id, cluster.id]
	raw <- raw / sum(miu[sample.id,])
	raw
}

# A(matrix): raw(i,j) * x | raw(i,j) | ...
computeMatrix <- function(miu, clusters, samples) {
	clusters.length <- dim(clusters)[1]
	samples.length <- dim(samples)[1]
	input.length <- dim(samples)[2] - 1
	# number of input dimensions + constant for every cluster
	ncols = (input.length + 1) * clusters.length

	A <- matrix(, nrow = samples.length, ncol = ncols)

	for(i in 1:samples.length) {
		for(j in 1:clusters.length) {
			# compute raw and fill |raw * vars|raw
			raw <- computeRaw(miu, i, j)
			col <- (j - 1) * (input.length + 1) + 1
			for(k in 1:input.length) {
				A[i, col] <- raw * samples[i, k]
				col <- col + 1
			}
			A[i, col] <- raw
		}
	}
	A
}

computeX <- function(A, output) {
	ginv(t(A)%*%A)%*%t(A)%*%output
}

# Plots results: samples, clusters centres and linear functions
# Arguments:
#	data: data frame containing 2D samples
#	clusters: matrix with 2D points X, Y
#	functions: data frame with a and b from y = ax + b
plotResults <- function(data, clusters, functions){
	plot(x=data$x, y=data$y)

	fNames <- c(paste("Cluster: ", 1:length(clusters[,1]), sep=""))

	legend(0, y=1, fNames, pch=19, col=c(1:length(clusters[,1]) + 1))

	for (i in 1:length(clusters[,1])) {
		colour <- i + 1
		points(x=clusters[i,1], y=clusters[i, 2], col=colour, pch=19)
		abline(a=functions[i,]$b, b=functions[i,]$a, col=colour)
	}
}

test <- function() {
	# samples: y = x + 10
	samples <- data.frame(x=c(1:100), y=c(1:100 + 10))
	samples <- normalizeData(samples)

	clusters <- getClusters(samples)$cls
	# miu[i, j] corresponds to membership function from samlpe i to cluster j
	miu <- computeMiu(clusters, samples, 1)
	# computes A from o = A x
	A <- computeMatrix(miu, clusters, samples)

	output <- samples$y
	# x is a vector(2D) where each pair is the weight(1) and intercept(2)
	x <- computeX(A, output)

	a_v <- c(1:length(clusters[, 1]))
	b_v <- c(1:length(clusters[, 1]))
	for (i in 1:length(clusters[,1])) {
		index <- i * 2 - 1
		a <- x[index]
		b <- x[index + 1]
		print (paste(i, ": y = ", a, "x + ", b, sep=""))
		a_v[i] <- a
		b_v[i] <- b
	}
	functions <- data.frame(a=a_v, b=b_v)
	plotResults(samples, clusters, functions)
}

# lm(output ~ A)