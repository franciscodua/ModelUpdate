library(methods)
library(DBI)
library("RSQLite")

library(frbs)

getSamples <- function(con, impactId) {
	samples <- dbGetQuery(conn=con,
		statement=paste("SELECT RspTime, Resources, NewRspTime
			FROM Samples WHERE Impact=", impactId, ";"))
	samples
}

normalize <- function(x) {
	(x - min(x)) / (max(x) - min(x))
}

newRules <- function(con) {
	# retrieves samples with no function associated (impactId = 1)
	samples <- getSamples(con, 2)
	# normalized data
	samples.norm <- data.frame (lapply(samples, normalize))
	# data should be normalized before
	range.data <- matrix(c(0, 1, 0, 1, 0, 1), ncol = 3)
	# subtractive clustering
	method.type <- "SBC"
	# standard values
	control.SBC <- list(r.a = 1, eps.high = 0.5, eps.low = 0.15, name = "Sim-0")

	object.SBC <- frbs.learn(samples.norm, range.data, method.type, control.SBC)
	object.SBC
}

updateFunction <- function(impactId, minRanges, maxRanges, con){
# Updates the Impact_Functions tables with the new values for the
# impact Function with impactId
#
# Args:
#	impactId: identifier of Impact Function being updated
#	minRanges: vector that has the minRanges for every variable
#	maxRanges: vector that has the maxRanges for every variable
	samples <- dbGetQuery(conn=con,
		statement=paste("SELECT RspTime, Resources, NewRspTime
			FROM Samples WHERE Impact=", impactId, ";"))

	samples.in.range <- dbGetQuery(conn=con,
		statement=paste("SELECT count(*) FROM Samples
			WHERE RspTime >=", minRanges[1], "AND
			RspTime <=", maxRanges[1], "AND
			Resources >=", minRanges[2], "AND
			Resources <=", maxRanges[2], ";"))

	# number of samples = number of rows
	n.impact.samples <- length(samples$RspTime)
	# only element returned by query
	n.samples.in.range <- samples.in.range[1, 1]

	# P(impact | state) = #(impact, state) / #(state)
	# where state = within range
	probability <- n.impact.samples / n.samples.in.range

	# linear regression
	fit <- lm(samples$NewRspTime ~ samples$RspTime + samples$Resources)

	coefficients <- coef(fit)

	wRspTime <- coefficients[[2]]
	wResources <- coefficients[[3]]
	intersection <- coefficients[[1]]

	#wRspTime <- summary(fit)$coefficients[2, 1]
	#wResources <- summary(fit)$coefficients[3, 1]
	#intersection <- summary(fit)$coefficients[1, 1]

	print (paste("Function" , impactId,":", wRspTime, "RspTime +", wResources,
		"Resources +", intersection))

	# updates Impact Function details in DB with new values
	updateQuery <- paste("UPDATE Impact_Functions SET weightRspTime =",
		wRspTime, ", weightResources =", wResources,
		", intersection =", intersection,
		", probability =", probability,
		"WHERE ImpactId =", impactId, ";")

	dbGetQuery(con, updateQuery)
}

updateFunctions <- function(dbname = "../db.sqlite") {
# Updates every Impact Function
#
# Args:
#	dbname: string that holds the name of the DB where data is

	con = dbConnect(drv=RSQLite::SQLite(), dbname=dbname)

	# > 1 because first function is NO_FUNC
	idsTable <- dbGetQuery(conn=con, statement="SELECT ImpactId,
		minRangeRspTime, maxRangeRspTime,
		minRangeResources, maxRangeResources
		FROM Impact_Functions
		WHERE ImpactId > 1;")

	ids <- idsTable$ImpactId

	# matrix with min ranges
	minRanges <- matrix(c(idsTable$minRangeRspTime, idsTable$minRangeResources),
		nrow=length(idsTable$minRangeRspTime))
	# matrix with max ranges
	maxRanges <- matrix(c(idsTable$maxRangeRspTime, idsTable$maxRangeResources),
		nrow=length(idsTable$maxRangeRspTime))

	dim <- length(ids)

	for (i in 1:dim) {
		updateFunction(ids[i], minRanges[i,], maxRanges[i,], con)
	}
	dbDisconnect(con)
}

# call to be used when Rscript is used
updateFunctions("/Users/francisco/Documents/IST/Working-dir/ModelUpdate/db.sqlite")