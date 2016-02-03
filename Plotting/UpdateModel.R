library("RSQLite")

args <- commandArgs(trailingOnly = TRUE)

impactId <- args[1]

con = dbConnect(drv=RSQLite::SQLite(), dbname="../db.sqlite")

samples <- dbGetQuery(conn=con,
	statement=paste("SELECT RspTime, Resources, NewRspTime
		FROM Samples WHERE Impact=", impactId, ";"))

fit <- lm(samples$NewRspTime ~ samples$RspTime + samples$Resources)

wRspTime <- summary(fit)$coefficients[2, 1]
wResources <- summary(fit)$coefficients[3, 1]
intersect <- summary(fit)$coefficients[1, 1]

updateQuery <- paste("UPDATE Impact_Functions SET weightRspTime =",
	wRspTime, ", weightResources=", wResources,
	"WHERE ImpactId=", impactId, ";")

dbGetQuery(con, updateQuery)

print(updateQuery)

dbDisconnect(con)