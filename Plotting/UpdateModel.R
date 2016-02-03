library(methods)
library(DBI)
library("RSQLite")
#args <- commandArgs(trailingOnly = TRUE)
#impactId <- args[1]

updateFunction <- function(impactId, con){
	#con = dbConnect(drv=RSQLite::SQLite(), dbname=dbname)
	samples <- dbGetQuery(conn=con,
		statement=paste("SELECT RspTime, Resources, NewRspTime
			FROM Samples WHERE Impact=", impactId, ";"))

	#print(samples$NewRspTime)

	fit <- lm(samples$NewRspTime ~ samples$RspTime + samples$Resources)

	wRspTime <- summary(fit)$coefficients[2, 1]
	wResources <- summary(fit)$coefficients[3, 1]
	intersection <- summary(fit)$coefficients[1, 1]

	updateQuery <- paste("UPDATE Impact_Functions SET weightRspTime =",
		wRspTime, ", weightResources=", wResources,
		", intersection=", intersection,
		"WHERE ImpactId=", impactId, ";")

	print(updateQuery)
	dbGetQuery(con, updateQuery)
}

updateFunctions <- function(dbname = "../db.sqlite") {
	con = dbConnect(drv=RSQLite::SQLite(), dbname=dbname)
	# > 1 because first function is NO_FUNC
	idsTable <- dbGetQuery(conn=con, statement="SELECT ImpactId
		FROM Impact_Functions
		WHERE ImpactId > 1;")
	ids <- idsTable$ImpactId
	dim <- length(ids)

	#print(ids)

	for (i in 1:dim) {
		updateFunction(ids[i], con)
	}
	dbDisconnect(con)
}

updateFunctions("/Users/francisco/Documents/Working-dir/ModelUpdate/db.sqlite")