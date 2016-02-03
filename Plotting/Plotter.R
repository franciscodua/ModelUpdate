# R script to create 3D plot of samples
library("RSQLite")
library(scatterplot3d)

con = dbConnect(drv=RSQLite::SQLite(), dbname="../db.sqlite")

syntheticSamples <- dbGetQuery(conn=con,
	statement="SELECT * FROM Samples WHERE Synthetic=0")

s3d <- scatterplot3d(syntheticSamples$RspTime,
	syntheticSamples$Resources,
	syntheticSamples$NewRspTime,
	main="Samples",
	xlab="Response Time",
	ylab="Resources",
	zlab="Expected Response Time",
	color="blue", pch=19,
	type="h")

realSamples <- dbGetQuery(conn=con,
	statement="SELECT * FROM Samples WHERE Synthetic=1")

s3d$points3d(realSamples$RspTime,
	realSamples$Resources,
	realSamples$NewRspTime,
	col="red",
	pch=19,
	type="h")

legend("right",
	legend=c("Synthetic", "Real"),
	col=c("blue", "red"),
	pch=c(19, 19),
	inset = -0.11, bty="n", xpd = TRUE)

dbDisconnect(con)
