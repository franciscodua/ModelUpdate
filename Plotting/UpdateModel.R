library(methods)
library(DBI)
library("RSQLite")

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

	wRspTime <- summary(fit)$coefficients[2, 1]
	wResources <- summary(fit)$coefficients[3, 1]
	intersection <- summary(fit)$coefficients[1, 1]

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
updateFunctions("/Users/francisco/Documents/Working-dir/ModelUpdate/db.sqlite")