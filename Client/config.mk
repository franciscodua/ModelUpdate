##############################
#    Environment variables   #
##############################

JAVA  = $(JAVA_HOME)/usr/bin/java
JAVAC = $(JAVA_HOME)/usr/bin/javac
#JAVAC = /usr/usr/bin/jikes
JAVACOPTS =
# +E -deprecation
JAVACC = $(JAVAC) $(JAVACOPTS)
RMIC = $(JAVA_HOME)/usr/bin/rmic
RMIREGISTRY= $(JAVA_HOME)/usr/bin/rmiregistry
CLASSPATH = .:$(J2EE_HOME)/lib/j2ee.jar:$(JAVA_HOME)/jre/lib/rt.jar:/cluster/opt/jakarta-tomcat-3.2.3/lib/servlet.jar:$(PWD)
JAVADOC = $(JAVA_HOME)/usr/bin/javadoc
JAR = $(JAVA_HOME)/usr/bin/jar

GENIC = ${JONAS_ROOT}/usr/bin/unix/GenIC

MAKE = gmake
CP = /bin/cp
RM = /bin/rm
MKDIR = /bin/mkdir


# EJB server: supported values are jonas or jboss
EJB_SERVER = jonas

# DB server: supported values are MySQL or PostgreSQL
DB_SERVER = MySQL

%.class: %.java
	${JAVACC} -classpath ${CLASSPATH} $<

